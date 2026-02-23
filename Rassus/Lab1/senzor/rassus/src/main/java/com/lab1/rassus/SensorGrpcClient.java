package com.lab1.rassus;

import com.lab1.rassus.model.Reading;
import com.lab1.rassus.proto.ReadingRequest;
import com.lab1.rassus.proto.ReadingResponse;
import com.lab1.rassus.proto.SensorServiceGrpc;

import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;
import io.grpc.StatusRuntimeException;

import java.util.concurrent.TimeUnit;

public class SensorGrpcClient {

    private final ManagedChannel channel;
    private final SensorServiceGrpc.SensorServiceBlockingStub blockingStub;

    // Konstruktor prima host i port susjeda
    public SensorGrpcClient(String host, int port) {
        this.channel = ManagedChannelBuilder.forAddress(host, port)
                .usePlaintext() // bez TLS-a
                .build();
        this.blockingStub = SensorServiceGrpc.newBlockingStub(channel);
    }

    // Dohvati očitanje susjeda
    public Reading getNeighborReading() {
        ReadingRequest request = ReadingRequest.newBuilder().build();

        try {
            ReadingResponse response = blockingStub.getReading(request);
            return new Reading(
                    response.getTemperature(),
                    response.getPressure(),
                    response.getHumidity(),
                    response.getCo(),
                    response.getNo2(),
                    response.getSo2()
            );
        } catch (StatusRuntimeException e) {
            System.err.println("Neuspješno dohvaćanje očitanja susjeda.");
            return null;
        }
    }

    // Zatvori gRPC kanal
    public void shutdown() {
        try {
            channel.shutdown().awaitTermination(3, TimeUnit.SECONDS);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}