#include "nav_sitl.h"
#include "Subsystems/NavigationEstimator.h"
#include <cstring>
#include <new>
#include <limits>

struct NavSitlHandle {
    NavigationEstimator estimator;
    uint64_t last_timestamp_us;
};

namespace {
NavigationSample to_native(const NavSitlSample& s)
{
    NavigationSample out{};
    for (int i = 0; i < 3; ++i) {
        out.low_g[i] = s.low_g[i];
        out.high_g[i] = s.high_g[i];
        out.gyro_dps[i] = s.gyro_dps[i];
    }
    out.baro_altitude_m = s.baro_altitude_m;
    out.baro_pressure_hpa = s.baro_pressure_hpa;
    out.low_g_valid = s.low_g_valid;
    out.high_g_valid = s.high_g_valid;
    out.gyro_valid = s.gyro_valid;
    out.baro_valid = s.baro_valid;
    return out;
}

void copy_result(const NavigationResult& in, NavSitlResult& out)
{
    out = NavSitlResult{};
    out.struct_size = sizeof(out);
    for (int i = 0; i < 3; ++i) {
        out.position_m[i] = in.position_m[i];
        out.velocity_mps[i] = in.velocity_mps[i];
        out.acceleration_mps2[i] = in.acceleration_mps2[i];
        out.euler_deg[i] = in.euler_deg[i];
    }
    for (int i = 0; i < 4; ++i) out.quaternion[i] = in.quaternion[i];
    out.quaternion_norm_before_normalization = in.quaternion_norm_before_normalization;
    out.dt_raw_s = in.dt_raw_s;
    out.dt_applied_s = in.dt_applied_s;
    out.dt_filter_s = in.dt_filter_s;
}

void copy_diagnostics(const NavigationDiagnostics& in, NavSitlDiagnostics& out)
{
    out = NavSitlDiagnostics{};
    out.struct_size = sizeof(out);
    for (int axis = 0; axis < 3; ++axis)
        for (int row = 0; row < 3; ++row)
            for (int col = 0; col < 3; ++col)
                out.covariance[axis][row][col] = in.covariance[axis][row][col];
    for (int i = 0; i < 7; ++i) {
        out.measurements[i].innovation = in.measurements[i].innovation;
        out.measurements[i].innovation_variance = in.measurements[i].innovation_variance;
        out.measurements[i].measurement_variance = in.measurements[i].measurement_variance;
        out.measurements[i].applied = in.measurements[i].applied;
    }
    out.low_g_variance_scale = in.low_g_variance_scale;
    out.gyro_used = in.gyro_used;
}
}

extern "C" uint32_t nav_sitl_api_version(void) { return NAV_SITL_ABI_VERSION; }
extern "C" uint32_t nav_sitl_sample_size(void) { return sizeof(NavSitlSample); }
extern "C" uint32_t nav_sitl_result_size(void) { return sizeof(NavSitlResult); }
extern "C" uint32_t nav_sitl_diagnostics_size(void) { return sizeof(NavSitlDiagnostics); }

extern "C" int nav_sitl_create(const NavSitlSample* initial,
                               uint64_t timestamp_us, NavSitlHandle** out)
{
    if (!out || !initial || initial->struct_size != sizeof(NavSitlSample)) return -1;
    *out = nullptr;
    NavSitlHandle* handle = new (std::nothrow) NavSitlHandle{};
    if (!handle) return -2;
    handle->last_timestamp_us = timestamp_us;
    handle->estimator.reset(to_native(*initial));
    *out = handle;
    return 0;
}

extern "C" int nav_sitl_step(NavSitlHandle* handle, const NavSitlSample* sample,
                             uint64_t timestamp_us, NavSitlResult* result,
                             NavSitlDiagnostics* diagnostics)
{
    if (!handle || !sample || !result || sample->struct_size != sizeof(NavSitlSample) ||
        result->struct_size != sizeof(NavSitlResult) ||
        (diagnostics && diagnostics->struct_size != sizeof(NavSitlDiagnostics))) return -1;
    if (timestamp_us < handle->last_timestamp_us) return -3;
    const uint64_t elapsed = timestamp_us - handle->last_timestamp_us;
    if (elapsed > std::numeric_limits<uint32_t>::max()) return -4;
    if (elapsed == 0) {
        copy_result(handle->estimator.result(), *result);
        if (diagnostics) {
            NavigationDiagnostics d{};
            handle->estimator.diagnostics(d);
            copy_diagnostics(d, *diagnostics);
        }
        return 1;
    }
    NavigationDiagnostics d{};
    handle->estimator.step(to_native(*sample), static_cast<uint32_t>(elapsed),
                           diagnostics ? &d : nullptr);
    handle->last_timestamp_us = timestamp_us;
    copy_result(handle->estimator.result(), *result);
    if (diagnostics) copy_diagnostics(d, *diagnostics);
    return 0;
}

extern "C" void nav_sitl_destroy(NavSitlHandle* handle) { delete handle; }
