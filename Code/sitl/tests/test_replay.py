import json
import tempfile
import unittest
from pathlib import Path

from navsitl import generate, read_rows, run_rows, write_rows
from navsitl.metrics import summarize
from navsitl.schema import DEFAULT_METADATA


class ReplayTests(unittest.TestCase):
    def test_synthetic_roundtrip_matches_native_estimates(self):
        rows = generate("rotation_xyz", 1, 5000)
        with tempfile.TemporaryDirectory(dir=Path(__file__).resolve().parents[1] / "build") as folder:
            path = Path(folder) / "fixture.csv"
            write_rows(path, rows)
            recovered, metadata = read_rows(path)
            self.assertEqual(metadata["schema_version"], 1)
            self.assertEqual(rows, recovered)
            a = run_rows(rows)
            b = run_rows(recovered)
            self.assertEqual([state["quaternion"] for _, state in a],
                             [state["quaternion"] for _, state in b])

    def test_pressure_only_source_mapping_and_units(self):
        with tempfile.TemporaryDirectory(dir=Path(__file__).resolve().parents[1] / "build") as folder:
            path = Path(folder) / "flight.csv"
            path.write_text("time_ms,ax,ay,az,p_pa\n0,0,0,9.81,101325\n2,0,0,9.81,101300\n")
            metadata = dict(DEFAULT_METADATA)
            metadata["column_map"] = {
                "timestamp_us": {"column": "time_ms", "scale": 1000},
                "low_g_x_mps2": "ax", "low_g_y_mps2": "ay",
                "low_g_z_mps2": "az",
                "baro_pressure_hpa": {"column": "p_pa", "scale": .01},
            }
            path.with_suffix(".json").write_text(json.dumps(metadata))
            rows, _ = read_rows(path)
            self.assertEqual([r.timestamp_us for r in rows], [0, 2000])
            self.assertTrue(rows[1].sample.baro_altitude_m > 0)
            self.assertFalse(rows[1].sample.high_g_valid)
            self.assertTrue(summarize(run_rows(rows))["all_finite"])

    def test_reject_timestamp_regression(self):
        with tempfile.TemporaryDirectory(dir=Path(__file__).resolve().parents[1] / "build") as folder:
            path = Path(folder) / "flight.csv"
            write_rows(path, generate("stationary", .01, 2000))
            lines = path.read_text().splitlines()
            fields = lines[2].split(",")
            fields[0] = "0"
            lines[2] = ",".join(fields)
            path.write_text("\n".join(lines) + "\n")
            with self.assertRaisesRegex(ValueError, "timestamps"):
                read_rows(path)


if __name__ == "__main__":
    unittest.main()
