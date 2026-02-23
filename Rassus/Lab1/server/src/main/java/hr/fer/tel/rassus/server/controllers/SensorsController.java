package hr.fer.tel.rassus.server.controllers;

import hr.fer.tel.rassus.server.beans.Sensor;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.net.URI;
import java.util.ArrayList;
import java.util.List;

@RestController
@RequestMapping("/sensors")
public class SensorsController {

    private final List<Sensor> sensors = new ArrayList<>();
    private int nextId = 1;

    @PostMapping
    public ResponseEntity<Sensor> registerSensor(@RequestBody Sensor sensor) {
        sensor.setId(nextId++);
        sensors.add(sensor);

        // Kreira URL novog resursa
        String url = "/sensors/" + sensor.getId();

        // Vrati 201 Created i Location header
        return ResponseEntity
                .created(URI.create(url)) // postavlja HTTP 201 i Location header
                .body(sensor);           // opcionalno: vraća JSON tijelo
    }

    @GetMapping
    public List<Sensor> getAllSensors() {
        return sensors;
    }

    @GetMapping("/{id}/neighbor")
    public Sensor getClosestSensor(@PathVariable int id) {
        Sensor me = sensors.stream()
                .filter(s -> s.getId() == id)
                .findFirst()
                .orElse(null);

        if (me == null) return null;

        Sensor closest = null;
        double minDistance = Double.MAX_VALUE;

        for (Sensor s : sensors) {
            if (s.getId() == me.getId()) continue;
            double dist = haversine(me.getLatitude(), me.getLongitude(), s.getLatitude(), s.getLongitude());
            if (dist < minDistance) {
                minDistance = dist;
                closest = s;
            }
        }

        return closest;
    }

    // Haversine formula za udaljenost
    private double haversine(double lat1, double lon1, double lat2, double lon2) {
        final double R = 6371e3; // metara
        double phi1 = Math.toRadians(lat1);
        double phi2 = Math.toRadians(lat2);
        double dLat = Math.toRadians(lat2 - lat1);
        double dLon = Math.toRadians(lon2 - lon1);

        double a = Math.sin(dLat/2) * Math.sin(dLat/2) +
                Math.cos(phi1) * Math.cos(phi2) *
                        Math.sin(dLon/2) * Math.sin(dLon/2);
        double c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1-a));
        return R * c;
    }
}