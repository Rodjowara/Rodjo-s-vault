package hr.fer.tel.rassus.server.services.impl;

import hr.fer.tel.rassus.server.beans.Reading;
import hr.fer.tel.rassus.server.services.ReadingRepository;

import java.util.*;
import java.util.concurrent.ConcurrentHashMap;

public class InMemoryReadingRepository implements ReadingRepository {

    private final Map<Integer, List<Reading>> readingsBySensor = new ConcurrentHashMap<>();

    @Override
    public void save(Reading reading) {
        readingsBySensor.computeIfAbsent(reading.getSensorId(), k -> new ArrayList<>()).add(reading);
    }

    @Override
    public List<Reading> findBySensorId(int sensorId) {
        return readingsBySensor.getOrDefault(sensorId, Collections.emptyList());
    }

    @Override
    public Reading findLatestBySensorId(int sensorId) {
        List<Reading> list = readingsBySensor.get(sensorId);
        if (list == null || list.isEmpty()) return null;
        return list.get(list.size() - 1);
    }

    @Override
    public List<Reading> findAll() {
        List<Reading> all = new ArrayList<>();
        readingsBySensor.values().forEach(all::addAll);
        return all;
    }
}