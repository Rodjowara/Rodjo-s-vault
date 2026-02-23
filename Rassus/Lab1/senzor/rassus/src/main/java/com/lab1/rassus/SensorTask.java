package com.lab1.rassus;

import com.lab1.rassus.model.Reading;
import com.lab1.rassus.model.SensorInfo;

import java.nio.file.Files;
import java.nio.file.Path;
import java.util.List;

public class SensorTask implements Runnable {

    private final int sensorId;
    private final double latitude;
    private final double longitude;
    private final SensorGrpcServer grpcServer;
    private final SensorRestClient restClient;
    private final SensorInfo neighbor; // fiksni susjed
    private final List<String> readings;
    private final long startTimeMillis;

    public SensorTask(int sensorId, double latitude, double longitude,
                      SensorGrpcServer grpcServer, SensorRestClient restClient,
                      SensorInfo neighbor) throws Exception {
        this.sensorId = sensorId;
        this.latitude = latitude;
        this.longitude = longitude;
        this.grpcServer = grpcServer;
        this.restClient = restClient;
        this.neighbor = neighbor;
        this.startTimeMillis = System.currentTimeMillis();

        this.readings = Files.readAllLines(Path.of("readings[7].csv"));
    }

    @Override
    public void run() {
        try {
            int index = (int)(((System.currentTimeMillis() - startTimeMillis)/1000) % 100) + 1;
            String[] parts = readings.get(index).trim().split(",");
            if (parts.length < 5) return;

            Reading myReading = new Reading(
                    parseDoubleSafe(parts[0]),
                    parseDoubleSafe(parts[1]),
                    parseDoubleSafe(parts[2]),
                    parseDoubleSafe(parts[3]),
                    parseDoubleSafe(parts[4]),
                    (parts.length > 5) ? Double.parseDouble(parts[5]) : 0
            );

            System.out.println("My values: " + myReading.getTemperature() + " " + myReading.getPressure()
                    + " " + myReading.getHumidity() + " " + myReading.getCo() + " " +  myReading.getNo2()
                    + " " + myReading.getSo2());

            grpcServer.updateCurrentReading(myReading);

            Reading neighborReading = null;
            if (neighbor != null) {
                SensorGrpcClient grpcClient = new SensorGrpcClient(neighbor.getIp(), neighbor.getPort());
                neighborReading = grpcClient.getNeighborReading();
                grpcClient.shutdown();
            }

            if(neighborReading != null) {
                System.out.println("Neighbor values: " + neighborReading.getTemperature() + " " + neighborReading.getPressure()
                        + " " + neighborReading.getHumidity() + " " + neighborReading.getCo() + " " +  neighborReading.getNo2()
                        + " " + neighborReading.getSo2());
            }else{
                System.out.println("Neighbor values: null");
            }

            Reading calibrated = calibrate(myReading, neighborReading);

            System.out.println("Sending values: " + calibrated.getTemperature() + " " + calibrated.getPressure()
                    + " " + calibrated.getHumidity() + " " + calibrated.getCo() + " " +  calibrated.getNo2()
                    + " " + calibrated.getSo2());

            System.out.println("\n");

            restClient.sendReading(sensorId, calibrated, latitude, longitude);

        } catch (Exception e) {
            System.err.println("Error in SensorTask: " + e.getMessage());
        }
    }

    private Reading calibrate(Reading my, Reading neighbor) {
        if (neighbor == null) return my;

        return new Reading(
                averageSafe(my.getTemperature(), neighbor.getTemperature()),
                averageSafe(my.getPressure(), neighbor.getPressure()),
                averageSafe(my.getHumidity(), neighbor.getHumidity()),
                averageSafe(my.getCo(), neighbor.getCo()),
                averageSafe(my.getNo2(), neighbor.getNo2()),
                averageSafe(my.getSo2(), neighbor.getSo2())
        );
    }

    private double parseDoubleSafe(String s) {
        if (s == null || s.isEmpty()) return 0.0;
        return Double.parseDouble(s);
    }

    private Double averageSafe(Double a, Double b) {
        if (a == null || a == 0) return b;
        if (b == null || b == 0) return a;
        return (a + b) / 2.0;
    }
}