package hr.fer.lab2.model;

import java.util.Map;

public class Reading {
    public final String sourceId;
    public final double value;
    public final long scalarTimestamp;
    public final Map<String,Integer> vectorTimestamp;
    public final long sendSeq; // unique sequence number per sender

    public Reading(String sourceId, double value, long scalarTimestamp, Map<String,Integer> vectorTimestamp, long sendSeq) {
        this.sourceId = sourceId;
        this.value = value;
        this.scalarTimestamp = scalarTimestamp;
        this.vectorTimestamp = vectorTimestamp;
        this.sendSeq = sendSeq;
    }

    public String toJson() {
        StringBuilder vc = new StringBuilder("{");
        boolean f=true;
        for (var e : vectorTimestamp.entrySet()) {
            if (!f) vc.append(",");
            f=false;
            vc.append("\"").append(e.getKey()).append("\":").append(e.getValue());
        }
        vc.append("}");
        return String.format("{\"type\":\"DATA\",\"src\":\"%s\",\"val\":%s,\"scalar\":%d,\"vec\":%s,\"seq\":%d}",
                sourceId, Double.toString(value), scalarTimestamp, vc.toString(), sendSeq);
    }

    public static String ackJson(String src, long seq) {
        return String.format("{\"type\":\"ACK\",\"src\":\"%s\",\"seq\":%d}", src, seq);
    }
}