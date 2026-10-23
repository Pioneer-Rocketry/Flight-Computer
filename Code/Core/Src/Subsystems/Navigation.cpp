/*
 * Navigation.cpp
 *
 *  Created on: Oct 27, 2025
 *      Author: colin
 */

#include "Subsystems/Navigation.h"

namespace {
constexpr float MAX_NAVIGATION_DT_S = 0.05f;
constexpr float LOW_G_FULL_SCALE_MPS2 = 16.0f * GRAVITY;
constexpr float LOW_G_DERATE_START = 0.80f;
constexpr float LOW_G_REJECT_START = 0.97f;
constexpr float SATURATED_VARIANCE_SCALE = 1.0e6f;
}

float softThreshold(float value, float threshold)
{
    if (fabsf(value) < threshold)
        return value * (fabsf(value) / threshold); // Linear taper
    return value;
}

Navigation::Navigation(DataContainer* data, SPI_HandleTypeDef* spiBus, UART_HandleTypeDef* uart, uint8_t* gpsRxBuffer)
	: Subsystem(data),
	  imu(data, spiBus, IMU_CS_GPIO_Port, IMU_CS_Pin),
	  baro(data, spiBus, BARO_CS_GPIO_Port, BARO_CS_Pin),
	  gps(data, uart, gpsRxBuffer)
{
	data->KalmanFilterPositionX_m = 0.0f;
	data->KalmanFilterPositionY_m = 0.0f;
	data->KalmanFilterPositionZ_m = 0.0f;

	data->KalmanFilterVelocityX_mps = 0.0f;
	data->KalmanFilterVelocityY_mps = 0.0f;
	data->KalmanFilterVelocityZ_mps = 0.0f;

	data->KalmanFilterAccelerationX_mps2 = 0.0f;
	data->KalmanFilterAccelerationY_mps2 = 0.0f;
	data->KalmanFilterAccelerationZ_mps2 = 0.0f;

	lastLoop_us = micros();
}


int Navigation::init()
{

	if (imu.init() < 0)
	{
		return -1;
	}

	if (baro.init() < 0)
	{
		return -1;
	}

	if (gps.init() < 0)
	{
		return -1;
	}

	initializeQuaternion();

	baro.startConversion();

	return 0;
}


int Navigation::update()
{
	now_us = micros();
	// Unsigned subtraction handles the micros() counter wrapping around.
	const uint32_t elapsed_us = now_us - lastLoop_us;
	lastLoop_us = now_us;
	if (elapsed_us == 0U)
		return 0;

	dt_s = static_cast<float>(elapsed_us) * 1.0e-6f;
	if (dt_s > MAX_NAVIGATION_DT_S)
		dt_s = MAX_NAVIGATION_DT_S;
	freq = 1.0f / dt_s;

	dt2 = dt_s * dt_s;
	dt3 = dt2  * dt_s;
	dt4 = dt3  * dt_s;

	// -------------------------------------------------------------
	// Read Sensor Data
	// -------------------------------------------------------------

	imu.update();

	lowG(0) = data->LSM6DSV320LowGAccelX_mps2;
	lowG(1) = data->LSM6DSV320LowGAccelY_mps2;
	lowG(2) = data->LSM6DSV320LowGAccelZ_mps2;

	highG(0) = data->LSM6DSV320HighGAccelX_mps2;
	highG(1) = data->LSM6DSV320HighGAccelY_mps2;
	highG(2) = data->LSM6DSV320HighGAccelZ_mps2;

	// -------------------------------------------------------------
	// Quaterion Intergration
	// -------------------------------------------------------------
	rollRate_rad	= data->LSM6DSV320GyroY_dps * DEG_TO_RAD;
	pitchRate_rad 	= data->LSM6DSV320GyroX_dps * DEG_TO_RAD;
	yawRate_rad 	= data->LSM6DSV320GyroZ_dps * DEG_TO_RAD;

	// integrate quaternion
	integrateQuaternion();

	// Rotate Accelerometer data by quaterion to get it to earth reference frame
	rotateVectorByQuaternion(lowG);
    rotateVectorByQuaternion(highG);

	// Accelerometers measure specific force. This navigation frame uses +Z up.
	lowG(2) -= GRAVITY;
	highG(2) -= GRAVITY;

	baro.update();
	gps.update();

	// -------------------------------------------------------------
	// Kalman Filter
	// -------------------------------------------------------------

	if (!isKalmanFilterInit)
		initKalmanFilter();

	updateKalmanFilter();
	runKalmanFilter();

	data->KalmanFilterPositionX_m			= axisState[0](POSITION);
	data->KalmanFilterVelocityX_mps			= axisState[0](VELOCITY);
	data->KalmanFilterAccelerationX_mps2	= axisState[0](ACCELERATION);

	data->KalmanFilterPositionY_m			= axisState[1](POSITION);
	data->KalmanFilterVelocityY_mps			= axisState[1](VELOCITY);
	data->KalmanFilterAccelerationY_mps2	= axisState[1](ACCELERATION);

	data->KalmanFilterPositionZ_m			= axisState[2](POSITION);
	data->KalmanFilterVelocityZ_mps			= axisState[2](VELOCITY);
	data->KalmanFilterAccelerationZ_mps2	= axisState[2](ACCELERATION);

	baro.startConversion();

	return 0;
}

void Navigation::rotateVectorByQuaternion(Matrix<float, 3, 1>& vec)
{
    // Rotate vector from body frame to earth frame using quaternion
    // Formula: v' = q * v * q^(-1)
    // For unit quaternions: q^(-1) = q* (conjugate)

    float qw = data->quaternionW;
    float qx = data->quaternionX;
    float qy = data->quaternionY;
    float qz = data->quaternionZ;

    float vx = vec(0);
    float vy = vec(1);
    float vz = vec(2);

    // Optimized rotation: v' = v + 2*r x (r x v + w*v)
    // where r = [qx, qy, qz] and w = qw

    // First cross product: r x v
    float cx = qy * vz - qz * vy;
    float cy = qz * vx - qx * vz;
    float cz = qx * vy - qy * vx;

    // Add w*v
    cx += qw * vx;
    cy += qw * vy;
    cz += qw * vz;

    // Second cross product: r x (r x v + w*v)
    float rx = qy * cz - qz * cy;
    float ry = qz * cx - qx * cz;
    float rz = qx * cy - qy * cx;

    // Final result: v + 2 * (r x (r x v + w*v))
    vec(0) = vx + 2.0f * rx;
    vec(1) = vy + 2.0f * ry;
    vec(2) = vz + 2.0f * rz;
}

void Navigation::integrateQuaternion()
{
	pitchRate_rad = softThreshold(pitchRate_rad, GYRO_THRESHOLD_DPS);
	rollRate_rad  = softThreshold(rollRate_rad,  GYRO_THRESHOLD_DPS);
	yawRate_rad   = softThreshold(yawRate_rad,   GYRO_THRESHOLD_DPS);

	quaternionWDot = 0.5f * (-pitchRate_rad * data->quaternionX - rollRate_rad * data->quaternionY - yawRate_rad   * data->quaternionZ);
	quaternionXDot = 0.5f * ( pitchRate_rad * data->quaternionW + yawRate_rad  * data->quaternionY - rollRate_rad  * data->quaternionZ);
	quaternionYDot = 0.5f * ( rollRate_rad 	* data->quaternionW - yawRate_rad  * data->quaternionX + pitchRate_rad * data->quaternionZ);
	quaternionZDot = 0.5f * ( yawRate_rad 	* data->quaternionW + rollRate_rad * data->quaternionX - pitchRate_rad * data->quaternionY);

	data->quaternionW += quaternionWDot * dt_s;
	data->quaternionX += quaternionXDot * dt_s;
	data->quaternionY += quaternionYDot * dt_s;
	data->quaternionZ += quaternionZDot * dt_s;

	data->quaternionNorm = sqrtf( data->quaternionW * data->quaternionW +
								  data->quaternionX * data->quaternionX +
								  data->quaternionY * data->quaternionY +
								  data->quaternionZ * data->quaternionZ);

	if (data->quaternionNorm > 1e-6f)
	{
		data->quaternionW /= data->quaternionNorm;
		data->quaternionX /= data->quaternionNorm;
		data->quaternionY /= data->quaternionNorm;
		data->quaternionZ /= data->quaternionNorm;
	} else {
		data->quaternionW = 1.0f;
		data->quaternionX = 0.0f;
		data->quaternionY = 0.0f;
		data->quaternionZ = 0.0f;
	}

	siny_cosp = 2.0f * (data->quaternionW * data->quaternionY - data->quaternionZ * data->quaternionX);
    cosy_cosp = 1.0f - 2.0f * (data->quaternionX * data->quaternionX + data->quaternionY * data->quaternionY);
    data->roll = atan2f(siny_cosp, cosy_cosp) * RAD_TO_DEG;

    // Pitch (rotation about X-axis)
    sinp = 2.0f * (data->quaternionW * data->quaternionX + data->quaternionY * data->quaternionZ);
    if (fabsf(sinp) >= 1.0f)
        data->pitch = copysignf(M_PI / 2.0f, sinp) * RAD_TO_DEG; // Use 90 degrees if out of range
    else
        data->pitch = asinf(sinp) * RAD_TO_DEG;

    // Yaw (rotation about Z-axis)
    sinr_cosp = 2.0f * (data->quaternionW * data->quaternionZ + data->quaternionX * data->quaternionY);
    cosr_cosp = 1.0f - 2.0f * (data->quaternionY * data->quaternionY + data->quaternionZ * data->quaternionZ);
    data->yaw = atan2f(sinr_cosp, cosr_cosp) * RAD_TO_DEG;
}

void Navigation::initializeQuaternion()
{
	float norm = sqrtf( data->LSM6DSV320LowGAccelX_mps2 * data->LSM6DSV320LowGAccelX_mps2 +
						data->LSM6DSV320LowGAccelY_mps2 * data->LSM6DSV320LowGAccelY_mps2 +
						data->LSM6DSV320LowGAccelZ_mps2 * data->LSM6DSV320LowGAccelZ_mps2);

	if (norm < 1e-6f)
	{
		data->quaternionW = 1.0f;
		data->quaternionX = 0.0f;
		data->quaternionY = 0.0f;
		data->quaternionZ = 0.0f;
		data->quaternionNorm = 1.0f;
		data->roll = 0.0f;
		data->pitch = 0.0f;
		data->yaw = 0.0f;
		return;
	}

	const float ax = data->LSM6DSV320LowGAccelX_mps2 / norm;
	const float ay = data->LSM6DSV320LowGAccelY_mps2 / norm;
	const float az = data->LSM6DSV320LowGAccelZ_mps2 / norm;

	// Shortest rotation from measured specific force to navigation-frame +Z.
	// Rotation about gravity remains unobservable and is initialized to zero.
	if (az > -0.9999f)
	{
		data->quaternionW = sqrtf(0.5f * (1.0f + az));
		const float scale = 0.5f / data->quaternionW;
		data->quaternionX = ay * scale;
		data->quaternionY = -ax * scale;
		data->quaternionZ = 0.0f;
	}
	else
	{
		// Anti-parallel vectors require an arbitrary 180-degree transverse axis.
		data->quaternionW = 0.0f;
		data->quaternionX = 1.0f;
		data->quaternionY = 0.0f;
		data->quaternionZ = 0.0f;
	}
	data->quaternionNorm = 1.0f;
	data->roll = 0.0f;
	data->pitch = 0.0f;
	data->yaw = 0.0f;
}

float Navigation::lowGVarianceScale() const
{
	// Rotation mixes body axes, so clipping on any raw axis invalidates the
	// complete rotated low-g vector rather than only one navigation axis.
	const float largestBodyAxis = fmaxf(
			fabsf(data->LSM6DSV320LowGAccelX_mps2),
			fmaxf(fabsf(data->LSM6DSV320LowGAccelY_mps2),
				   fabsf(data->LSM6DSV320LowGAccelZ_mps2)));
	const float fraction = largestBodyAxis / LOW_G_FULL_SCALE_MPS2;
	if (fraction <= LOW_G_DERATE_START)
		return 1.0f;
	if (fraction >= LOW_G_REJECT_START)
		return SATURATED_VARIANCE_SCALE;

	const float transition = (fraction - LOW_G_DERATE_START) /
						 (LOW_G_REJECT_START - LOW_G_DERATE_START);
	return 1.0f + 99.0f * transition * transition;
}

void Navigation::initKalmanFilter()
{
	dt_s = NAVIGATION_TARGET_DT;
	dt2  = dt_s * dt_s;
	dt3  = dt2  * dt_s;
	dt4  = dt3  * dt_s;

	// Initialize Kalman Filter Matricies

	// Initial axis states and covariances.
	for (int axis = 0; axis < AXIS_COUNT; ++axis)
	{
		axisState[axis].setZero();
		axisState[axis](ACCELERATION) = lowG(axis);
		axisCovariance[axis].setIdentity();
	}
	axisState[2](POSITION) = data->MS560702BA03Altitude_m;

	// State Transition
	F.setIdentity();
    // For each axis block:
    // pos' = pos + vel*dt + 0.5*acc*dt^2
    // vel' = vel + acc*dt
    // acc' = acc

	// Shared axis transition matrix.
	F(0,1) = dt_s;
	F(0,2) = (dt_s * dt_s) / 2.0f;
	F(1,2) = dt_s;

	// Process Noise
	Q.setZero();
	Q(0,0) = dt4 / 4.0f;
	Q(0,1) = dt3 / 2.0f; Q(1,0) = Q(0,1);
	Q(0,2) = dt2 / 2.0f; Q(2,0) = Q(0,2);
	Q(1,1) = dt2;
	Q(1,2) = dt_s; Q(2,1) = Q(1,2);
	Q(2,2) = 1.0f; // keep small baseline

	Q *= processNoise;

	I.setIdentity();

	isKalmanFilterInit = true;

	return;
}

void Navigation::updateKalmanFilter()
{
	// Update State Transition Matrix
	F(0,1) = dt_s;
	F(0,2) = (dt_s * dt_s) / 2.0f;
	F(1,2) = dt_s;

	// Update Process Noise
	Q.setZero();
	Q(0,0) = dt4 / 4.0f;
	Q(0,1) = dt3 / 2.0f; Q(1,0) = Q(0,1);
	Q(0,2) = dt2 / 2.0f; Q(2,0) = Q(0,2);
	Q(1,1) = dt2;
	Q(1,2) = dt_s; Q(2,1) = Q(1,2);
	Q(2,2) = 1.0f; // keep small baseline

	Q *= processNoise;

	return;
}

void Navigation::runKalmanFilter()
{
	const float lowGVariance = lowGNoise * lowGNoise;
	const float highGVariance = highGNoise * highGNoise;
	const float lowVariance = lowGVariance * lowGVarianceScale();

	for (int axis = 0; axis < AXIS_COUNT; ++axis)
	{
		axisState[axis] = F * axisState[axis];
		axisCovariance[axis] = F * axisCovariance[axis] * F.transpose() + Q;

		scalarMeasurementUpdate(axis, ACCELERATION, lowG(axis), lowVariance);
		scalarMeasurementUpdate(axis, ACCELERATION, highG(axis), highGVariance);
	}

	if (data->MS560702BA03Pressure_hPA > 0.0f)
		scalarMeasurementUpdate(2, POSITION, data->MS560702BA03Altitude_m,
							baroNoise * baroNoise);

	return;
}

void Navigation::scalarMeasurementUpdate(int axis, AxisStateIndex stateIndex,
									 float measurement, float variance)
{
	if (!std::isfinite(measurement) || !std::isfinite(variance) || variance <= 0.0f)
		return;

	Matrix<float, AXIS_STATE_COUNT, 1> gain;
	const float innovationVariance = axisCovariance[axis](stateIndex, stateIndex) + variance;
	if (!std::isfinite(innovationVariance) || innovationVariance <= 1.0e-9f)
		return;

	for (int row = 0; row < AXIS_STATE_COUNT; ++row)
		gain(row) = axisCovariance[axis](row, stateIndex) / innovationVariance;

	const float innovation = measurement - axisState[axis](stateIndex);
	for (int row = 0; row < AXIS_STATE_COUNT; ++row)
		axisState[axis](row) += gain(row) * innovation;

	// Joseph-form scalar covariance update.
	Matrix<float, AXIS_STATE_COUNT, AXIS_STATE_COUNT> correction = I;
	for (int row = 0; row < AXIS_STATE_COUNT; ++row)
		correction(row, stateIndex) -= gain(row);

	axisCovariance[axis] = correction * axisCovariance[axis] * correction.transpose();
	for (int row = 0; row < AXIS_STATE_COUNT; ++row)
		for (int column = 0; column < AXIS_STATE_COUNT; ++column)
			axisCovariance[axis](row, column) += gain(row) * variance * gain(column);

	axisCovariance[axis] = axisCovariance[axis] + axisCovariance[axis].transpose();
	axisCovariance[axis] *= 0.5f;

	return;
}
