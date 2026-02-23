package com.lab1.rassus.model;

public class Reading {
    private double temperature;
    private double pressure;
    private double humidity;
    private double co;
    private double no2;
    private double so2;

    public Reading(double temperature, double pressure, double humidity,
                         double co, double no2, double so2) {
        this.temperature = temperature;
        this.pressure = pressure;
        this.humidity = humidity;
        this.co = co;
        this.no2 = no2;
        this.so2 = so2;
    }

    public double getTemperature() { return temperature; }
    public double getPressure() { return pressure; }
    public double getHumidity() { return humidity; }
    public double getCo() { return co; }
    public double getNo2() { return no2; }
    public double getSo2() { return so2; }

    /** Izračunava prosjek između dva očitanja (this i drugo). */
    public static Reading average(Reading a, Reading b) {
        return new Reading(
                (a.temperature + b.temperature) / 2.0,
                (a.pressure + b.pressure) / 2.0,
                (a.humidity + b.humidity) / 2.0,
                (a.co + b.co) / 2.0,
                (a.no2 + b.no2) / 2.0,
                (a.so2 + b.so2) / 2.0
        );
    }
}