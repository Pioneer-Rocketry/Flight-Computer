#ifndef SUBSYSTEMS_NAVIGATION_ESTIMATOR_H_
#define SUBSYSTEMS_NAVIGATION_ESTIMATOR_H_

#include <cstdint>
#include "matrix.hpp"

// Engineering units after the firmware sensor drivers. Quaternion rotates body
// vectors into a local navigation frame with +Z up. Gyro axes are body XYZ.
struct NavigationSample {
    float low_g[3];                 // specific force, m/s^2
    float high_g[3];                // specific force, m/s^2
    float gyro_dps[3];              // degrees/s
    float baro_altitude_m;          // same absolute datum as the baro driver
    float baro_pressure_hpa;        // used for the legacy positive-pressure gate
    uint8_t low_g_valid;
    uint8_t high_g_valid;
    uint8_t gyro_valid;
    uint8_t baro_valid;
};

struct NavigationResult {
    float position_m[3];
    float velocity_mps[3];
    float acceleration_mps2[3];
    float quaternion[4];           // [w,x,y,z], body to navigation
    float euler_deg[3];            // legacy [roll,pitch,yaw] convention
    float quaternion_norm_before_normalization;
    float dt_raw_s;
    float dt_applied_s;
    float dt_filter_s;
};

struct NavigationMeasurementDiagnostic {
    float innovation;
    float innovation_variance;
    float measurement_variance;
    uint8_t applied;
};

struct NavigationDiagnostics {
    float covariance[3][3][3];    // axis, row, column
    NavigationMeasurementDiagnostic measurements[7]; // low XYZ, high XYZ, baro Z
    float low_g_variance_scale;
    uint8_t gyro_used;
};

class NavigationEstimator {
public:
    NavigationEstimator();
    void reset(const NavigationSample& initial);
    // Returns false on a zero elapsed interval. No sensor or state change then.
    bool step(const NavigationSample& sample, uint32_t elapsed_us,
              NavigationDiagnostics* diagnostics = nullptr);
    const NavigationResult& result() const { return output; }
    void diagnostics(NavigationDiagnostics& out) const;

private:
    enum StateIndex { POSITION = 0, VELOCITY = 1, ACCELERATION = 2 };
    Matrix<float, 3, 1> axis_state[3];
    Matrix<float, 3, 3> covariance[3];
    Matrix<float, 3, 3> F, Q, I;
    NavigationResult output{};
    NavigationMeasurementDiagnostic last_measurements[7]{};
    float last_low_g_scale = 1.0f;
    uint8_t last_gyro_used = 0;
    bool filter_initialized = false;

    void initialize_quaternion(const NavigationSample& sample);
    void integrate_quaternion(const NavigationSample& sample, float dt_s);
    void initialize_filter(const Matrix<float, 3, 1>& low_g,
                           const Matrix<float, 3, 1>& high_g,
                           const NavigationSample& sample);
    void update_matrices(float dt_s);
    void scalar_update(int axis, StateIndex state, float value, float variance,
                       int diagnostic_index);
    static void rotate_by_quaternion(Matrix<float, 3, 1>& vector,
                                     const float quaternion[4]);
    static bool vector_finite(const float vector[3]);
    static float low_g_variance_scale(const NavigationSample& sample);
};

#endif
