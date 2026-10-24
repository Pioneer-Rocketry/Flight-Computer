import dataclasses
import math
import unittest

from navsitl import Runner, Sample, SensorConfig, generate, run_rows
from navsitl.metrics import covariance_health, orientation_error_deg, summarize


class NavigationTests(unittest.TestCase):
    def test_stationary_analytical_truth_and_covariance(self):
        pairs = run_rows(generate("stationary", 3.0, 2000))
        metrics = summarize(pairs)
        self.assertLess(metrics["position_max_error"], 1e-5)
        self.assertLess(metrics["velocity_max_error"], 1e-5)
        self.assertLess(metrics["acceleration_max_error"], 1e-5)
        self.assertLess(metrics["max_quaternion_norm_error"], 1e-6)
        self.assertTrue(metrics["all_finite"])
        self.assertTrue(metrics["covariance_psd"])
        self.assertLess(metrics["max_covariance_symmetry_error"], 1e-5)

    def test_constant_acceleration_with_pad_alignment(self):
        pairs = run_rows(generate("constant_acceleration", 3.0, 2000))
        metrics = summarize(pairs)
        self.assertLess(metrics["position_max_error"], 0.005)
        self.assertLess(metrics["velocity_max_error"], 0.005)
        self.assertLess(metrics["orientation_max_error_deg"], .01)

    def test_single_and_multi_axis_rotations(self):
        for scenario in ("rotation_x", "rotation_xyz"):
            with self.subTest(scenario=scenario):
                pairs = run_rows(generate(scenario, 3.0, 2000))
                metrics = summarize(pairs)
                self.assertLess(metrics["orientation_max_error_deg"], .02)
                self.assertLess(metrics["acceleration_max_error"], .01)
                self.assertTrue(metrics["covariance_psd"])
        self.assertAlmostEqual(pairs[-1][1]["dt_applied_s"], .002, places=6)

    def test_full_flight_phases_and_ground_truth(self):
        pairs = run_rows(generate("full_flight", 30.0, 5000))
        self.assertTrue({"pad", "boost", "coast", "drogue", "main_deceleration"}
                        <= {row.phase for row, _ in pairs})
        metrics = summarize(pairs)
        self.assertLess(metrics["position_max_error"], .2)
        self.assertLess(metrics["coast_drift_m"], .2)
        self.assertTrue(metrics["all_finite"])
        coast = [row for row, _ in pairs if row.phase == "coast"]
        self.assertTrue(coast)
        self.assertTrue(all(abs(row.sample.low_g[2]) < 1e-5 for row in coast))
        apogee = next(row for row, _ in pairs if row.phase == "drogue")
        self.assertLess(abs(apogee.truth.velocity_mps[2]), .02)

    def test_full_flight_lands_and_filter_recovers(self):
        pairs = run_rows(generate("full_flight", 160.0, 20000))
        self.assertEqual(pairs[-1][0].phase, "landing")
        self.assertTrue({"main_descent", "landing"} <= {row.phase for row, _ in pairs})
        self.assertLess(abs(pairs[-1][1]["position_m"][2]), .5)
        self.assertLess(abs(pairs[-1][1]["velocity_mps"][2]), .5)
        self.assertTrue(summarize(pairs)["covariance_psd"])

    def test_low_g_saturation_preserves_high_g_update(self):
        pairs = run_rows(generate("saturation_transition", 3.5, 2000))
        saturated = [(row, result) for row, result in pairs
                     if row.phase == "low_g_saturated"]
        self.assertTrue(saturated)
        self.assertTrue(all(result["low_g_variance_scale"] == 1e6
                            for _, result in saturated))
        self.assertTrue(all(result["measurements"][5]["applied"]
                            for _, result in saturated))
        self.assertLess(summarize(pairs)["max_low_g_transition_accel_jump_mps2"], 120)

    def test_stall_and_variable_sampling(self):
        stalled = run_rows(generate("scheduler_stall", 2.0, 2000))
        metrics = summarize(stalled)
        self.assertEqual(metrics["clamped_steps"], 1)
        self.assertTrue(any(abs(est["dt_raw_s"]-.1) < 1e-5 and
                            abs(est["dt_applied_s"]-.05) < 1e-5
                            for _, est in stalled))
        variable = run_rows(generate("variable_dt", 1.0, 2000))
        self.assertGreater(len({round(est["dt_raw_s"], 6) for _, est in variable}), 1)

    def test_outage_invalid_and_nonfinite_inputs(self):
        outage = run_rows(generate("dropout", 5.0, 2000))
        missing = [est for row, est in outage if 2.5 <= row.timestamp_us*1e-6 < 3.5]
        self.assertTrue(missing)
        self.assertTrue(all(not any(m["applied"] for m in est["measurements"])
                            for est in missing))
        self.assertTrue(summarize(outage)["all_finite"])
        self.assertIsNotNone(summarize(outage)["outage_recovery_s"])
        invalid = run_rows(generate("invalid", 2.0, 2000))
        self.assertTrue(summarize(invalid)["all_finite"])
        self.assertTrue(any(not est["gyro_used"] for _, est in invalid))

    def test_gps_is_not_fused_and_outlier_is_visible(self):
        rows = generate("stationary", 1.0, 2000)
        changed = [dataclasses.replace(row, sample=dataclasses.replace(
            row.sample, gps_altitude_m=10000.0, gps_latitude_deg=50.0,
            gps_longitude_deg=-50.0, gps_fix=3)) for row in rows]
        reference = run_rows(rows)
        altered = run_rows(changed)
        self.assertEqual([x[1]["position_m"] for x in reference],
                         [x[1]["position_m"] for x in altered])
        outlier = run_rows(generate("baro_outlier", 2.0, 2000))
        self.assertTrue(any(abs(est["measurements"][6]["innovation"]) > 90
                            for _, est in outlier))
        self.assertGreater(summarize(outlier)["position_max_error"], .1)

    def test_noise_bias_and_long_covariance_stability(self):
        self.assertEqual([r.sample for r in generate("noise_bias", 1, 2000, seed=7)],
                         [r.sample for r in generate("noise_bias", 1, 2000, seed=7)])
        self.assertNotEqual([r.sample for r in generate("noise_bias", 1, 2000, seed=7)],
                            [r.sample for r in generate("noise_bias", 1, 2000, seed=8)])
        pairs = run_rows(generate("long_duration", 120, 10000))
        metrics = summarize(pairs)
        self.assertTrue(metrics["covariance_psd"])
        self.assertTrue(metrics["all_finite"])

    def test_timestamp_api_and_first_filter_interval(self):
        sample = Sample()
        with Runner(sample, 1000) as runner:
            result = runner.step(sample, 11000)
            self.assertAlmostEqual(result["dt_raw_s"], .01, places=7)
            self.assertAlmostEqual(result["dt_filter_s"], .002, places=7)
            duplicate = runner.step(sample, 11000)
            self.assertFalse(duplicate["stepped"])
            with self.assertRaises(ValueError):
                runner.step(sample, 10000)
            with self.assertRaises(ValueError):
                runner.step(sample, 11000 + 2**32)

    def test_covariance_health_detects_bad_matrix(self):
        good = ((1.0, 0.0, 0.0), (0.0, 1.0, 0.0), (0.0, 0.0, 1.0))
        bad = ((1.0, 0.0, 0.0), (0.0, -1.0, 0.0), (0.0, 0.0, 1.0))
        self.assertTrue(covariance_health((good, good, good))[1])
        self.assertFalse(covariance_health((good, bad, good))[1])


if __name__ == "__main__":
    unittest.main()
