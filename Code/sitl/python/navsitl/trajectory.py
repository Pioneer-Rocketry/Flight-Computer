"""Analytical trajectories and illustrative deterministic post-driver sensors."""

import math
import random
from dataclasses import dataclass, replace

from .schema import Row, Sample, Truth, pressure_from_altitude

G = 9.81
SCENARIOS = (
    "stationary", "rotation_x", "rotation_xyz", "constant_acceleration",
    "vertical_boost", "saturation_transition", "full_flight", "variable_dt",
    "scheduler_stall", "noise_bias", "gyro_bias", "baro_outlier",
    "invalid", "dropout", "long_duration",
)


@dataclass(frozen=True)
class SensorConfig:
    # All defaults except configured full-scale ranges are illustrative.
    low_g_sigma_mps2: float = 0.0
    high_g_sigma_mps2: float = 0.0
    gyro_sigma_dps: float = 0.0
    baro_sigma_m: float = 0.0
    low_g_bias_mps2: tuple[float, float, float] = (0.0, 0.0, 0.0)
    high_g_bias_mps2: tuple[float, float, float] = (0.0, 0.0, 0.0)
    gyro_bias_dps: tuple[float, float, float] = (0.0, 0.0, 0.0)
    low_g_full_scale_mps2: float = 16.0 * G
    high_g_full_scale_mps2: float = 32.0 * G
    gyro_full_scale_dps: float = 250.0
    baro_period_us: int = 0  # zero means sample on every navigation step
    gps_period_us: int = 100_000
    baro_outlier_m: float = 0.0
    baro_outlier_time_s: float = 1.0


def _normal(rng):
    # Box-Muller uses Python's specified random() stream and an explicit draw order.
    u1 = max(rng.random(), 2**-53)
    u2 = rng.random()
    return math.sqrt(-2.0 * math.log(u1)) * math.cos(2.0 * math.pi * u2)


def _noisy(value, sigma, rng):
    return value + sigma * _normal(rng) if sigma else value


def _clip(value, limit):
    return max(-limit, min(limit, value))


def _quat(rate_dps, time_s):
    mag = math.sqrt(sum(x*x for x in rate_dps))
    if mag == 0:
        return (1.0, 0.0, 0.0, 0.0)
    half_angle = math.radians(mag) * time_s / 2.0
    scale = math.sin(half_angle) / mag
    return (math.cos(half_angle), *(x * scale for x in rate_dps))


def _rotate(q, v):
    w, x, y, z = q
    vx, vy, vz = v
    cx, cy, cz = y*vz-z*vy, z*vx-x*vz, x*vy-y*vx
    return (vx + 2*(w*cx + y*cz-z*cy),
            vy + 2*(w*cy + z*cx-x*cz),
            vz + 2*(w*cz + x*cy-y*cx))


def _segments(t, segments):
    z = v = previous = 0.0
    for end, acceleration, phase in segments:
        dt = max(0.0, min(t, end) - previous)
        z += v*dt + 0.5*acceleration*dt*dt
        v += acceleration*dt
        if t <= end:
            return z, v, acceleration, phase
        previous = end
    return z, v, 0.0, "landing"


def _truth(scenario, t):
    q = (1.0, 0.0, 0.0, 0.0)
    rate = (0.0, 0.0, 0.0)
    if scenario == "rotation_x":
        rate = (60.0, 0.0, 0.0)
        q = _quat(rate, t)
    elif scenario == "rotation_xyz":
        rate = (30.0, 20.0, 40.0)
        q = _quat(rate, t)
    if scenario == "constant_acceleration":
        if t <= 0.2:
            return Truth((0.0, 0.0, 0.0), (0.0, 0.0, 0.0),
                         (0.0, 0.0, 0.0), q), rate, "pad"
        a = (1.0, -0.5, 3.0)
        dt = t-0.2
        return Truth(tuple(0.5*x*dt*dt for x in a), tuple(x*dt for x in a), a, q), rate, "acceleration"
    if scenario in ("vertical_boost", "dropout"):
        z, v, az, phase = _segments(t, ((3.0, 30.0, "boost"), (1e9, -G, "coast")))
    elif scenario == "saturation_transition":
        z, v, az, phase = _segments(t, ((1.0, 80.0, "boost"),
                                           (2.0, 200.0, "low_g_saturated"),
                                           (3.0, 80.0, "boost"),
                                           (1e9, -G, "coast")))
    elif scenario in ("full_flight", "scheduler_stall", "variable_dt"):
        apogee = 5.0 + 90.0/G
        main = apogee + 5.0
        descent = main + 3.0
        segments = ((2.0, 0.0, "pad"), (5.0, 30.0, "boost"),
                    (apogee, -G, "coast"), (main, -2.0, "drogue"),
                    (descent, 2.0, "main_deceleration"))
        z0, _, _, _ = _segments(descent, segments)
        landing = descent + z0/4.0
        z, v, az, phase = _segments(t, segments + ((landing, 0.0, "main_descent"),))
        if t >= landing:
            z = v = az = 0.0
            phase = "landing"
    else:
        z = v = az = 0.0
        phase = "pad" if scenario == "stationary" else scenario
    return Truth((0.0, 0.0, z), (0.0, 0.0, v),
                 (0.0, 0.0, az), q), rate, phase


def generate(scenario="full_flight", duration_s=None, dt_us=2000,
             seed=1, sensors=None):
    """Generate rows including an initial sample for quaternion initialization."""
    if scenario not in SCENARIOS:
        raise ValueError(f"unknown scenario {scenario}; choose from {SCENARIOS}")
    if dt_us <= 0:
        raise ValueError("dt_us must be positive")
    if duration_s is None:
        duration_s = 220.0 if scenario == "full_flight" else (
            120.0 if scenario == "long_duration" else 5.0)
    if duration_s <= 0:
        raise ValueError("duration_s must be positive")
    config = sensors or SensorConfig()
    if scenario == "noise_bias":
        config = replace(config, low_g_sigma_mps2=0.12, high_g_sigma_mps2=0.4,
                         gyro_sigma_dps=0.05, baro_sigma_m=1.5,
                         low_g_bias_mps2=(0.02, -0.01, 0.04))
    elif scenario == "gyro_bias":
        config = replace(config, gyro_bias_dps=(0.3, -0.1, 0.2))
    elif scenario == "baro_outlier":
        config = replace(config, baro_outlier_m=100.0)
    rng = random.Random(seed)
    t_us = 0
    end_us = round(duration_s * 1_000_000)
    last_baro_us = last_gps_us = -10**18
    held_baro = 0.0
    held_gps = None
    rows = []
    outlier_done = False
    step_index = 0
    while t_us <= end_us:
        t = t_us / 1_000_000.0
        truth, rate, phase = _truth(scenario, t)
        # Navigation gravity is (0,0,-G); accelerometers measure specific force.
        inertial_specific_force = (truth.acceleration_mps2[0],
                                   truth.acceleration_mps2[1],
                                   truth.acceleration_mps2[2] + G)
        q = truth.quaternion
        body_force = _rotate((q[0], -q[1], -q[2], -q[3]), inertial_specific_force)
        low = tuple(_clip(_noisy(v+b, config.low_g_sigma_mps2, rng),
                          config.low_g_full_scale_mps2)
                    for v, b in zip(body_force, config.low_g_bias_mps2))
        high = tuple(_clip(_noisy(v+b, config.high_g_sigma_mps2, rng),
                           config.high_g_full_scale_mps2)
                     for v, b in zip(body_force, config.high_g_bias_mps2))
        gyro = tuple(_clip(_noisy(v+b, config.gyro_sigma_dps, rng),
                           config.gyro_full_scale_dps)
                     for v, b in zip(rate, config.gyro_bias_dps))
        if t_us - last_baro_us >= config.baro_period_us:
            held_baro = _noisy(truth.position_m[2], config.baro_sigma_m, rng)
            if (config.baro_outlier_m and not outlier_done and
                    t >= config.baro_outlier_time_s):
                held_baro += config.baro_outlier_m
                outlier_done = True
            last_baro_us = t_us
        if t_us - last_gps_us >= config.gps_period_us:
            held_gps = truth.position_m[2]
            last_gps_us = t_us
        low_valid = high_valid = gyro_valid = baro_valid = True
        if scenario == "dropout" and 2.5 <= t < 3.5:
            low_valid = high_valid = baro_valid = False
        if scenario == "invalid" and 1.0 <= t < 1.02:
            low = (float("nan"), low[1], low[2])
            gyro = (float("nan"), gyro[1], gyro[2])
            held_baro = float("nan")
        sample = Sample(low, high, gyro, held_baro,
                        pressure_from_altitude(held_baro) if math.isfinite(held_baro)
                        else float("nan"), low_valid, high_valid, gyro_valid,
                        baro_valid, 0.0, 0.0, held_gps, 3, 8, None)
        rows.append(Row(t_us, sample, truth, phase))
        step_index += 1
        interval = dt_us
        if scenario == "variable_dt":
            interval = (dt_us//2, dt_us*3//2, dt_us)[step_index % 3]
        if scenario == "scheduler_stall" and t_us < 1_000_000 <= t_us + interval:
            interval = 100_000
        t_us += max(1, interval)
    return rows
