package com.lab1.rassus;

import com.lab1.rassus.model.SensorInfo;
import io.grpc.Server;
import io.grpc.ServerBuilder;

import java.util.Random;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

public class SensorMain {
    public static void main(String[] args) throws Exception {
        int grpcPort = 50051;
        if (args.length > 0) {
            try {
                grpcPort = Integer.parseInt(args[0]);
            } catch (NumberFormatException e) {
                System.err.println("Invalid port argument, using default 50051");
            }
        }

        double latitude = 45.75 + new Random().nextDouble() * 0.1;
        double longitude = 15.87 + new Random().nextDouble() * 0.13;

        // start gRPC server
        SensorGrpcServer grpcService = new SensorGrpcServer();
        Server grpcServer = ServerBuilder.forPort(grpcPort)
                .addService(grpcService)
                .build();

        grpcServer.start();
        System.out.println("gRPC server started on port " + grpcPort);

        // REST client
        SensorRestClient restClient = new SensorRestClient("http://localhost:8080");

        // registriraj senzor
        Integer sensorId = restClient.registerSensor(latitude, longitude, grpcPort);
        if (sensorId == null) {
            System.err.println("Failed to register sensor, shutting down.");
            grpcServer.shutdownNow();
            return;
        }

        Thread.sleep(15000);

        // dohvati najbližeg susjeda
        SensorInfo neighbor = restClient.getClosestNeighbor(sensorId);
        System.out.println("Closest neighbor: " + (neighbor != null ? neighbor.getId() : "none"));

        // schedule reading loop
        ScheduledExecutorService ex = Executors.newSingleThreadScheduledExecutor();
        SensorTask task = new SensorTask(sensorId, latitude, longitude, grpcService, restClient, neighbor);
        ex.scheduleAtFixedRate(task, 1, 5, TimeUnit.SECONDS);

        // graceful shutdown hook
        Runtime.getRuntime().addShutdownHook(new Thread(() -> {
            System.out.println("Shutdown requested -> stopping tasks and gRPC server...");
            ex.shutdownNow();
            grpcServer.shutdown();
            try {
                grpcServer.awaitTermination(3, TimeUnit.SECONDS);
            } catch (InterruptedException ignored) {}
            System.out.println("Shutdown complete.");
        }));

        grpcServer.awaitTermination();
    }
}