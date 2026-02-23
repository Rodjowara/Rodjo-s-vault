package hr.fer.lab3.temperaturemicroservice;

import jakarta.annotation.PostConstruct;
import org.springframework.stereotype.Component;

import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;

@Component
public class CsvLoader {

    private final List<Double> temperatures = new ArrayList<>();

    @PostConstruct
    public void load() throws Exception {
        Path path = Paths.get("/config/readings.csv");
        List<String> lines = Files.readAllLines(path);

        for (String line : lines) {
            String[] parts = line.split(",");
            temperatures.add(Double.parseDouble(parts[0]));
        }
    }

    public double getCurrentTemperature() {
        long minutes = System.currentTimeMillis() / 60000;
        int index = (int) ((minutes % 100));
        return temperatures.get(index);
    }
}