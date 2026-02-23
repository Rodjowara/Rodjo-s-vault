package hr.fer.tel.rassus.server.beans;

public class Reading {

    private int sensorId;
    private double temperature;
    private double pressure;
    private double humidity;
    private double co;
    private double no2;
    private double so2;
    private double latitude;
    private double longitude;

    public Reading() {
    }

    public Reading(int sensorId, double temperature, double pressure, double humidity,
                   double co, double no2, double so2,
                   double latitude, double longitude) {
        this.sensorId = sensorId;
        this.temperature = temperature;
        this.pressure = pressure;
        this.humidity = humidity;
        this.co = co;
        this.no2 = no2;
        this.so2 = so2;
        this.latitude = latitude;
        this.longitude = longitude;
    }

    // Getteri i setteri
    public int getSensorId() {return sensorId;}
    public void setSensorId(int sensorId) {this.sensorId = sensorId;}

    public double getTemperature() { return temperature; }
    public void setTemperature(double temperature) { this.temperature = temperature; }

    public double getPressure() { return pressure; }
    public void setPressure(double pressure) { this.pressure = pressure; }

    public double getHumidity() { return humidity; }
    public void setHumidity(double humidity) { this.humidity = humidity; }

    public double getCo() { return co; }
    public void setCo(double co) { this.co = co; }

    public double getNo2() { return no2; }
    public void setNo2(double no2) { this.no2 = no2; }

    public double getSo2() { return so2; }
    public void setSo2(double so2) { this.so2 = so2; }

    public double getLatitude() { return latitude; }
    public void setLatitude(double latitude) { this.latitude = latitude; }

    public double getLongitude() { return longitude; }
    public void setLongitude(double longitude) { this.longitude = longitude; }

    @Override
    public String toString() {
        return String.format(
                "Reading{temperature=%.2f, pressure=%.2f, humidity=%.2f, co=%.2f, no2=%.2f, so2=%.2f, latitude=%.5f, longitude=%.5f}",
                temperature, pressure, humidity, co, no2, so2, latitude, longitude
        );
    }
}