package hr.fer.tel.rassus.server.services;

import hr.fer.tel.rassus.server.beans.Sensor;
import java.util.List;

public interface SensorRepository {

    int save(Sensor sensor);
    Sensor findById(int sensorId);
    List<Sensor> findAll();
    Sensor findClosestNeighbor(int sensorId);
}
