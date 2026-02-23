package hr.fer.lab3.humiditymicroservice;

import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController
@RequestMapping("/reading")
public class HumidityController {

    private final CsvLoader loader;

    public HumidityController(CsvLoader loader) {
        this.loader = loader;
    }

    @GetMapping
    public Reading getHumidity() {
        return new Reading(
                "Humidity",
                "%",
                loader.getCurrentHumidity()
        );
    }
}