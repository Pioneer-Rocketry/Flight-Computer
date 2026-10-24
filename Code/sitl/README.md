# Navigation software-in-the-loop testing

This harness executes the same `NavigationEstimator.cpp` source as the STM32
firmware. Python supplies post-driver sensor values and timestamps through a
versioned C ABI. The firmware `Navigation` class still owns the real IMU,
barometer, GPS, and MCU clock. This tests estimator logic; it does not test SPI,
DMA, sensor register conversion, sensor startup calibration, interrupt timing,
or deployment hardware.

## Build and run

From the repository root on Linux, with `g++`, `make`, and Python 3.10+:

```sh
make -C Code/sitl all
make -C Code/sitl test
PYTHONPATH=Code/sitl/python python3 -m navsitl list-scenarios
PYTHONPATH=Code/sitl/python python3 -m navsitl synthetic full_flight \
    --duration-s 30 --dt-us 5000 --seed 1 --save-input \
    --output Code/sitl/build/flight
```

To save PNG plots, install the optional dependency and add `--plots`:

```sh
python3 -m pip install -r Code/sitl/requirements-plot.txt
PYTHONPATH=Code/sitl/python python3 -m navsitl synthetic saturation_transition \
    --duration-s 3.5 --dt-us 2000 --plots --output Code/sitl/build/saturation
```

The output directory contains `estimates.csv`, `metrics.json`, and requested
plots. `--save-input` also writes `input.csv` and its `input.json` metadata.
`estimates.csv` contains measured, estimated, and available truth values,
covariances, per-measurement innovations and applied flags, low-g confidence,
and timing. Reports and the native library live under the ignored `build/`
directory. To build firmware too, run `make -C Code all` with the Arm GNU
toolchain and the checked-out TinyUSB submodule.

## Replay recorded data

```sh
PYTHONPATH=Code/sitl/python python3 -m navsitl replay flight.csv \
    --metadata flight.json --plots --output Code/sitl/build/replay
```

Replay requires a metadata JSON file with `schema_version: 1`, `frame`,
`altitude_datum`, and `units`. A canonical CSV has `timestamp_us`; low-g and
high-g `low_g_x_mps2` etc.; `gyro_x_dps` etc.; `baro_altitude_m` and/or
`baro_pressure_hpa`; the four `*_valid` flags; optional GPS fields; and
optional complete truth fields. See `schema.py` for all column names. Every
timestamp is a nonnegative, strictly increasing integer in microseconds.
Each input row represents a navigation step after the first row, which is the
initial attitude sample. Missing channels are marked invalid. The importer
does not treat GPS as estimator input.

### Synchronized high-rate and low-rate COTS flight demo

`examples/blrv_flight_demo.py` accepts the Blue Raven high-rate (HR) and
low-rate (LR) CSVs from the **same flight**. HR contains 3-axis accelerometer
and gyro samples at 2 ms intervals and COTS quaternions. LR contains pressure,
barometric AGL altitude, and COTS computed navigation fields at 20 ms
intervals. The importer aligns them using `Flight_Time_(s)`, converts HR
acceleration from inferred g to m/s², pressure from atm to hPa, and LR feet
to meters. It calls the production C++ estimator for **every HR sample** and
applies each LR barometer value **once** on arrival. Before the first LR row
and after the last, barometer input is unavailable; comparisons use only the
overlapping span. No repeated barometer reading is treated as a new
independent measurement.

From the repository root:

```sh
make -C Code/sitl all
PYTHONPATH=Code/sitl/python python3 Code/sitl/examples/blrv_flight_demo.py \
    'BlRv_SN2556 HR_03-21-2026_13_46_49.csv' \
    'BlRv_SN2556 LR_03-21-2026_13_46_49.csv' \
    --output Code/sitl/build/blrv-flight-demo --plots
```

The output includes normalized `input.csv`/`input.json`, full-rate
`hr_estimates.csv`, `comparison.csv` at LR times, `comparison_summary.json`,
and PNGs. `comparison_ascent.png` shows the first 40 s at readable scale;
`comparison_early_ascent.png` shows the first eight seconds, and
`comparison_full.png` shows the whole common span. The dedicated example
streams HR samples to avoid holding the full 500 Hz result in memory. COTS inertial altitude,
vertical velocity, horizontal distance from downrange/crossrange components,
tilt, and quaternion are **computed reference outputs**, not independent
ground truth. Feet and feet/s are assumed for the COTS inertial fields based
on their neighboring altitude units; check the device documentation before
using these values as calibrated benchmarks. COTS quaternion order `[w,x,y,z]`
and gyro degrees/s are supported by their measured 2 ms relative rotation.
The quaternion comparison removes one fixed startup-frame rotation; it does
not establish absolute heading. Barometric altitude is fused by the SITL
filter and its residual is not an accuracy score.

Only one HR accelerometer vector is available, so it is mapped to low-g and
high-g is marked unavailable. HR values are passed through without clipping
for an **algorithm comparison**. Many logged gyro rates exceed this firmware's
configured ±250 degrees/s gyro range, and some acceleration samples exceed
its ±16 g low-g range. This run does not claim that the current board could
measure those values. The report counts the out-of-range samples.

Other logs can be mapped without rewriting them. In the metadata, use
`column_map` entries such as:

```json
{
  "schema_version": 1,
  "frame": "body XYZ; local +Z up",
  "altitude_datum": "1013.25 hPa standard pressure",
  "units": "source time ms, pressure Pa; canonical values us and hPa",
  "column_map": {
    "timestamp_us": {"column": "time_ms", "scale": 1000},
    "low_g_z_mps2": "accel_z",
    "baro_pressure_hpa": {"column": "pressure_pa", "scale": 0.01}
  }
}
```

Map every available axis explicitly. The metadata must document any source
sensor mounting rotation, axis inversion, unit conversion, and altitude
reference; the importer applies only the conversions requested in the map.
If only pressure is present, it derives altitude using the barometer driver's
fixed 1013.25 hPa reference equation. This is a host preprocessing step, so
raw ADC compensation and rounding remain outside this SITL boundary. If only
altitude is present, it supplies the matching nominal pressure for the
estimator's existing positive-pressure gate. Logs with partial truth fields
are rejected rather than silently compared against fabricated truth.

## Frames, timing, and synthetic truth

- Navigation coordinates are local XYZ with +Z up. No north/east alignment is
  assumed. Quaternion `[w,x,y,z]` rotates body vectors into this frame.
- Accelerometers measure **specific force**. For inertial acceleration `a`
  and gravity `(0,0,-9.81)` m/s², the sensor model sends
  `R(q)^T (a + (0,0,9.81))`. An upright stationary pad sample reads +9.81
  m/s² on body Z; ideal ballistic coast reads zero.
- The firmware's current gyro mapping is body X→pitch, Y→roll, Z→yaw. Euler
  output labels retain its existing convention. Yaw at startup is arbitrary:
  gravity alone cannot establish heading.
- Barometer altitude uses the driver's standard pressure datum; it is not
  automatically relative to launch altitude or GPS mean sea level. GPS is
  carried for visual comparison only and is never fused.
- Python timestamps are 64-bit and monotonic. The core receives their integer
  microsecond difference. It caps each step at 50 ms and reports both raw and
  applied intervals. The first filter step uses the firmware's 2 ms target
  interval while attitude propagation uses the measured interval.

The synthetic generator supports stationary, exact single- and multi-axis
rotations, piecewise constant acceleration, vertical boost, low-g saturation,
and a full flight with pad, boost, burnout, ballistic coast, apogee, drogue,
main deceleration/descent, and landing. Additional presets vary sampling,
insert a scheduler stall, noise/bias, gyro bias, a barometer outlier, invalid
values, a sensor outage, or a long stationary run. Position and velocity are
integrated analytically within each piecewise segment. The recovery phases
are deliberately simple kinematics, not a validated parachute dynamics model.

The configured sensor ranges match the current firmware: ±16 g low-g, ±32 g
high-g, and ±250 degrees/s gyro. All noise and bias defaults are **illustrative**
and zero unless a preset or config supplies them. They are not hardware
specifications. A sensor configuration JSON may contain any `SensorConfig`
field, for example `{"gyro_bias_dps": [0.3, 0, 0], "baro_sigma_m": 1.5}`;
pass it with `--sensor-config`. Synthetic randomness uses a fixed seed and an
explicit Box-Muller transform. Hold periods, clipping, outliers, and invalid
flags are applied before samples enter C++.

## Interpreting results

The report includes 3D position/velocity/acceleration RMSE and maximum
errors, quaternion orientation error (treating `q` and `-q` as the same
attitude), norm error, coast drift, outage recovery, transition acceleration
jump, covariance finite/symmetry/PSD checks, and host execution time.
Covariance PSD uses all principal minors of the symmetric 3×3 matrix with a
scale-aware float32 tolerance. Host timing includes the ctypes call and is
**not** a Cortex-M real-time bound. CI uses analytical error limits where
appropriate and saves PNGs with Matplotlib's noninteractive backend.

The estimator updates from low-g and high-g acceleration sequentially, then
barometer altitude. It has no hard low-g/high-g selector; the low-g variance
grows near saturation. Finite barometer outliers are currently fused. The
plots show innovation and the resulting state error rather than claiming an
outlier rejection feature.

Known firmware limitations remain visible: `micros()` divides a 32-bit DWT
cycle counter, which wraps about every 23.9 seconds at 180 MHz; the apparent
microsecond reset causes a capped 50 ms step. This host harness accepts
monotonic 64-bit timestamps, so it cannot validate that clock path. The
firmware still ignores IMU read status and has no explicit per-sample validity
signal from its drivers. Host validity and non-finite handling therefore test
the estimator's response at the post-driver boundary; they do not prove that
the current device layer detects a dropout. Neither issue was silently
changed by this extraction.
