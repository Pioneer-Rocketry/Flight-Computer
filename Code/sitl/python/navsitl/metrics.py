"""Numerical checks and ground-truth metrics without plotting dependencies."""

import math
import statistics


def norm(values):
    return math.sqrt(sum(value*value for value in values))


def orientation_error_deg(q, truth):
    qnorm, tnorm = norm(q), norm(truth)
    if not qnorm or not tnorm:
        return float("inf")
    dot = abs(sum(a*b for a, b in zip(q, truth)) / (qnorm*tnorm))
    return math.degrees(2*math.acos(min(1.0, max(-1.0, dot))))


def covariance_health(covariance):
    """Worst symmetry error and PSD result using all 3x3 principal minors."""
    symmetry = 0.0
    psd = True
    for matrix in covariance:
        scale = max(1.0, *(abs(value) for row in matrix for value in row))
        tolerance = 3e-5 * scale
        symmetry = max(symmetry, *(abs(matrix[i][j]-matrix[j][i])
                                    for i in range(3) for j in range(3)))
        m = [[(matrix[i][j]+matrix[j][i])/2 for j in range(3)] for i in range(3)]
        one = [m[i][i] for i in range(3)]
        two = [m[i][i]*m[j][j]-m[i][j]*m[i][j]
               for i, j in ((0, 1), (0, 2), (1, 2))]
        determinant = (m[0][0]*(m[1][1]*m[2][2]-m[1][2]*m[2][1])
                       - m[0][1]*(m[1][0]*m[2][2]-m[1][2]*m[2][0])
                       + m[0][2]*(m[1][0]*m[2][1]-m[1][1]*m[2][0]))
        psd &= (all(value >= -tolerance for value in one) and
                all(value >= -tolerance*scale for value in two) and
                determinant >= -tolerance*scale*scale)
    return symmetry, bool(psd)


def summarize(pairs, recovery_threshold_m=10.0):
    """Summarize [(Row, estimate)] pairs; truth metrics omitted if unavailable."""
    if not pairs:
        raise ValueError("no navigation steps")
    result = {"steps": len(pairs)}
    durations = sorted(est["duration_ns"] for _, est in pairs)
    result["execution_ns_median"] = durations[len(durations)//2]
    result["execution_ns_p95"] = durations[min(len(durations)-1, int(.95*len(durations)))]
    result["execution_ns_max"] = durations[-1]
    result["max_dt_raw_s"] = max(est["dt_raw_s"] for _, est in pairs)
    result["clamped_steps"] = sum(est["dt_raw_s"] > est["dt_applied_s"]+1e-7
                                  for _, est in pairs)
    result["max_quaternion_norm_error"] = max(abs(norm(est["quaternion"])-1)
                                               for _, est in pairs)
    result["max_pre_normalization_norm_error"] = max(
        abs(est["quaternion_norm_before_normalization"]-1) for _, est in pairs)
    result["max_covariance_symmetry_error"] = 0.0
    result["covariance_psd"] = True
    result["all_finite"] = True
    for _, est in pairs:
        symmetry, psd = covariance_health(est["covariance"])
        result["max_covariance_symmetry_error"] = max(
            result["max_covariance_symmetry_error"], symmetry)
        result["covariance_psd"] &= psd
        vectors = (est["position_m"], est["velocity_mps"],
                   est["acceleration_mps2"], est["quaternion"],
                   *(row for axis in est["covariance"] for row in axis))
        result["all_finite"] &= all(math.isfinite(v) for vector in vectors for v in vector)
    truth_pairs = [(row, est) for row, est in pairs if row.truth is not None]
    if not truth_pairs:
        return result
    for kind in ("position_m", "velocity_mps", "acceleration_mps2"):
        errors = [norm(a-b for a, b in zip(est[kind], getattr(row.truth, kind)))
                  for row, est in truth_pairs]
        prefix = kind.split("_")[0]
        result[prefix + "_rmse"] = math.sqrt(statistics.fmean(e*e for e in errors))
        result[prefix + "_max_error"] = max(errors)
    angles = [orientation_error_deg(est["quaternion"], row.truth.quaternion)
              for row, est in truth_pairs]
    result["orientation_rmse_deg"] = math.sqrt(statistics.fmean(a*a for a in angles))
    result["orientation_max_error_deg"] = max(angles)
    coast = [(row, est) for row, est in truth_pairs if row.phase == "coast"]
    if coast:
        initial = tuple(a-b for a, b in zip(coast[0][1]["position_m"],
                                             coast[0][0].truth.position_m))
        result["coast_drift_m"] = max(norm((a-b)-base for a, b, base in
                                         zip(est["position_m"], row.truth.position_m, initial))
                                       for row, est in coast)
    # First time after an all-accel/baro outage that position error recovers.
    outage_end = None
    for row, _ in truth_pairs:
        if not row.sample.low_g_valid and not row.sample.high_g_valid and not row.sample.baro_valid:
            outage_end = row.timestamp_us
    if outage_end is not None:
        recovered = next((row.timestamp_us for row, est in truth_pairs
                          if row.timestamp_us > outage_end and norm(
                              a-b for a, b in zip(est["position_m"],
                                                  row.truth.position_m)) <= recovery_threshold_m), None)
        result["outage_recovery_s"] = (None if recovered is None else
                                       (recovered-outage_end)*1e-6)
    transitions = []
    for (_, previous), (_, current) in zip(pairs, pairs[1:]):
        if (previous["low_g_variance_scale"] < 1e6 <= current["low_g_variance_scale"] or
                previous["low_g_variance_scale"] >= 1e6 > current["low_g_variance_scale"]):
            transitions.append(norm(a-b for a, b in zip(
                current["acceleration_mps2"], previous["acceleration_mps2"])))
    if transitions:
        result["max_low_g_transition_accel_jump_mps2"] = max(transitions)
    return result
