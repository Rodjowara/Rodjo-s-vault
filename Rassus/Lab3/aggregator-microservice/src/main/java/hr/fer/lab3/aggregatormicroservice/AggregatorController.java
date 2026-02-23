package hr.fer.lab3.aggregatormicroservice;

import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;

@RestController
@RequestMapping("/aggregate")
public class AggregatorController {

    private final AggregationService service;

    public AggregatorController(AggregationService service) {
        this.service = service;
    }

    @GetMapping
    public List<Reading> aggregate() {
        return service.aggregate();
    }
}