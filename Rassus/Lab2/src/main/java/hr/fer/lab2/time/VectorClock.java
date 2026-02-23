package hr.fer.lab2.time;

import org.json.JSONObject;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;

/**
 * Thread-safe vector clock implemented as a map from nodeId -> counter.
 * Provides merge, tick, snapshot, json serialize/deserialize, and helpers for comparison.
 */
public class VectorClock {

    private final Map<String, Integer> v = new HashMap<>();

    public VectorClock() {}

    /** Increment local entry for given node id (tick). */
    public synchronized void tick(String nodeId) {
        v.put(nodeId, v.getOrDefault(nodeId, 0) + 1);
    }

    /** Merge another vector (take element-wise max). */
    public synchronized void merge(Map<String, Integer> other) {
        if (other == null) return;
        for (Map.Entry<String, Integer> e : other.entrySet()) {
            String k = e.getKey();
            Integer val = e.getValue();
            if (val == null) continue;
            v.put(k, Math.max(v.getOrDefault(k, 0), val));
        }
    }

    /** Snapshot a copy of vector (safe copy). */
    public synchronized Map<String, Integer> snapshot() {
        return new HashMap<>(v);
    }

    /** Convert to JSONObject for embedding into messages. */
    public synchronized JSONObject toJson() {
        JSONObject jo = new JSONObject();
        for (Map.Entry<String, Integer> e : v.entrySet()) jo.put(e.getKey(), e.getValue());
        return jo;
    }

    /** Static: parse JSONObject to Map<String,Integer> (used for incoming messages). */
    public static Map<String, Integer> fromJson(org.json.JSONObject jo) {
        if (jo == null) return Collections.emptyMap();
        Map<String, Integer> m = new HashMap<>();
        Set<String> keys = jo.keySet();
        for (String k : keys) {
            try {
                int val = jo.getInt(k);
                m.put(k, val);
            } catch (Exception ignored) {}
        }
        return m;
    }

    /**
     * Vector clock partial order helpers:
     * isBefore(a,b) returns true iff a <= b and exists i: a[i] < b[i]
     * They operate on maps (snapshots).
     */
    public static boolean isLessOrEqual(Map<String, Integer> a, Map<String, Integer> b) {
        // a[i] <= b[i] for all i (missing = 0)
        for (String k : unionKeys(a, b)) {
            int av = a.getOrDefault(k, 0);
            int bv = b.getOrDefault(k, 0);
            if (av > bv) return false;
        }
        return true;
    }

    public static boolean isStrictlyBefore(Map<String, Integer> a, Map<String, Integer> b) {
        if (!isLessOrEqual(a, b)) return false;
        // exists i: a[i] < b[i]
        for (String k : unionKeys(a, b)) {
            int av = a.getOrDefault(k, 0);
            int bv = b.getOrDefault(k, 0);
            if (av < bv) return true;
        }
        return false;
    }

    private static Set<String> unionKeys(Map<String,Integer> a, Map<String,Integer> b) {
        var s = new java.util.HashSet<String>();
        if (a != null) s.addAll(a.keySet());
        if (b != null) s.addAll(b.keySet());
        return s;
    }
}