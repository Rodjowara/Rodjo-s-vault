package com.lab1.rassus;

import com.lab1.rassus.model.Reading;
import com.lab1.rassus.proto.SensorServiceGrpc;
import com.lab1.rassus.proto.ReadingRequest;
import com.lab1.rassus.proto.ReadingResponse;

import io.grpc.stub.StreamObserver;

public class SensorGrpcServer extends SensorServiceGrpc.SensorServiceImplBase {

    // Ovdje se sprema trenutno očitanje senzora
    private volatile Reading currentReading;

    public SensorGrpcServer() {
        this.currentReading = new Reading(0, 0, 0, 0, 0, 0);
    }

    public void updateCurrentReading(Reading reading) {
        this.currentReading = reading;
    }

    @Override
    public void getReading(ReadingRequest request,
                           StreamObserver<ReadingResponse> responseObserver) {

        Reading r = currentReading;
        if (r == null) {
            responseObserver.onError(new IllegalStateException("Current reading not available"));
            return;
        }

        ReadingResponse response = ReadingResponse.newBuilder()
                .setTemperature(r.getTemperature())
                .setPressure(r.getPressure())
                .setHumidity(r.getHumidity())
                .setCo(r.getCo())
                .setNo2(r.getNo2())
                .setSo2(r.getSo2())
                .build();

        responseObserver.onNext(response);
        responseObserver.onCompleted();
    }
}