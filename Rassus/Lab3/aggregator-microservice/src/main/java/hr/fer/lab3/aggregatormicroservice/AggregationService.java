package hr.fer.lab3.aggregatormicroservice;

import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;
import org.springframework.web.reactive.function.client.WebClient;

import java.util.List;

@Service
public class AggregationService {

    @Value("${temperature.url}")
    private String temperatureUrl;

    @Value("${humidity.url}")
    private String humidityUrl;

    @Value("${temperature-unit}")
    private String unit;

    private final WebClient webClient;

    public AggregationService(WebClient webClient) {
        this.webClient = webClient;
    }

    public List<Reading> aggregate() {

        Reading temperature = webClient.get()
                .uri(temperatureUrl)
                .retrieve()
                .bodyToMono(Reading.class)
                .block();

        Reading humidity = webClient.get()
                .uri(humidityUrl)
                .retrieve()
                .bodyToMono(Reading.class)
                .block();

        if ("K".equalsIgnoreCase(unit)) {
            temperature.setValue(temperature.getValue() + 273.15);
            temperature.setUnit("K");
        }

        return List.of(humidity, temperature);
    }
}