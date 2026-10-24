import csv
import tempfile
import unittest
from pathlib import Path

from examples.blrv_flight_demo import run_demo


class BlueRavenDemoTests(unittest.TestCase):
    def test_high_rate_steps_and_once_per_low_rate_baro_update(self):
        with tempfile.TemporaryDirectory(dir=Path(__file__).resolve().parents[1] / "build") as folder:
            directory = Path(folder)
            high = directory / "hr.csv"
            low = directory / "lr.csv"
            high.write_text("Flight_Time_(s),Accel_X,Accel_Y,Accel_Z,Gyro_X,Gyro_Y,Gyro_Z,Quat_1,Quat_2,Quat_3,Quat_4\n" +
                            "".join(f"{i*.002:.3f},-1,0,0,0,0,0,1,0,0,0\n" for i in range(-1, 12)))
            low.write_text("Flight_Time_(s),Baro_Press_(atm),Baro_Altitude_AGL_(feet),Inertial_Altitude,Inertial_DR_Position,Inertial_CR_position,Velocity_Up,Tilt_Angle_(deg),Liftoff,Apogee\n"
                           "0,1,0,0,0,0,0,0,0,0\n"
                           "0.02,1,0,0,0,0,0,0,0,0\n")
            output = directory / "report"
            summary = run_demo(high, low, output)
            self.assertEqual(summary["high_rate_steps_processed"], 13)
            self.assertEqual(summary["matched_comparison_rows"], 2)
            self.assertEqual(summary["high_rate_samples_after_low_rate_end"], 1)
            self.assertTrue(summary["full_high_rate_state_and_covariance_finite"])
            self.assertFalse(summary["comparison_is_ground_truth"])
            with (output / "input.csv").open(newline="") as file:
                rows = list(csv.DictReader(file))
            self.assertEqual([i for i, row in enumerate(rows) if row["baro_valid"] == "1"],
                             [1, 11])
            self.assertLess(summary["overall_difference"]["tilt_deg_max_abs_difference"], .05)


if __name__ == "__main__":
    unittest.main()
