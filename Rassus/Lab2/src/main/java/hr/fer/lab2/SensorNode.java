package hr.fer.lab2;

import hr.fer.lab2.kafka.KafkaConfig;
import hr.fer.lab2.model.Reading;
import hr.fer.lab2.time.VectorClock;
import hr.fer.lab2.udp.EmulatedSystemClock;
import hr.fer.lab2.udp.SimpleSimulatedDataSocket;
import hr.fer.lab2.util.CsvReader;
import org.apache.kafka.clients.consumer.ConsumerRecord;
import org.apache.kafka.clients.consumer.KafkaConsumer;
import org.apache.kafka.clients.producer.KafkaProducer;
import org.apache.kafka.clients.producer.ProducerRecord;
import org.json.JSONObject;

import java.net.*;
import java.time.Duration;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;

/**
 * Sensor node main class. Run multiple times in separate processes.
 * Integrates scalar timestamp (EmulatedSystemClock) and vector clock.
 */
public class SensorNode {

    private static final String REGISTER_TOPIC = "Register";
    private static final String COMMAND_TOPIC = "Command";

    private final String id;
    private final int udpPort;
    private final KafkaProducer<String,String> producer;
    private final KafkaConsumer<String,String> consumer;
    private final EmulatedSystemClock clock = new EmulatedSystemClock();
    private final VectorClock vclock = new VectorClock();
    private final CsvReader csv;

    private final Map<String, InetSocketAddress> peers = new ConcurrentHashMap<>();
    private final Map<Long, String> sentPackets = new ConcurrentHashMap<>(); // seq -> payload
    private final Map<String, Set<Long>> receivedSeq = new ConcurrentHashMap<>(); // src -> seqs received (dedupe)
    private final List<Reading> allReadings = Collections.synchronizedList(new ArrayList<>());

    private volatile boolean running = true;
    private long seqCounter = 0;
    private final double LOSS_RATE = 0.3; // per spec
    private final int AVG_DELAY = 1000;

    // store readings for time window
    private final List<Reading> windowReadings = Collections.synchronizedList(new ArrayList<>());

    public static void main(String[] args) throws Exception {
        Scanner sc = new Scanner(System.in);
        System.out.print("Sensor id: ");
        String id = sc.nextLine().trim();
        System.out.print("UDP port: ");
        int port = Integer.parseInt(sc.nextLine().trim());
        new SensorNode(id, port).run();
    }

    public SensorNode(String id, int udpPort) throws Exception {
        this.id = id;
        this.udpPort = udpPort;
        this.producer = KafkaConfig.createProducer();
        this.consumer = KafkaConfig.createConsumer("sensor-" + id + "-" + UUID.randomUUID());
        this.consumer.subscribe(Arrays.asList(COMMAND_TOPIC, REGISTER_TOPIC));
        this.csv = new CsvReader("/readings.csv");

        // ensure our vector has an entry for ourselves so ordering is deterministic
        vclock.tick(id); // initializes id->1 (you might prefer 0; this ensures it exists)
    }

    public void run() throws Exception {
        System.out.println("Sensor " + id + " waiting for START...");
        waitForStart();

        // send registration
        JSONObject reg = new JSONObject();
        reg.put("id", id);
        reg.put("address", InetAddress.getLocalHost().getHostAddress());
        reg.put("port", String.valueOf(udpPort));
        producer.send(new ProducerRecord<>(REGISTER_TOPIC, reg.toString()));
        producer.flush();
        System.out.println("Registration published.");

        // collect peers for a short time (3s)
        collectRegistrations(3000);

        // start UDP socket receiver thread
        SimpleSimulatedDataSocket socket = new SimpleSimulatedDataSocket(udpPort, LOSS_RATE, AVG_DELAY);
        Thread recvThread = new Thread(() -> udpReceiveLoop(socket));
        recvThread.setDaemon(true);
        recvThread.start();

        // Main loops: sending readings (every 1s), retransmission, and 5s window processing
        Thread sendThread = new Thread(() -> {
            long startTime = System.currentTimeMillis();
            while (running) {
                long activeSeconds = (System.currentTimeMillis() - startTime) / 1000;
                try {
                    sendReading(activeSeconds, socket);
                } catch (Exception e) { e.printStackTrace(); }
                try { Thread.sleep(1000); } catch (InterruptedException ignored) {}
            }
        });
        sendThread.setDaemon(true);
        sendThread.start();

        Thread retransThread = new Thread(() -> {
            while (running) {
                try {
                    for (var e : sentPackets.entrySet()) {
                        long seq = e.getKey();
                        String payload = e.getValue();
                        byte[] buf = payload.getBytes();
                        for (InetSocketAddress peer : peers.values()) {
                            try {
                                DatagramPacket p = new DatagramPacket(buf, buf.length, peer.getAddress(), peer.getPort());
                                socket.send(p);
                            } catch (Exception ex) { ex.printStackTrace(); }
                        }
                    }
                    Thread.sleep(2000);
                } catch (InterruptedException ignored) {}
            }
        });
        retransThread.setDaemon(true);
        retransThread.start();

        Thread windowThread = new Thread(() -> {
            while (running) {
                try { Thread.sleep(5000); } catch (InterruptedException ignored) {}
                processWindow();
            }
        });
        windowThread.setDaemon(true);
        windowThread.start();

        // Glavna petlja za slušanje "Stop" i registracija
        boolean stopReceived = false;
        while (!stopReceived) {
            for (ConsumerRecord<String,String> rec : consumer.poll(Duration.ofMillis(200))) {
                if (rec.topic().equals(COMMAND_TOPIC) && rec.value().equalsIgnoreCase("Stop")) {
                    System.out.println("Received STOP. Shutting down sensor " + id);
                    stopReceived = true;
                    break;
                } else if (rec.topic().equals(REGISTER_TOPIC)) {
                    try {
                        JSONObject o = new JSONObject(rec.value());
                        String otherId = o.getString("id");
                        if (!otherId.equals(id)) {
                            peers.put(otherId, new InetSocketAddress(o.getString("address"), Integer.parseInt(o.getString("port"))));
                        }
                    } catch (Exception ignored) {}
                }
            }
        }

        // zaustavi sve petlje
        running = false;
        Thread.sleep(500); // da završe trenutni udp/thread poslovi
        socket.close();    // prekini UDP komunikaciju

        System.out.println("=== FINAL READINGS ===");
        List<Reading> copyAll;
        synchronized(allReadings) {
            copyAll = new ArrayList<>(allReadings);
        }
        copyAll.sort(Comparator.comparingLong(r -> r.scalarTimestamp)); // npr. po SCALAR
        for (Reading r : copyAll) {
            System.out.printf("src=%s seq=%d scalar=%d val=%.3f vec=%s%n",
                    r.sourceId, r.sendSeq, r.scalarTimestamp, r.value, r.vectorTimestamp.toString());
        }

        // zatvori Kafka klijente
        producer.close();
        consumer.close();
        System.out.println("Sensor " + id + " stopped.");
    }

    private void waitForStart() {
        consumer.poll(Duration.ofMillis(100));
        consumer.seekToEnd(Collections.emptyList());

        while (true) {
            for (ConsumerRecord<String,String> rec : consumer.poll(Duration.ofMillis(200))) {
                if (rec.topic().equals(COMMAND_TOPIC) && rec.value().equalsIgnoreCase("Start")) {
                    System.out.println("Start received.");
                    return;
                }
            }
        }
    }

    private void collectRegistrations(long millis) {
        long start = System.currentTimeMillis();
        System.out.println("Collecting registrations for " + millis + " ms...");
        while (System.currentTimeMillis() - start < millis) {
            for (ConsumerRecord<String,String> rec : consumer.poll(Duration.ofMillis(200))) {
                if (rec.topic().equals(REGISTER_TOPIC)) {
                    try {
                        JSONObject o = new JSONObject(rec.value());
                        String otherId = o.getString("id");
                        if (!otherId.equals(id)) {
                            peers.put(otherId, new InetSocketAddress(o.getString("address"), Integer.parseInt(o.getString("port"))));
                        }
                    } catch (Exception ignored) {}
                }
            }
        }
        System.out.println("Peers: " + peers.keySet());
    }

    private void udpReceiveLoop(SimpleSimulatedDataSocket socket) {
        byte[] buf = new byte[8192];
        while (running) {
            try {
                DatagramPacket p = new DatagramPacket(buf, buf.length);
                socket.receive(p);
                String msg = new String(p.getData(), 0, p.getLength());
                JSONObject o = new JSONObject(msg);
                String type = o.optString("type", "DATA");
                if ("ACK".equalsIgnoreCase(type)) {
                    String src = o.getString("src");
                    long seq = o.getLong("seq");
                    // remove from outstanding if present
                    sentPackets.remove(seq);
                } else { // DATA
                    String src = o.getString("src");
                    long seq = o.getLong("seq");
                    double val = o.getDouble("val");
                    long scalar = o.getLong("scalar");
                    JSONObject vec = o.getJSONObject("vec");
                    Map<String,Integer> receivedVec = VectorClock.fromJson(vec);

                    // dedupe
                    receivedSeq.putIfAbsent(src, ConcurrentHashMap.newKeySet());
                    Set<Long> s = receivedSeq.get(src);
                    boolean first = s.add(seq);
                    if (first) {
                        // create Reading and store
                        Map<String,Integer> vecCopy = new HashMap<>(receivedVec);
                        Reading r = new Reading(src, val, scalar, vecCopy, seq);
                        windowReadings.add(r);
                        allReadings.add(r);

                        // merge vector into local vclock, then tick local id (per vector clock receive rule)
                        vclock.merge(receivedVec);
                        vclock.tick(id);
                    } else {
                        // still need to update vector? some implementations may merge duplicate's vector as well
                        vclock.merge(receivedVec); // safe to call again
                    }

                    // send ACK back (even for duplicates)
                    String ack = Reading.ackJson(id, seq);
                    byte[] ackBuf = ack.getBytes();
                    InetSocketAddress replyTo = new InetSocketAddress(p.getAddress(), p.getPort());
                    DatagramPacket ackPacket = new DatagramPacket(ackBuf, ackBuf.length, replyTo.getAddress(), replyTo.getPort());
                    socket.send(ackPacket);
                }
            } catch (Exception e) {
                if (running) e.printStackTrace();
            }
        }
    }

    private void sendReading(long activeSeconds, SimpleSimulatedDataSocket socket) throws Exception {
        // generate reading from CSV
        int row = (int)(activeSeconds % 100) + 1;
        double val = csv.getReadingForRow(row);

        // tick own vector clock BEFORE sending (send rule)
        vclock.tick(id);
        Map<String,Integer> vcSnapshot = vclock.snapshot();

        long scalar = clock.currentTimeMillis();

        long seq = ++seqCounter;
        Reading r = new Reading(id, val, scalar, vcSnapshot, seq);
        String payload = r.toJson();

        // store in outstanding (for retransmission)
        sentPackets.put(seq, payload);

        // also add own reading to local window
        windowReadings.add(r);
        allReadings.add(r);

        byte[] buf = payload.getBytes();

        // send to all peers
        for (InetSocketAddress peer : peers.values()) {
            try {
                DatagramPacket p = new DatagramPacket(buf, buf.length, peer.getAddress(), peer.getPort());
                socket.send(p);
            } catch (Exception e) { e.printStackTrace(); }
        }
    }

    private void processWindow() {
        List<Reading> copy;
        synchronized (windowReadings) {
            copy = new ArrayList<>(windowReadings);
            windowReadings.clear();
        }
        if (copy.isEmpty()) {
            System.out.println("[" + id + "] No readings in window.");
            return;
        }

        // 1) Sort by SCALAR (ascending)
        copy.sort(Comparator.comparingLong(r -> r.scalarTimestamp));
        System.out.println("[" + id + "] Sorted by SCALAR:");
        for (Reading r : copy) {
            System.out.printf("  src=%s seq=%d scalar=%d val=%.3f vec=%s%n", r.sourceId, r.sendSeq, r.scalarTimestamp, r.value, r.vectorTimestamp.toString());
        }
        double sum=0; int cnt=0;
        for (Reading r : copy) { sum += r.value; cnt++; }
        System.out.printf("[%s] SCALAR window avg (n=%d) = %.4f%n", id, cnt, cnt==0?0:sum/cnt);

        // 2) Sort by VECTOR partial order: if a -> b then a before b; if concurrent use deterministic tie-break (scalar then src+seq)
        copy.sort((a,b) -> {
            boolean aBeforeB = VectorClock.isStrictlyBefore(a.vectorTimestamp, b.vectorTimestamp);
            boolean bBeforeA = VectorClock.isStrictlyBefore(b.vectorTimestamp, a.vectorTimestamp);
            if (aBeforeB && !bBeforeA) return -1;
            if (bBeforeA && !aBeforeB) return 1;
            // concurrent: deterministic tie-break
            int cmp = Long.compare(a.scalarTimestamp, b.scalarTimestamp);
            if (cmp != 0) return cmp;
            cmp = a.sourceId.compareTo(b.sourceId);
            if (cmp != 0) return cmp;
            return Long.compare(a.sendSeq, b.sendSeq);
        });

        System.out.println("[" + id + "] Sorted by VECTOR (partial order + tie-break):");
        for (Reading r : copy) {
            System.out.printf("  src=%s seq=%d vec=%s val=%.3f%n", r.sourceId, r.sendSeq, r.vectorTimestamp.toString(), r.value);
        }

        // average already printed above (same set)
    }
}