package hr.fer.lab3.temperaturemicroservice;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController @RequestMapping("/reading")
public class TemperatureController {
    private final CsvLoader loader;
    public TemperatureController(CsvLoader loader) {
        this.loader = loader;
    }

    @GetMapping
    public Reading getTemperature() {
        return new Reading( "Temperature", "C", loader.getCurrentTemperature() );
    }
}