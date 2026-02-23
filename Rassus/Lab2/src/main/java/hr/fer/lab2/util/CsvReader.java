package hr.fer.lab2.util;

import org.apache.commons.csv.CSVFormat;
import org.apache.commons.csv.CSVParser;
import org.apache.commons.csv.CSVRecord;

import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;

/**
 * Reads CSV from resources and returns numeric column values. Attempts to find NO2 column; otherwise uses first numeric.
 */
public class CsvReader {
    private final List<Double> no2 = new ArrayList<>();

    public CsvReader(String resourcePath) throws Exception {
        InputStream in = getClass().getResourceAsStream(resourcePath);
        if (in == null) throw new IllegalArgumentException("CSV not found: " + resourcePath);
        CSVParser p = CSVFormat.DEFAULT.withFirstRecordAsHeader().parse(new InputStreamReader(in));
        String no2Col = null;
        for (String h : p.getHeaderMap().keySet()) {
            if (h.toLowerCase().contains("no2")) { no2Col = h; break; }
        }
        for (CSVRecord r : p) {
            if (no2Col != null) {
                try { no2.add(Double.parseDouble(r.get(no2Col))); } catch (Exception ex) { no2.add(0.0); }
            } else {
                // pick first numeric column
                boolean found=false;
                for (String h : p.getHeaderMap().keySet()) {
                    try { no2.add(Double.parseDouble(r.get(h))); found=true; break; } catch (Exception ignored) {}
                }
                if (!found) no2.add(0.0);
            }
        }
    }

    /** CSV rows are 1..N ; we expect formula row = (activeSeconds % 100) + 1 -> convert to 0-based index */
    public double getReadingForRow(int rowIndex1Based) {
        int idx = Math.max(0, Math.min(no2.size()-1, rowIndex1Based-1));
        return no2.get(idx);
    }
}