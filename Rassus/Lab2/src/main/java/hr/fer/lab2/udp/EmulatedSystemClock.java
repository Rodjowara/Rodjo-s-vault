package hr.fer.lab2.udp;

public class EmulatedSystemClock {
    private long offset = 0;
    public EmulatedSystemClock() {}
    public long currentTimeMillis() { return System.currentTimeMillis() + offset; }
    public void adjust(long delta) { offset += delta; }
}