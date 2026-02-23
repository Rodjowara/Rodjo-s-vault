package hr.fer.tel.rassus.server.controllers;

import hr.fer.tel.rassus.server.beans.Reading;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.*;

@RestController
@RequestMapping("/sensors")
public class ReadingController {

    // Mapiranje sensorId -> lista očitanja
    private final Map<Integer, List<Reading>> sensorReadings = new HashMap<>();

    @PostMapping("/{sensorId}/readings")
    public ResponseEntity<Void> addReading(@PathVariable int sensorId, @RequestBody Reading reading) {
        reading.setSensorId(sensorId);
        sensorReadings.computeIfAbsent(sensorId, k -> new ArrayList<>()).add(reading);
        return ResponseEntity.status(HttpStatus.CREATED).build();
    }

    @GetMapping("/{sensorId}/readings")
    public ResponseEntity<List<Reading>> getReadings(@PathVariable int sensorId) {
        List<Reading> readings = sensorReadings.get(sensorId);
        if (readings == null) {
            return ResponseEntity.noContent().build();
        }
        return ResponseEntity.ok(readings);
    }

    @GetMapping("/all-readings")
    public ResponseEntity<List<Reading>> getAllReadings() {
        List<Reading> allReadings = new ArrayList<>();
        sensorReadings.values().forEach(allReadings::addAll);
        return ResponseEntity.ok(allReadings);
    }
}