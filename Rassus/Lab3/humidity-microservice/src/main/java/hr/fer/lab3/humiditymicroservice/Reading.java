package hr.fer.lab3.humiditymicroservice;

public class Reading {
    private String name;
    private String unit;
    private double value;

    public Reading() {}

    public Reading(String name, String unit, double value) {
        this.name = name;
        this.unit = unit;
        this.value = value;
    }

    public String getName() { return name; }
    public String getUnit() { return unit; }
    public double getValue() { return value; }

    public void setName(String name) { this.name = name; }
    public void setUnit(String unit) { this.unit = unit; }
    public void setValue(double value) { this.value = value; }
}