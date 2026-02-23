package com.lab1.rassus;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.lab1.rassus.model.Reading;
import com.lab1.rassus.model.SensorInfo;

import java.net.URI;
import java.net.http.HttpClient;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse;
import java.time.Duration;

public class SensorRestClient {

    private final HttpClient httpClient;
    private final String baseUrl;

    public SensorRestClient(String baseUrl) {
        this.baseUrl = baseUrl.endsWith("/") ? baseUrl.substring(0, baseUrl.length() - 1) : baseUrl;
        this.httpClient = HttpClient.newHttpClient();
    }

    public Integer registerSensor(double latitude, double longitude, int grpcPort) {
        try {
            String json = String.format(
                    "{\"latitude\":%f,\"longitude\":%f,\"ip\":\"127.0.0.1\",\"port\":%d}",
                    latitude, longitude, grpcPort
            );

            HttpRequest request = HttpRequest.newBuilder()
                    .uri(URI.create(baseUrl + "/sensors"))
                    .header("Content-Type", "application/json")
                    .timeout(Duration.ofSeconds(5))
                    .POST(HttpRequest.BodyPublishers.ofString(json))
                    .build();

            HttpResponse<String> response = httpClient.send(request, HttpResponse.BodyHandlers.ofString());
            if (response.statusCode() != 201) {
                System.err.println("Failed to register sensor: " + response.statusCode() + " -> " + response.body());
                return null;
            }

            // Dohvati sensor ID iz Location headera
            String location = response.headers().firstValue("location").orElse(null);
            if (location == null) return null;

            String[] parts = location.split("/");
            return Integer.parseInt(parts[parts.length - 1]);

        } catch (Exception e) {
            System.err.println("Exception during sensor registration: " + e.getMessage());
            return null;
        }
    }

    public SensorInfo getClosestNeighbor(int sensorId) {
        try {
            HttpRequest request = HttpRequest.newBuilder()
                    .uri(URI.create(baseUrl + "/sensors/" + sensorId + "/neighbor"))
                    .header("Accept", "application/json")
                    .timeout(Duration.ofSeconds(5))
                    .GET()
                    .build();

            HttpResponse<String> response = httpClient.send(request, HttpResponse.BodyHandlers.ofString());
            if (response.statusCode() != 200) return null;

            ObjectMapper mapper = new ObjectMapper();
            return mapper.readValue(response.body(), SensorInfo.class);

        } catch (Exception e) {
            System.err.println("Exception while fetching closest neighbor: " + e.getMessage());
            return null;
        }
    }

    public boolean sendReading(int sensorId, Reading reading, double latitude, double longitude) {
        try {
            String json = String.format(
                    "{\"id\":%d,\"temperature\":%.2f,\"pressure\":%.2f,\"humidity\":%.2f," +
                            "\"co\":%.2f,\"no2\":%.2f,\"so2\":%.2f," +
                            "\"latitude\":%.5f,\"longitude\":%.5f}",
                    sensorId, reading.getTemperature(), reading.getPressure(), reading.getHumidity(),
                    reading.getCo(), reading.getNo2(), reading.getSo2(),
                    latitude, longitude
            );

            HttpRequest request = HttpRequest.newBuilder()
                    .uri(URI.create(baseUrl + "/sensors/" + sensorId + "/readings"))
                    .header("Content-Type", "application/json")
                    .timeout(Duration.ofSeconds(5))
                    .POST(HttpRequest.BodyPublishers.ofString(json))
                    .build();

            HttpResponse<String> response = httpClient.send(request, HttpResponse.BodyHandlers.ofString());
            return response.statusCode() == 200 || response.statusCode() == 201;

        } catch (Exception e) {
            System.err.println("Exception while sending reading: " + e.getMessage());
            return false;
        }
    }
}