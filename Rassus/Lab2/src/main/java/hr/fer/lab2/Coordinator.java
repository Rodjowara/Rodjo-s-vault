package hr.fer.lab2;

import hr.fer.lab2.kafka.KafkaConfig;
import org.apache.kafka.clients.producer.KafkaProducer;
import org.apache.kafka.clients.producer.ProducerRecord;

import java.util.Scanner;

public class Coordinator {

    private static final String COMMAND_TOPIC = "Command";

    public static void main(String[] args) {
        KafkaProducer<String, String> p = KafkaConfig.createProducer();
        Scanner sc = new Scanner(System.in);
        System.out.println("Coordinator ready. Type 'start' to launch sensors, 'stop' to stop them and exit.");

        while (true) {
            String in = sc.nextLine().trim().toLowerCase();
            if (in.equals("start")) {
                p.send(new ProducerRecord<>(COMMAND_TOPIC, "Start"));
                System.out.println("Start sent.");
            } else if (in.equals("stop")) {
                p.send(new ProducerRecord<>(COMMAND_TOPIC, "Stop"));
                System.out.println("Stop sent; exiting coordinator.");
                p.flush();
                p.close();
                break;
            } else {
                System.out.println("Unknown. Use 'start' or 'stop'.");
            }
        }
    }
}