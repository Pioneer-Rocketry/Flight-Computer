"""Replay synchronized Blue Raven high/low rate logs through flight C++ navigation.

The source CSVs are calibrated engineering units, despite the 'raw' high-rate
label. COTS estimates are comparison references, not independent ground truth.
"""

import argparse
import csv
import json
import math
import statistics
from decimal import Decimal
from pathlib import Path

from navsitl import Row, Runner, Sample
from navsitl.metrics import orientation_error_deg
from navsitl.report import write_report
from navsitl.schema import CSV_FIELDS, DEFAULT_METADATA, _as_row

G = 9.81
FT_TO_M = 0.3048
ATM_TO_HPA = 1013.25


def time_us(value):
    return int(Decimal(value) * 1_000_000)


def quaternion_multiply(a, b):
    w, x, y, z = a
    W, X, Y, Z = b
    return (w*W-x*X-y*Y-z*Z, w*X+x*W+y*Z-z*Y,
            w*Y-x*Z+y*W+z*X, w*Z+x*Y-y*X+z*W)


def quaternion_conjugate(q):
    return (q[0], -q[1], -q[2], -q[3])


def rotate(q, vector):
    pure = (0.0, *vector)
    return quaternion_multiply(quaternion_multiply(q, pure),
                               quaternion_conjugate(q))[1:]


def load_low_rate(path):
    fields = ("Flight_Time_(s)", "Baro_Press_(atm)",
              "Baro_Altitude_AGL_(feet)", "Inertial_Altitude",
              "Inertial_DR_Position", "Inertial_CR_position", "Velocity_Up",
              "Tilt_Angle_(deg)", "Liftoff", "Apogee")
    rows = []
    with Path(path).open(newline="") as file:
        reader = csv.DictReader(file)
        if not set(fields) <= set(reader.fieldnames or ()):
            raise ValueError(f"low-rate CSV missing {sorted(set(fields)-set(reader.fieldnames or ())) }")
        for raw in reader:
            item = {"time_us": time_us(raw["Flight_Time_(s)"])}
            for field in fields[1:]:
                item[field] = float(raw[field])
            if rows and item["time_us"] <= rows[-1]["time_us"]:
                raise ValueError("low-rate time must increase strictly")
            rows.append(item)
    if len(rows) < 2:
        raise ValueError("low-rate CSV needs at least two samples")
    return rows


def _comparison(low, hr_time_us, hr_quat, estimate, alignment):
    q_ref = quaternion_multiply(alignment, hr_quat)
    body_up = rotate(estimate["quaternion"], (-1.0, 0.0, 0.0))
    sitl_tilt = math.degrees(math.acos(max(-1.0, min(1.0, body_up[2]))))
    horizontal = math.hypot(*estimate["position_m"][:2])
    cots_horizontal = math.hypot(low["Inertial_DR_Position"],
                                 low["Inertial_CR_position"]) * FT_TO_M
    return {
        "flight_time_s": hr_time_us * 1e-6,
        "lr_time_s": low["time_us"] * 1e-6,
        "time_alignment_error_us": hr_time_us-low["time_us"],
        "sitl_altitude_m_agl": estimate["position_m"][2],
        "cots_inertial_altitude_m_assumed": low["Inertial_Altitude"]*FT_TO_M,
        "cots_baro_altitude_m_agl": low["Baro_Altitude_AGL_(feet)"]*FT_TO_M,
        "sitl_vertical_velocity_mps": estimate["velocity_mps"][2],
        "cots_velocity_up_mps_assumed": low["Velocity_Up"]*FT_TO_M,
        "sitl_horizontal_distance_m": horizontal,
        "cots_horizontal_distance_m_assumed": cots_horizontal,
        "sitl_tilt_deg": sitl_tilt,
        "cots_tilt_deg": low["Tilt_Angle_(deg)"],
        "aligned_quaternion_difference_deg": orientation_error_deg(
            estimate["quaternion"], q_ref),
        "liftoff": int(low["Liftoff"]),
        "apogee": int(low["Apogee"]),
    }


def _difference_metrics(rows):
    definitions = {
        "inertial_altitude_m": ("sitl_altitude_m_agl", "cots_inertial_altitude_m_assumed"),
        "baro_altitude_m": ("sitl_altitude_m_agl", "cots_baro_altitude_m_agl"),
        "vertical_velocity_mps": ("sitl_vertical_velocity_mps", "cots_velocity_up_mps_assumed"),
        "horizontal_distance_m": ("sitl_horizontal_distance_m", "cots_horizontal_distance_m_assumed"),
        "tilt_deg": ("sitl_tilt_deg", "cots_tilt_deg"),
    }
    out = {"samples": len(rows)}
    if not rows:
        return out
    for name, (a, b) in definitions.items():
        differences = [r[a]-r[b] for r in rows]
        out[name + "_median_abs_difference"] = statistics.median(map(abs, differences))
        out[name + "_rmse_difference"] = math.sqrt(statistics.fmean(d*d for d in differences))
        out[name + "_max_abs_difference"] = max(map(abs, differences))
    q_errors = [r["aligned_quaternion_difference_deg"] for r in rows]
    out["aligned_quaternion_median_difference_deg"] = statistics.median(q_errors)
    out["aligned_quaternion_max_difference_deg"] = max(q_errors)
    return out


def plot_comparison(output, rows):
    import matplotlib
    matplotlib.use("Agg")
    import matplotlib.pyplot as plt
    for suffix, selected in (("full", rows),
                             ("ascent", [r for r in rows if -2 <= r["flight_time_s"] <= 40]),
                             ("early_ascent", [r for r in rows if -2 <= r["flight_time_s"] <= 8])):
        if not selected:
            continue
        t = [r["flight_time_s"] for r in selected]
        fig, axes = plt.subplots(5, 1, figsize=(13, 15), sharex=True)
        traces = (
            (("SITL", "sitl_altitude_m_agl"), ("COTS inertial", "cots_inertial_altitude_m_assumed"),
             ("COTS barometer (fused)", "cots_baro_altitude_m_agl")),
            (("SITL", "sitl_vertical_velocity_mps"),
             ("COTS computed", "cots_velocity_up_mps_assumed")),
            (("SITL radial", "sitl_horizontal_distance_m"),
             ("COTS DR/CR radial", "cots_horizontal_distance_m_assumed")),
            (("SITL", "sitl_tilt_deg"), ("COTS computed", "cots_tilt_deg")),
            (("aligned quaternion difference", "aligned_quaternion_difference_deg"),),
        )
        labels = ("altitude AGL (m)", "vertical velocity (m/s)",
                  "horizontal distance (m)", "tilt (deg)",
                  "relative attitude difference (deg)")
        for ax, group, label in zip(axes, traces, labels):
            for title, key in group:
                ax.plot(t, [r[key] for r in selected], label=title, linewidth=1)
            ax.set_ylabel(label)
            ax.grid(True, alpha=.3)
            ax.legend(loc="best")
        axes[-1].set_xlabel("COTS flight time (s)")
        fig.tight_layout()
        fig.savefig(output / f"comparison_{suffix}.png", dpi=120)
        plt.close(fig)


def run_demo(high_path, low_path, output, plots=False):
    output = Path(output)
    output.mkdir(parents=True, exist_ok=True)
    low_rows = load_low_rate(low_path)
    last_lr_us = low_rows[-1]["time_us"]
    compare_rows, report_pairs = [], []
    high_count = processed = accel_above_16 = gyro_above_250 = 0
    after_lr_end = 0
    full_hr_state_finite = True
    low_index = 0
    alignment = None
    runner = None
    last_hr_us = None
    first_hr_us = None
    last_baro = None
    fields = ("Flight_Time_(s)", "Accel_X", "Accel_Y", "Accel_Z",
              "Gyro_X", "Gyro_Y", "Gyro_Z", "Quat_1", "Quat_2", "Quat_3", "Quat_4")
    hr_fields = (["flight_time_s", "timestamp_us"] +
                 [f"position_{axis}_m" for axis in "xyz"] +
                 [f"velocity_{axis}_mps" for axis in "xyz"] +
                 [f"acceleration_{axis}_mps2" for axis in "xyz"] +
                 [f"quaternion_{component}" for component in "wxyz"] +
                 ["low_g_variance_scale", "barometer_update"])
    try:
        with Path(high_path).open(newline="") as source, \
             (output / "input.csv").open("w", newline="") as normalized, \
             (output / "hr_estimates.csv").open("w", newline="") as full_output:
            reader = csv.DictReader(source)
            if not set(fields) <= set(reader.fieldnames or ()):
                raise ValueError(f"high-rate CSV missing {sorted(set(fields)-set(reader.fieldnames or ())) }")
            input_writer = csv.DictWriter(normalized, fieldnames=CSV_FIELDS)
            input_writer.writeheader()
            hr_writer = csv.DictWriter(full_output, fieldnames=hr_fields)
            hr_writer.writeheader()
            for raw in reader:
                high_count += 1
                hr_us = time_us(raw["Flight_Time_(s)"])
                if first_hr_us is None:
                    first_hr_us = hr_us
                if last_hr_us is not None and hr_us <= last_hr_us:
                    raise ValueError("high-rate time must increase strictly")
                last_hr_us = hr_us
                new_low = None
                while low_index < len(low_rows) and low_rows[low_index]["time_us"] <= hr_us:
                    new_low = low_rows[low_index]
                    last_baro = new_low
                    low_index += 1
                accel_g = tuple(float(raw["Accel_"+axis]) for axis in "XYZ")
                gyro = tuple(float(raw["Gyro_"+axis]) for axis in "XYZ")
                hr_quat = tuple(float(raw["Quat_"+str(i)]) for i in range(1,5))
                if max(map(abs, accel_g)) > 16: accel_above_16 += 1
                if max(map(abs, gyro)) > 250: gyro_above_250 += 1
                finite_accel = all(map(math.isfinite, accel_g))
                finite_gyro = all(map(math.isfinite, gyro))
                pressure = (last_baro["Baro_Press_(atm)"]*ATM_TO_HPA
                            if last_baro is not None else float("nan"))
                altitude = (last_baro["Baro_Altitude_AGL_(feet)"]*FT_TO_M
                            if last_baro is not None else float("nan"))
                sample = Sample(low_g=tuple(value*G for value in accel_g),
                                high_g=(float("nan"),)*3, gyro_dps=gyro,
                                baro_altitude_m=altitude, baro_pressure_hpa=pressure,
                                low_g_valid=finite_accel, high_g_valid=False,
                                gyro_valid=finite_gyro,
                                baro_valid=(new_low is not None and pressure > 0 and
                                            math.isfinite(pressure) and math.isfinite(altitude)))
                phase = ("pad" if last_baro is None or not last_baro["Liftoff"] else
                         ("descent" if last_baro["Apogee"] else "ascent"))
                row = Row(hr_us-first_hr_us, sample, phase=phase)
                input_writer.writerow(_as_row(row))
                processed += 1
                if hr_us > last_lr_us:
                    after_lr_end += 1
                if runner is None:
                    runner = Runner(sample, row.timestamp_us)
                    continue
                estimate = runner.step(sample, row.timestamp_us)
                full_hr_state_finite &= all(math.isfinite(value) for values in (
                    estimate["position_m"], estimate["velocity_mps"],
                    estimate["acceleration_mps2"], estimate["quaternion"],
                    *(line for matrix in estimate["covariance"] for line in matrix))
                    for value in values)
                record = {"flight_time_s": hr_us*1e-6,
                          "timestamp_us": row.timestamp_us,
                          "low_g_variance_scale": estimate["low_g_variance_scale"],
                          "barometer_update": int(sample.baro_valid)}
                for name, values, suffix in (("position", estimate["position_m"], "m"),
                                              ("velocity", estimate["velocity_mps"], "mps"),
                                              ("acceleration", estimate["acceleration_mps2"], "mps2")):
                    for axis, value in zip("xyz", values):
                        record[f"{name}_{axis}_{suffix}"] = value
                for component, value in zip("wxyz", estimate["quaternion"]):
                    record[f"quaternion_{component}"] = value
                hr_writer.writerow(record)
                if new_low is not None:
                    if alignment is None:
                        alignment = quaternion_multiply(
                            estimate["quaternion"], quaternion_conjugate(hr_quat))
                    report_pairs.append((row, estimate))
                    compare_rows.append(_comparison(new_low, hr_us, hr_quat,
                                                    estimate, alignment))
    finally:
        if runner is not None:
            runner.close()
    if not compare_rows:
        raise ValueError("high and low-rate logs have no usable overlapping samples")
    metadata = dict(DEFAULT_METADATA)
    metadata.update({
        "source_high_rate": str(high_path), "source_low_rate": str(low_path),
        "frame": "COTS body XYZ; initial -X gravity direction aligned to local +Z by production estimator",
        "altitude_datum": "COTS barometric AGL feet converted to meters",
        "units": "COTS accelerometer inferred g, gyro inferred deg/s, LR pressure atm, altitude feet",
        "fusion": "one COTS accelerometer mapped to low-g; high-g unavailable; LR barometer applied once per new 20 ms sample",
        "timestamp_origin": f"COTS flight time {first_hr_us*1e-6:.6f} s",
        "sensor_limits": "raw COTS values are passed even beyond firmware ±16 g and ±250 deg/s; algorithm comparison only",
    })
    (output / "input.json").write_text(json.dumps(metadata, indent=2) + "\n")
    navigation = write_report(output, report_pairs, plots=plots)
    with (output / "comparison.csv").open("w", newline="") as file:
        writer = csv.DictWriter(file, fieldnames=list(compare_rows[0]))
        writer.writeheader()
        writer.writerows(compare_rows)
    if plots:
        plot_comparison(output, compare_rows)
    apogee_index = next((i for i, r in enumerate(compare_rows) if r["apogee"]),
                        len(compare_rows))
    summary = {
        "high_rate_source_rows_scanned": high_count,
        "high_rate_steps_processed": processed,
        "low_rate_source_rows": len(low_rows),
        "matched_comparison_rows": len(compare_rows),
        "high_rate_last_flight_time_s": last_hr_us*1e-6,
        "low_rate_last_flight_time_s": last_lr_us*1e-6,
        "high_rate_samples_after_low_rate_end": after_lr_end,
        "full_high_rate_state_and_covariance_finite": bool(full_hr_state_finite),
        "processed_accel_samples_above_firmware_16g": accel_above_16,
        "processed_gyro_samples_above_firmware_250dps": gyro_above_250,
        "comparison_is_ground_truth": False,
        "overall_difference": _difference_metrics(compare_rows),
        "early_ascent_0_to_7s_difference": _difference_metrics(
            [r for r in compare_rows if 0 <= r["flight_time_s"] < 7]),
        "ascent_difference": _difference_metrics(
            [r for r in compare_rows[:apogee_index] if r["flight_time_s"] >= 0]),
        "navigation_health_at_lr_times": navigation,
    }
    (output / "comparison_summary.json").write_text(json.dumps(summary, indent=2) + "\n")
    return summary


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("high_rate_csv", type=Path)
    parser.add_argument("low_rate_csv", type=Path)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--plots", action="store_true")
    args = parser.parse_args()
    result = run_demo(args.high_rate_csv, args.low_rate_csv, args.output, args.plots)
    print(json.dumps({key: value for key, value in result.items()
                      if key not in ("overall_difference", "ascent_difference",
                                     "early_ascent_0_to_7s_difference",
                                     "navigation_health_at_lr_times")}, indent=2))


if __name__ == "__main__":
    main()
