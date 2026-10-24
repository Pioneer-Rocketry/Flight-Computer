"""Versioned ctypes boundary. Field order matches native/nav_sitl.h exactly."""

import ctypes as C
import os
from pathlib import Path
from time import perf_counter_ns


F3 = C.c_float * 3
F4 = C.c_float * 4


class CSample(C.Structure):
    _fields_ = [("struct_size", C.c_uint32), ("low_g", F3), ("high_g", F3),
                ("gyro_dps", F3), ("baro_altitude_m", C.c_float),
                ("baro_pressure_hpa", C.c_float), ("low_g_valid", C.c_uint8),
                ("high_g_valid", C.c_uint8), ("gyro_valid", C.c_uint8),
                ("baro_valid", C.c_uint8)]


class CResult(C.Structure):
    _fields_ = [("struct_size", C.c_uint32), ("position_m", F3),
                ("velocity_mps", F3), ("acceleration_mps2", F3),
                ("quaternion", F4), ("euler_deg", F3),
                ("quaternion_norm_before_normalization", C.c_float),
                ("dt_raw_s", C.c_float), ("dt_applied_s", C.c_float),
                ("dt_filter_s", C.c_float)]


class CMeasurement(C.Structure):
    _fields_ = [("innovation", C.c_float),
                ("innovation_variance", C.c_float),
                ("measurement_variance", C.c_float), ("applied", C.c_uint8)]


class CDiagnostics(C.Structure):
    _fields_ = [("struct_size", C.c_uint32),
                ("covariance", ((C.c_float * 3) * 3) * 3),
                ("measurements", CMeasurement * 7),
                ("low_g_variance_scale", C.c_float), ("gyro_used", C.c_uint8)]


def _native(sample):
    return CSample(C.sizeof(CSample), F3(*sample.low_g), F3(*sample.high_g),
                   F3(*sample.gyro_dps), sample.baro_altitude_m,
                   sample.baro_pressure_hpa, int(sample.low_g_valid),
                   int(sample.high_g_valid), int(sample.gyro_valid),
                   int(sample.baro_valid))


def _library(path=None):
    path = path or os.environ.get("NAV_SITL_LIB") or (
        Path(__file__).resolve().parents[2] / "build/libnavsitl.so")
    lib = C.CDLL(str(path))
    for name in ("nav_sitl_api_version", "nav_sitl_sample_size",
                 "nav_sitl_result_size", "nav_sitl_diagnostics_size"):
        getattr(lib, name).restype = C.c_uint32
    expected = (1, C.sizeof(CSample), C.sizeof(CResult), C.sizeof(CDiagnostics))
    actual = tuple(getattr(lib, name)() for name in
                   ("nav_sitl_api_version", "nav_sitl_sample_size",
                    "nav_sitl_result_size", "nav_sitl_diagnostics_size"))
    if actual != expected:
        raise RuntimeError(f"SITL ABI mismatch: native {actual}, Python {expected}")
    lib.nav_sitl_create.argtypes = [C.POINTER(CSample), C.c_uint64,
                                    C.POINTER(C.c_void_p)]
    lib.nav_sitl_create.restype = C.c_int
    lib.nav_sitl_step.argtypes = [C.c_void_p, C.POINTER(CSample), C.c_uint64,
                                  C.POINTER(CResult), C.POINTER(CDiagnostics)]
    lib.nav_sitl_step.restype = C.c_int
    lib.nav_sitl_destroy.argtypes = [C.c_void_p]
    return lib


class Runner:
    def __init__(self, initial, timestamp_us, library=None):
        self.lib = _library(library)
        self.handle = C.c_void_p()
        sample = _native(initial)
        status = self.lib.nav_sitl_create(C.byref(sample), timestamp_us,
                                          C.byref(self.handle))
        if status != 0:
            raise RuntimeError(f"nav_sitl_create returned {status}")

    def close(self):
        if self.handle:
            self.lib.nav_sitl_destroy(self.handle)
            self.handle = C.c_void_p()

    def __enter__(self):
        return self

    def __exit__(self, *_):
        self.close()

    def step(self, sample, timestamp_us):
        if not self.handle:
            raise RuntimeError("SITL runner is closed")
        native = _native(sample)
        result = CResult()
        result.struct_size = C.sizeof(CResult)
        diag = CDiagnostics()
        diag.struct_size = C.sizeof(CDiagnostics)
        start = perf_counter_ns()
        status = self.lib.nav_sitl_step(self.handle, C.byref(native), timestamp_us,
                                        C.byref(result), C.byref(diag))
        duration_ns = perf_counter_ns() - start
        if status < 0:
            raise ValueError(f"nav_sitl_step returned {status} at {timestamp_us} us")
        return {
            "timestamp_us": timestamp_us, "stepped": status == 0,
            "position_m": tuple(result.position_m),
            "velocity_mps": tuple(result.velocity_mps),
            "acceleration_mps2": tuple(result.acceleration_mps2),
            "quaternion": tuple(result.quaternion),
            "euler_deg": tuple(result.euler_deg),
            "quaternion_norm_before_normalization":
                result.quaternion_norm_before_normalization,
            "dt_raw_s": result.dt_raw_s,
            "dt_applied_s": result.dt_applied_s,
            "dt_filter_s": result.dt_filter_s,
            "covariance": tuple(tuple(tuple(value for value in row)
                                      for row in axis) for axis in diag.covariance),
            "measurements": tuple({"innovation": m.innovation,
                                   "innovation_variance": m.innovation_variance,
                                   "measurement_variance": m.measurement_variance,
                                   "applied": bool(m.applied)}
                                  for m in diag.measurements),
            "low_g_variance_scale": diag.low_g_variance_scale,
            "gyro_used": bool(diag.gyro_used),
            "duration_ns": duration_ns,
        }


def run_rows(rows, library=None):
    """Return [(input Row, estimate dict)] for every row after initialization."""
    rows = iter(rows)
    first = next(rows, None)
    if first is None:
        raise ValueError("at least one input row is required")
    output = []
    with Runner(first.sample, first.timestamp_us, library) as runner:
        for row in rows:
            output.append((row, runner.step(row.sample, row.timestamp_us)))
    return output
