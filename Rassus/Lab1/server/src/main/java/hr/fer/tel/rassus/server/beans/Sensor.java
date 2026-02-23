package hr.fer.tel.rassus.server.beans;

public class Sensor {
    private int id;
    private double latitude;
    private double longitude;
    private String ip;
    private int port;

    public Sensor() {}

    public Sensor(int id, double latitude, double longitude, String ip, int port) {
        this.id = id;
        this.latitude = latitude;
        this.longitude = longitude;
        this.ip = ip;
        this.port = port;
    }

    public int getId() { return id; }
    public void setId(int id) { this.id = id; }

    public double getLatitude() { return latitude; }
    public void setLatitude(double latitude) { this.latitude = latitude; }

    public double getLongitude() { return longitude; }
    public void setLongitude(double longitude) { this.longitude = longitude; }

    public String getIp() { return ip; }
    public void setIp(String ip) { this.ip = ip; }

    public int getPort() { return port; }
    public void setPort(int port) { this.port = port; }
}