"""Version 1 engineering-unit replay format and source-column mapping."""

import csv
import json
import math
from dataclasses import dataclass
from pathlib import Path

AXES = "xyz"
ACCEL_FIELDS = [f"{kind}_{axis}_mps2" for kind in ("low_g", "high_g") for axis in AXES]
GYRO_FIELDS = [f"gyro_{axis}_dps" for axis in AXES]
TRUTH_FIELDS = ([f"truth_position_{axis}_m" for axis in AXES] +
                [f"truth_velocity_{axis}_mps" for axis in AXES] +
                [f"truth_acceleration_{axis}_mps2" for axis in AXES] +
                [f"truth_quaternion_{part}" for part in "wxyz"])
CSV_FIELDS = (["timestamp_us"] + ACCEL_FIELDS + GYRO_FIELDS +
              ["baro_altitude_m", "baro_pressure_hpa", "low_g_valid",
               "high_g_valid", "gyro_valid", "baro_valid", "gps_latitude_deg",
               "gps_longitude_deg", "gps_altitude_m", "gps_fix",
               "gps_satellites", "gps_utc", "phase"] + TRUTH_FIELDS)
DEFAULT_METADATA = {
    "schema_version": 1,
    "frame": "local XYZ, +Z up; body XYZ; quaternion [w,x,y,z] body-to-navigation",
    "altitude_datum": "barometer 1013.25 hPa reference; not automatically launch-relative",
    "units": "timestamps us; accelerometer specific force m/s^2; gyro deg/s; altitude m; pressure hPa",
    "sensor_parameters": "illustrative unless calibrated from hardware",
}
BARO_HEIGHT_M = 145366.45 * 0.3048


@dataclass(frozen=True)
class Sample:
    low_g: tuple[float, float, float] = (0.0, 0.0, 9.81)
    high_g: tuple[float, float, float] = (0.0, 0.0, 9.81)
    gyro_dps: tuple[float, float, float] = (0.0, 0.0, 0.0)
    baro_altitude_m: float = 0.0
    baro_pressure_hpa: float = 1013.25
    low_g_valid: bool = True
    high_g_valid: bool = True
    gyro_valid: bool = True
    baro_valid: bool = True
    gps_latitude_deg: float | None = None
    gps_longitude_deg: float | None = None
    gps_altitude_m: float | None = None
    gps_fix: int | None = None
    gps_satellites: int | None = None
    gps_utc: str | None = None


@dataclass(frozen=True)
class Truth:
    position_m: tuple[float, float, float]
    velocity_mps: tuple[float, float, float]
    acceleration_mps2: tuple[float, float, float]
    quaternion: tuple[float, float, float, float]


@dataclass(frozen=True)
class Row:
    timestamp_us: int
    sample: Sample
    truth: Truth | None = None
    phase: str = ""


def altitude_from_pressure(pressure_hpa):
    if not math.isfinite(pressure_hpa) or pressure_hpa <= 0:
        return float("nan")
    return (1.0 - (pressure_hpa / 1013.25) ** 0.190284) * BARO_HEIGHT_M


def pressure_from_altitude(altitude_m):
    return 1013.25 * max(1e-6, 1.0 - altitude_m / BARO_HEIGHT_M) ** (1 / 0.190284)


def _as_row(row):
    s = row.sample
    out = {"timestamp_us": row.timestamp_us, "phase": row.phase}
    for kind in ("low_g", "high_g"):
        out.update({f"{kind}_{axis}_mps2": value for axis, value in zip(AXES, getattr(s, kind))})
    out.update({f"gyro_{axis}_dps": value for axis, value in zip(AXES, s.gyro_dps)})
    for field in ("baro_altitude_m", "baro_pressure_hpa", "low_g_valid",
                  "high_g_valid", "gyro_valid", "baro_valid", "gps_latitude_deg",
                  "gps_longitude_deg", "gps_altitude_m", "gps_fix",
                  "gps_satellites", "gps_utc"):
        value = getattr(s, field)
        out[field] = int(value) if isinstance(value, bool) else value
    if row.truth:
        for kind, suffix in (("position_m", "m"), ("velocity_mps", "mps"),
                             ("acceleration_mps2", "mps2")):
            out.update({f"truth_{kind.split('_')[0]}_{axis}_{suffix}": value
                        for axis, value in zip(AXES, getattr(row.truth, kind))})
        out.update({f"truth_quaternion_{part}": value
                    for part, value in zip("wxyz", row.truth.quaternion)})
    return out


def write_rows(path, rows, metadata=None):
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", newline="") as file:
        writer = csv.DictWriter(file, fieldnames=CSV_FIELDS)
        writer.writeheader()
        for row in rows:
            writer.writerow(_as_row(row))
    meta = dict(DEFAULT_METADATA)
    meta.update(metadata or {})
    path.with_suffix(".json").write_text(json.dumps(meta, indent=2) + "\n")


def _mapped(raw, field, mapping):
    specification = mapping.get(field, field)
    if isinstance(specification, str):
        return raw.get(specification, "")
    value = raw.get(specification["column"], "")
    if value in ("", None):
        return ""
    return float(value) * specification.get("scale", 1.0) + specification.get("offset", 0.0)


def _float(raw, field, mapping, default=None):
    value = _mapped(raw, field, mapping)
    return default if value in ("", None) else float(value)


def _bool(raw, field, mapping, inferred):
    value = _mapped(raw, field, mapping)
    if value in ("", None):
        return inferred
    if str(value).lower() in ("1", "true", "yes"):
        return True
    if str(value).lower() in ("0", "false", "no"):
        return False
    raise ValueError(f"invalid {field}: {value}")


def read_rows(path, metadata_path=None):
    """Read canonical CSV or a source log with metadata.column_map entries.

    A map entry is a column name or {column, scale, offset}; scale and offset
    explicitly convert a source unit to the canonical unit.
    """
    path = Path(path)
    meta_path = Path(metadata_path) if metadata_path else path.with_suffix(".json")
    metadata = json.loads(meta_path.read_text())
    if metadata.get("schema_version") != 1:
        raise ValueError("replay metadata must declare schema_version 1")
    if not metadata.get("frame") or not metadata.get("altitude_datum") or not metadata.get("units"):
        raise ValueError("replay metadata must declare frame, altitude_datum and units")
    mapping = metadata.get("column_map", {})
    output = []
    with path.open(newline="") as file:
        for raw in csv.DictReader(file):
            t = _float(raw, "timestamp_us", mapping)
            if t is None or not math.isfinite(t) or t < 0 or int(t) != t:
                raise ValueError("timestamps must be nonnegative integer microseconds")
            t = int(t)
            if output and t <= output[-1].timestamp_us:
                raise ValueError("timestamps must increase strictly")
            low = tuple(_float(raw, f"low_g_{a}_mps2", mapping, float("nan")) for a in AXES)
            high = tuple(_float(raw, f"high_g_{a}_mps2", mapping, float("nan")) for a in AXES)
            gyro = tuple(_float(raw, f"gyro_{a}_dps", mapping, float("nan")) for a in AXES)
            altitude = _float(raw, "baro_altitude_m", mapping)
            pressure = _float(raw, "baro_pressure_hpa", mapping)
            if altitude is None and pressure is not None:
                altitude = altitude_from_pressure(pressure)
            if pressure is None and altitude is not None:
                pressure = pressure_from_altitude(altitude)
            s = Sample(low, high, gyro, altitude if altitude is not None else float("nan"),
                       pressure if pressure is not None else float("nan"),
                       _bool(raw, "low_g_valid", mapping, all(math.isfinite(v) for v in low)),
                       _bool(raw, "high_g_valid", mapping, all(math.isfinite(v) for v in high)),
                       _bool(raw, "gyro_valid", mapping, all(math.isfinite(v) for v in gyro)),
                       _bool(raw, "baro_valid", mapping,
                             altitude is not None and pressure is not None and pressure > 0),
                       _float(raw, "gps_latitude_deg", mapping),
                       _float(raw, "gps_longitude_deg", mapping),
                       _float(raw, "gps_altitude_m", mapping),
                       None if _mapped(raw, "gps_fix", mapping) in ("", None)
                       else int(_mapped(raw, "gps_fix", mapping)),
                       None if _mapped(raw, "gps_satellites", mapping) in ("", None)
                       else int(_mapped(raw, "gps_satellites", mapping)),
                       _mapped(raw, "gps_utc", mapping) or None)
            truth_values = [_float(raw, field, mapping) for field in TRUTH_FIELDS]
            if any(v is not None for v in truth_values) and not all(v is not None for v in truth_values):
                raise ValueError("ground truth must contain all position, velocity, acceleration and quaternion fields")
            truth = None
            if all(v is not None for v in truth_values):
                truth = Truth(tuple(truth_values[0:3]), tuple(truth_values[3:6]),
                              tuple(truth_values[6:9]), tuple(truth_values[9:13]))
            output.append(Row(t, s, truth, _mapped(raw, "phase", mapping) or ""))
    if not output:
        raise ValueError("replay contains no samples")
    return output, metadata
