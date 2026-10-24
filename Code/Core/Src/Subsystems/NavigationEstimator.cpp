#include "Subsystems/NavigationEstimator.h"
#include "defines.h"
#include <cmath>

namespace {
constexpr float kMaxDt = 0.05f;
constexpr float kLowGFullScale = 16.0f * GRAVITY;
constexpr float kSaturatedScale = 1.0e6f;
constexpr float kPi = 3.14159265358979323846f;
constexpr float kRadiansPerDegree = kPi / 180.0f;
constexpr float kDegreesPerRadian = 180.0f / kPi;
constexpr float kGyroThreshold = 0.1f; // legacy value is applied in rad/s
constexpr float kProcessNoise = 0.01f;
constexpr float kLowGNoise = 0.5f;
constexpr float kHighGNoise = 2.0f;
constexpr float kBaroNoise = 3.0f;

float soft_threshold(float value, float threshold)
{
    return std::fabs(value) < threshold
        ? value * (std::fabs(value) / threshold) : value;
}
}

NavigationEstimator::NavigationEstimator()
{
    NavigationSample initial{};
    reset(initial);
}

bool NavigationEstimator::vector_finite(const float vector[3])
{
    return std::isfinite(vector[0]) && std::isfinite(vector[1]) &&
           std::isfinite(vector[2]);
}

void NavigationEstimator::reset(const NavigationSample& initial)
{
    output = NavigationResult{};
    for (int axis = 0; axis < 3; ++axis) {
        axis_state[axis].setZero();
        covariance[axis].setZero();
    }
    for (auto& measurement : last_measurements)
        measurement = NavigationMeasurementDiagnostic{};
    last_low_g_scale = 1.0f;
    last_gyro_used = 0;
    filter_initialized = false;
    initialize_quaternion(initial);
}

void NavigationEstimator::initialize_quaternion(const NavigationSample& sample)
{
    float* q = output.quaternion;
    q[0] = 1.0f;
    output.quaternion_norm_before_normalization = 1.0f;
    if (!sample.low_g_valid || !vector_finite(sample.low_g))
        return;
    const float norm = std::sqrt(sample.low_g[0] * sample.low_g[0] +
                                 sample.low_g[1] * sample.low_g[1] +
                                 sample.low_g[2] * sample.low_g[2]);
    if (!(norm >= 1.0e-6f) || !std::isfinite(norm))
        return;
    const float ax = sample.low_g[0] / norm;
    const float ay = sample.low_g[1] / norm;
    const float az = sample.low_g[2] / norm;
    if (az > -0.9999f) {
        q[0] = std::sqrt(0.5f * (1.0f + az));
        const float scale = 0.5f / q[0];
        q[1] = ay * scale;
        q[2] = -ax * scale;
    } else {
        q[0] = 0.0f;
        q[1] = 1.0f;
    }
}

void NavigationEstimator::rotate_by_quaternion(Matrix<float, 3, 1>& v,
                                                const float q[4])
{
    const float vx = v(0), vy = v(1), vz = v(2);
    const float cx = q[2] * vz - q[3] * vy + q[0] * vx;
    const float cy = q[3] * vx - q[1] * vz + q[0] * vy;
    const float cz = q[1] * vy - q[2] * vx + q[0] * vz;
    v(0) = vx + 2.0f * (q[2] * cz - q[3] * cy);
    v(1) = vy + 2.0f * (q[3] * cx - q[1] * cz);
    v(2) = vz + 2.0f * (q[1] * cy - q[2] * cx);
}

void NavigationEstimator::integrate_quaternion(const NavigationSample& sample,
                                                float dt_s)
{
    float* q = output.quaternion;
    last_gyro_used = sample.gyro_valid && vector_finite(sample.gyro_dps);
    if (last_gyro_used) {
        // Legacy mapping: gyro X is pitch, gyro Y is roll, gyro Z is yaw.
        const float pitch = soft_threshold(sample.gyro_dps[0] * kRadiansPerDegree,
                                           kGyroThreshold);
        const float roll = soft_threshold(sample.gyro_dps[1] * kRadiansPerDegree,
                                          kGyroThreshold);
        const float yaw = soft_threshold(sample.gyro_dps[2] * kRadiansPerDegree,
                                         kGyroThreshold);
        const float dw = 0.5f * (-pitch*q[1] - roll*q[2] - yaw*q[3]);
        const float dx = 0.5f * ( pitch*q[0] + yaw*q[2] - roll*q[3]);
        const float dy = 0.5f * ( roll*q[0] - yaw*q[1] + pitch*q[3]);
        const float dz = 0.5f * ( yaw*q[0] + roll*q[1] - pitch*q[2]);
        q[0] += dw * dt_s;
        q[1] += dx * dt_s;
        q[2] += dy * dt_s;
        q[3] += dz * dt_s;
    }
    const float norm = std::sqrt(q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]);
    output.quaternion_norm_before_normalization = norm;
    if (norm > 1.0e-6f && std::isfinite(norm)) {
        for (float& component : output.quaternion)
            component /= norm;
    } else {
        q[0] = 1.0f; q[1] = 0.0f; q[2] = 0.0f; q[3] = 0.0f;
    }

    const float siny = 2.0f * (q[0]*q[2] - q[3]*q[1]);
    const float cosy = 1.0f - 2.0f * (q[1]*q[1] + q[2]*q[2]);
    output.euler_deg[0] = std::atan2(siny, cosy) * kDegreesPerRadian;
    const float sinp = 2.0f * (q[0]*q[1] + q[2]*q[3]);
    output.euler_deg[1] = (std::fabs(sinp) >= 1.0f
        ? std::copysign(kPi/2.0f, sinp) : std::asin(sinp)) * kDegreesPerRadian;
    const float sinr = 2.0f * (q[0]*q[3] + q[1]*q[2]);
    const float cosr = 1.0f - 2.0f * (q[2]*q[2] + q[3]*q[3]);
    output.euler_deg[2] = std::atan2(sinr, cosr) * kDegreesPerRadian;
}

float NavigationEstimator::low_g_variance_scale(const NavigationSample& sample)
{
    if (!sample.low_g_valid || !vector_finite(sample.low_g))
        return 1.0f;
    const float largest = std::fmax(std::fabs(sample.low_g[0]),
        std::fmax(std::fabs(sample.low_g[1]), std::fabs(sample.low_g[2])));
    const float fraction = largest / kLowGFullScale;
    if (fraction <= 0.80f) return 1.0f;
    if (fraction >= 0.97f) return kSaturatedScale;
    const float transition = (fraction - 0.80f) / (0.97f - 0.80f);
    return 1.0f + 99.0f * transition * transition;
}

void NavigationEstimator::initialize_filter(const Matrix<float, 3, 1>& low_g,
                                              const Matrix<float, 3, 1>& high_g,
                                              const NavigationSample& sample)
{
    const bool low_valid = sample.low_g_valid && vector_finite(sample.low_g);
    const bool high_valid = sample.high_g_valid && vector_finite(sample.high_g);
    for (int axis = 0; axis < 3; ++axis) {
        axis_state[axis].setZero();
        axis_state[axis](ACCELERATION) = low_valid ? low_g(axis)
            : (high_valid ? high_g(axis) : 0.0f);
        covariance[axis].setIdentity();
    }
    axis_state[2](POSITION) = sample.baro_valid &&
        std::isfinite(sample.baro_altitude_m) ? sample.baro_altitude_m : 0.0f;
    I.setIdentity();
    filter_initialized = true;
}

void NavigationEstimator::update_matrices(float dt_s)
{
    F.setIdentity();
    F(0,1) = dt_s;
    F(0,2) = dt_s * dt_s / 2.0f;
    F(1,2) = dt_s;
    const float dt2 = dt_s*dt_s;
    const float dt3 = dt2*dt_s;
    const float dt4 = dt3*dt_s;
    Q.setZero();
    Q(0,0) = dt4/4.0f;
    Q(0,1) = dt3/2.0f; Q(1,0) = Q(0,1);
    Q(0,2) = dt2/2.0f; Q(2,0) = Q(0,2);
    Q(1,1) = dt2;
    Q(1,2) = dt_s; Q(2,1) = Q(1,2);
    Q(2,2) = 1.0f;
    Q *= kProcessNoise;
}

void NavigationEstimator::scalar_update(int axis, StateIndex state, float value,
                                         float variance, int index)
{
    NavigationMeasurementDiagnostic& d = last_measurements[index];
    d.measurement_variance = variance;
    if (!std::isfinite(value) || !std::isfinite(variance) || variance <= 0.0f)
        return;
    const float S = covariance[axis](state, state) + variance;
    if (!std::isfinite(S) || S <= 1.0e-9f)
        return;
    d.innovation = value - axis_state[axis](state);
    d.innovation_variance = S;
    Matrix<float, 3, 1> gain;
    for (int row = 0; row < 3; ++row)
        gain(row) = covariance[axis](row, state) / S;
    for (int row = 0; row < 3; ++row)
        axis_state[axis](row) += gain(row) * d.innovation;
    Matrix<float, 3, 3> correction = I;
    for (int row = 0; row < 3; ++row)
        correction(row, state) -= gain(row);
    covariance[axis] = correction * covariance[axis] * correction.transpose();
    for (int row = 0; row < 3; ++row)
        for (int column = 0; column < 3; ++column)
            covariance[axis](row, column) += gain(row) * variance * gain(column);
    covariance[axis] = covariance[axis] + covariance[axis].transpose();
    covariance[axis] *= 0.5f;
    d.applied = 1;
}

bool NavigationEstimator::step(const NavigationSample& sample, uint32_t elapsed_us,
                               NavigationDiagnostics* diag)
{
    if (elapsed_us == 0U)
        return false;
    for (auto& measurement : last_measurements)
        measurement = NavigationMeasurementDiagnostic{};
    output.dt_raw_s = static_cast<float>(elapsed_us) * 1.0e-6f;
    output.dt_applied_s = output.dt_raw_s > kMaxDt ? kMaxDt : output.dt_raw_s;
    integrate_quaternion(sample, output.dt_applied_s);

    Matrix<float, 3, 1> low_g, high_g;
    const bool low_valid = sample.low_g_valid && vector_finite(sample.low_g);
    const bool high_valid = sample.high_g_valid && vector_finite(sample.high_g);
    if (low_valid)
        for (int axis = 0; axis < 3; ++axis) low_g(axis) = sample.low_g[axis];
    if (high_valid)
        for (int axis = 0; axis < 3; ++axis) high_g(axis) = sample.high_g[axis];
    rotate_by_quaternion(low_g, output.quaternion);
    rotate_by_quaternion(high_g, output.quaternion);
    low_g(2) -= GRAVITY;
    high_g(2) -= GRAVITY;

    const bool first_step = !filter_initialized;
    if (first_step)
        initialize_filter(low_g, high_g, sample);
    // Preserve the existing first-filter-step target dt, even though the
    // quaternion has already used the measured elapsed time.
    output.dt_filter_s = first_step ? NAVIGATION_TARGET_DT : output.dt_applied_s;
    update_matrices(output.dt_filter_s);
    last_low_g_scale = low_g_variance_scale(sample);
    const float low_variance = kLowGNoise*kLowGNoise*last_low_g_scale;
    const float high_variance = kHighGNoise*kHighGNoise;
    for (int axis = 0; axis < 3; ++axis) {
        axis_state[axis] = F * axis_state[axis];
        covariance[axis] = F * covariance[axis] * F.transpose() + Q;
        if (low_valid)
            scalar_update(axis, ACCELERATION, low_g(axis), low_variance, axis);
        if (high_valid)
            scalar_update(axis, ACCELERATION, high_g(axis), high_variance, axis + 3);
    }
    if (sample.baro_valid && sample.baro_pressure_hpa > 0.0f)
        scalar_update(2, POSITION, sample.baro_altitude_m,
                      kBaroNoise*kBaroNoise, 6);
    for (int axis = 0; axis < 3; ++axis) {
        output.position_m[axis] = axis_state[axis](POSITION);
        output.velocity_mps[axis] = axis_state[axis](VELOCITY);
        output.acceleration_mps2[axis] = axis_state[axis](ACCELERATION);
    }
    if (diag) diagnostics(*diag);
    return true;
}

void NavigationEstimator::diagnostics(NavigationDiagnostics& out) const
{
    out = NavigationDiagnostics{};
    for (int axis = 0; axis < 3; ++axis)
        for (int row = 0; row < 3; ++row)
            for (int column = 0; column < 3; ++column)
                out.covariance[axis][row][column] = covariance[axis](row, column);
    for (int i = 0; i < 7; ++i) out.measurements[i] = last_measurements[i];
    out.low_g_variance_scale = last_low_g_scale;
    out.gyro_used = last_gyro_used;
}
