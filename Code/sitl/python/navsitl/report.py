"""Aligned machine-readable output and noninteractive diagnostic plots."""

import csv
import json
import math
from pathlib import Path

from .metrics import covariance_health, norm, orientation_error_deg, summarize

NAMES = ("low_x", "low_y", "low_z", "high_x", "high_y", "high_z", "baro_z")


def _record(row, est):
    s = row.sample
    record = {"timestamp_us": row.timestamp_us, "phase": row.phase,
              "low_g_variance_scale": est["low_g_variance_scale"],
              "gyro_used": int(est["gyro_used"]), "duration_ns": est["duration_ns"],
              "dt_raw_s": est["dt_raw_s"], "dt_applied_s": est["dt_applied_s"],
              "dt_filter_s": est["dt_filter_s"],
              "quaternion_norm": norm(est["quaternion"]),
              "quaternion_norm_before_normalization":
                  est["quaternion_norm_before_normalization"]}
    for kind, values in (("low_g_mps2", s.low_g), ("high_g_mps2", s.high_g),
                         ("gyro_dps", s.gyro_dps)):
        for axis, value in zip("xyz", values):
            record[f"{kind}_{axis}"] = value
    record["baro_altitude_m"] = s.baro_altitude_m
    record["baro_pressure_hpa"] = s.baro_pressure_hpa
    for sensor in ("low_g", "high_g", "gyro", "baro"):
        record[f"{sensor}_valid"] = int(getattr(s, f"{sensor}_valid"))
    record["gps_latitude_deg"] = s.gps_latitude_deg
    record["gps_longitude_deg"] = s.gps_longitude_deg
    record["gps_altitude_m"] = s.gps_altitude_m
    record["gps_fix"] = s.gps_fix
    record["gps_satellites"] = s.gps_satellites
    record["gps_utc"] = s.gps_utc
    symmetry, psd = covariance_health(est["covariance"])
    record["covariance_symmetry_error"] = symmetry
    record["covariance_psd"] = int(psd)
    for kind in ("position_m", "velocity_mps", "acceleration_mps2"):
        for axis, value in zip("xyz", est[kind]):
            record[f"estimate_{kind}_{axis}"] = value
        if row.truth:
            for axis, value in zip("xyz", getattr(row.truth, kind)):
                record[f"truth_{kind}_{axis}"] = value
            record[kind + "_error_norm"] = norm(
                a-b for a, b in zip(est[kind], getattr(row.truth, kind)))
    for part, value in zip("wxyz", est["quaternion"]):
        record[f"quaternion_{part}"] = value
    for name, value in zip(("roll", "pitch", "yaw"), est["euler_deg"]):
        record[f"{name}_deg"] = value
    if row.truth:
        for part, value in zip("wxyz", row.truth.quaternion):
            record[f"truth_quaternion_{part}"] = value
        record["orientation_error_deg"] = orientation_error_deg(
            est["quaternion"], row.truth.quaternion)
    for axis, matrix in zip("xyz", est["covariance"]):
        for i in range(3):
            for j in range(3):
                record[f"cov_{axis}_{i}{j}"] = matrix[i][j]
    for name, measurement in zip(NAMES, est["measurements"]):
        for key, value in measurement.items():
            record[f"{name}_{key}"] = int(value) if isinstance(value, bool) else value
    return record


def write_report(directory, pairs, plots=False):
    directory = Path(directory)
    directory.mkdir(parents=True, exist_ok=True)
    records = [_record(row, est) for row, est in pairs]
    fields = list(dict.fromkeys(key for record in records for key in record))
    with (directory / "estimates.csv").open("w", newline="") as file:
        writer = csv.DictWriter(file, fieldnames=fields)
        writer.writeheader()
        writer.writerows(records)
    summary = summarize(pairs)
    (directory / "metrics.json").write_text(json.dumps(summary, indent=2,
                                                      allow_nan=False) + "\n")
    if plots:
        plot_report(directory, records)
    return summary


def plot_report(directory, records):
    import matplotlib
    matplotlib.use("Agg")
    import matplotlib.pyplot as plt

    directory = Path(directory)
    t = [row["timestamp_us"]*1e-6 for row in records]

    def save(name, labels, series, ylabel):
        fig, axes = plt.subplots(len(labels), 1, figsize=(12, 2.5*len(labels)),
                                 sharex=True, squeeze=False)
        units = ylabel if isinstance(ylabel, (tuple, list)) else [ylabel]*len(labels)
        for ax, label, traces, unit in zip(axes[:, 0], labels, series, units):
            for trace_name, values in traces:
                ax.plot(t, values, label=trace_name, linewidth=1)
            ax.set_ylabel(f"{label}\n{unit}")
            ax.grid(True, alpha=.3)
            ax.legend(loc="best")
        axes[-1, 0].set_xlabel("time (s)")
        fig.tight_layout()
        fig.savefig(directory / name, dpi=120)
        plt.close(fig)

    for kind, units in (("position_m", "m"), ("velocity_mps", "m/s"),
                        ("acceleration_mps2", "m/s²")):
        labels, series = list("xyz"), []
        for axis in labels:
            traces = [("estimate", [r[f"estimate_{kind}_{axis}"] for r in records])]
            if f"truth_{kind}_{axis}" in records[0]:
                traces.append(("truth", [r.get(f"truth_{kind}_{axis}", math.nan)
                                         for r in records]))
            if kind == "position_m" and axis == "z":
                traces.append(("barometer", [r["baro_altitude_m"] for r in records]))
                traces.append(("GPS (not fused)", [r["gps_altitude_m"] if
                                               r["gps_altitude_m"] is not None else math.nan
                                               for r in records]))
            if kind == "acceleration_mps2" and axis == "z":
                traces.extend((("low-g body Z − g", [r["low_g_mps2_z"]-9.81
                                                       for r in records]),
                               ("high-g body Z − g", [r["high_g_mps2_z"]-9.81
                                                        for r in records])))
            series.append(traces)
        save(kind + ".png", labels, series, units)

    sensor_labels = [f"accelerometer {axis.upper()}" for axis in "xyz"] + [
        f"gyro {axis.upper()}" for axis in "xyz"] + ["altitude", "validity"]
    sensor_series = []
    for axis in "xyz":
        sensor_series.append([
            ("low-g body", [r[f"low_g_mps2_{axis}"] for r in records]),
            ("high-g body", [r[f"high_g_mps2_{axis}"] for r in records])])
    for axis in "xyz":
        sensor_series.append([("body rate", [r[f"gyro_dps_{axis}"] for r in records])])
    sensor_series.append([
        ("barometer", [r["baro_altitude_m"] for r in records]),
        ("GPS (not fused)", [r["gps_altitude_m"] if r["gps_altitude_m"] is not None
                             else math.nan for r in records])])
    sensor_series.append([(sensor, [r[f"{sensor}_valid"] for r in records])
                          for sensor in ("low_g", "high_g", "gyro", "baro")])
    save("measurements.png", sensor_labels, sensor_series,
         ["m/s²"]*3 + ["deg/s"]*3 + ["m", "0/1"])

    save("attitude.png", ["quaternion norm", "orientation error", "Euler angles"], [
        [("after normalization", [r["quaternion_norm"] for r in records]),
         ("before normalization", [r["quaternion_norm_before_normalization"]
                                   for r in records])],
        [("angle to truth", [r.get("orientation_error_deg", math.nan) for r in records])],
        [(name, [r[f"{name}_deg"] for r in records]) for name in
         ("roll", "pitch", "yaw")]], "degrees / norm")
    save("confidence_innovations.png", ["low-g variance scale", "innovation",
                                        "normalized innovation", "update applied"], [
        [("scale", [r["low_g_variance_scale"] for r in records])],
        [(name, [r[f"{name}_innovation"] if r[f"{name}_applied"] else math.nan
                 for r in records]) for name in NAMES],
        [(name, [r[f"{name}_innovation"]/
                 math.sqrt(r[f"{name}_innovation_variance"])
                 if r[f"{name}_applied"] and r[f"{name}_innovation_variance"] > 0
                 else math.nan for r in records]) for name in NAMES],
        [(name, [r[f"{name}_applied"] for r in records]) for name in NAMES]],
         ["multiplier", "m/s² or m", "standard deviations", "0/1"])
    save("covariance.png", list("xyz") + ["health"], [
        [(name, [r[f"cov_{axis}_{index}{index}"] for r in records])
         for index, name in enumerate(("position", "velocity", "acceleration"))]
        for axis in "xyz"] + [[
            ("symmetry error", [r["covariance_symmetry_error"] for r in records]),
            ("PSD (1 pass, 0 fail)", [r["covariance_psd"] for r in records])]],
         "variance / health")
    save("timing.png", ["time step", "host duration"], [
        [(name, [r[name] for r in records]) for name in
         ("dt_raw_s", "dt_applied_s", "dt_filter_s")],
        [("native step + ctypes (ms)", [r["duration_ns"]*1e-6 for r in records])]],
         "s / ms")
    if "position_m_error_norm" in records[0]:
        save("errors.png", ["position", "velocity", "acceleration"], [
            [("3D error", [r.get(kind + "_error_norm", math.nan) for r in records])]
            for kind in ("position_m", "velocity_mps", "acceleration_mps2")],
            "m / m/s / m/s²")
