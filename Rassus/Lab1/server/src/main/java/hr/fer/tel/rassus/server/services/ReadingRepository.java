package hr.fer.tel.rassus.server.services;

import hr.fer.tel.rassus.server.beans.Reading;

import java.util.List;

public interface ReadingRepository {

    void save(Reading reading);
    List<Reading> findBySensorId(int sensorId);
    Reading findLatestBySensorId(int sensorId);
    List<Reading> findAll();
}
