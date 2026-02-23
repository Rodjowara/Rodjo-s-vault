package hr.fer.lab3.humiditymicroservice;

import jakarta.annotation.PostConstruct;
import org.springframework.stereotype.Component;

import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;

@Component
public class CsvLoader {

    private final List<Double> humidities = new ArrayList<>();

    @PostConstruct
    public void load() throws Exception {
        Path path = Paths.get("/config/readings.csv");
        List<String> lines = Files.readAllLines(path);

        for (String line : lines) {
            String[] parts = line.split(",");
            humidities.add(Double.parseDouble(parts[2]));
        }
    }

    public double getCurrentHumidity() {
        long minutes = System.currentTimeMillis() / 60000;
        int index = (int) ((minutes % 100));
        return humidities.get(index);
    }
}