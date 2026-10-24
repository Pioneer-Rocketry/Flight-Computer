#ifndef NAV_SITL_H_
#define NAV_SITL_H_
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NAV_SITL_ABI_VERSION 1u

typedef struct NavSitlHandle NavSitlHandle;

typedef struct {
    uint32_t struct_size;
    float low_g[3];
    float high_g[3];
    float gyro_dps[3];
    float baro_altitude_m;
    float baro_pressure_hpa;
    uint8_t low_g_valid;
    uint8_t high_g_valid;
    uint8_t gyro_valid;
    uint8_t baro_valid;
} NavSitlSample;

typedef struct {
    uint32_t struct_size;
    float position_m[3];
    float velocity_mps[3];
    float acceleration_mps2[3];
    float quaternion[4];
    float euler_deg[3];
    float quaternion_norm_before_normalization;
    float dt_raw_s;
    float dt_applied_s;
    float dt_filter_s;
} NavSitlResult;

typedef struct {
    float innovation;
    float innovation_variance;
    float measurement_variance;
    uint8_t applied;
} NavSitlMeasurement;

typedef struct {
    uint32_t struct_size;
    float covariance[3][3][3];
    NavSitlMeasurement measurements[7]; // low XYZ, high XYZ, baro Z
    float low_g_variance_scale;
    uint8_t gyro_used;
} NavSitlDiagnostics;

// Return codes: 0 stepped, 1 duplicate timestamp (unchanged state),
// -1 invalid argument/ABI, -2 allocation failure, -3 backward timestamp,
// -4 interval exceeds uint32 microseconds.
uint32_t nav_sitl_api_version(void);
uint32_t nav_sitl_sample_size(void);
uint32_t nav_sitl_result_size(void);
uint32_t nav_sitl_diagnostics_size(void);
int nav_sitl_create(const NavSitlSample* initial, uint64_t timestamp_us,
                    NavSitlHandle** out);
int nav_sitl_step(NavSitlHandle* handle, const NavSitlSample* sample,
                  uint64_t timestamp_us, NavSitlResult* result,
                  NavSitlDiagnostics* diagnostics);
void nav_sitl_destroy(NavSitlHandle* handle);

#ifdef __cplusplus
}
#endif
#endif
