package hr.fer.tel.rassus.server.services.impl;

import hr.fer.tel.rassus.server.beans.Sensor;
import hr.fer.tel.rassus.server.services.SensorRepository;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicInteger;

public class InMemorySensorRepository implements SensorRepository {

    private final ConcurrentHashMap<Integer, Sensor> sensors = new ConcurrentHashMap<>();
    private final AtomicInteger idGenerator = new AtomicInteger(1);

    @Override
    public int save(Sensor sensor) {
        int id = idGenerator.getAndIncrement();
        sensor.setId(id);
        sensors.put(id, sensor);
        return id;
    }

    @Override
    public Sensor findById(int sensorId) {
        return sensors.get(sensorId);
    }

    @Override
    public List<Sensor> findAll() {
        return new ArrayList<>(sensors.values());
    }

    @Override
    public Sensor findClosestNeighbor(int sensorId) {
        Sensor current = sensors.get(sensorId);
        if (current == null) return null;

        Sensor closest = null;
        double minDistance = Double.MAX_VALUE;

        for (Sensor s : sensors.values()) {
            if (s.getId() == sensorId) continue;
            double dist = haversine(current.getLatitude(), current.getLongitude(), s.getLatitude(), s.getLongitude());
            if (dist < minDistance) {
                minDistance = dist;
                closest = s;
            }
        }

        return closest;
    }

    private double haversine(double lat1, double lon1, double lat2, double lon2) {
        final double R = 6371e3; // radius in meters
        double phi1 = Math.toRadians(lat1);
        double phi2 = Math.toRadians(lat2);
        double dPhi = Math.toRadians(lat2 - lat1);
        double dLambda = Math.toRadians(lon2 - lon1);

        double a = Math.sin(dPhi/2) * Math.sin(dPhi/2) +
                Math.cos(phi1) * Math.cos(phi2) *
                        Math.sin(dLambda/2) * Math.sin(dLambda/2);
        double c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1-a));
        return R * c;
    }
}