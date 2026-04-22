/*
 * Navigation.cpp
 *
 *  Created on: Oct 27, 2025
 *      Author: colin
 */

#include "Subsystems/Navigation.h"

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

	lastLoop = HAL_GetTick();
	last_us = micros();
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

	baro.update();

	data->startingBaroAltitude_m = data->MS560702BA03Altitude_m;

	baro.startConversion();

	return 0;
}


int Navigation::update()
{
	now = micros();

	// Handle micros() overflow
	if (now > lastLoop)
	{
		dt_us 	= now - lastLoop;
		dt_s  	= dt_us / 1000000.0f;
		freq 	= 1.0f / dt_s;
		lastLoop = now;
	}
	// If now < lastLoop, micros() has overflowed and pretend that the dt didn't change

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

	baro.update();
	gps.update();

	data->baroAltitudeOffset_m = data->MS560702BA03Altitude_m - data->startingBaroAltitude_m;

	// -------------------------------------------------------------
	// Kalman Filter
	// -------------------------------------------------------------

	if (!isKalmanFilterInit)
		initKalmanFilter();

	updateKalmanFilter();
	runKalmanFilter();

	data->KalmanFilterAltitude_m = x(0);
	data->KalmanFilterVerticalVelocity_mps = x(1);
	data->KalmanFilterVerticalAcceleration_mps2 = x(2);

	baro.startConversion();

	// Intergrate to find roll
	data->intergratedRoll += (rollRate_rad * dt_s) * RAD_TO_DEG;

	// Bound Roll Angle (-180, 180)
	if (data->intergratedRoll >  180) data->intergratedRoll -= 360;
	if (data->intergratedRoll < -180) data->intergratedRoll += 360;

	// Intergrate to find vertical velocity and altitude
	data->verticalAcceleration_mps2 = data->LSM6DSV320LowGAccelY_mps2 - 9.8f; // Subtract gravity to get vertical acceleration

	data->intergratedVerticalVelocity_mps	+= data->verticalAcceleration_mps2 * dt_s;
	data->intergratedVerticalAltitude_m 	+= data->intergratedVerticalVelocity_mps * dt_s;

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
	}

	float ax = data->LSM6DSV320LowGAccelX_mps2 / norm;
	float ay = data->LSM6DSV320LowGAccelY_mps2 / norm;
	float az = data->LSM6DSV320LowGAccelZ_mps2 / norm;

	data->quaternionW = sqrtf(1 + ax + ay + az) / 2;
	data->quaternionX = (ay - az) / (4 * data->quaternionW);
	data->quaternionY = (az - ax) / (4 * data->quaternionW);
	data->quaternionZ = (ax - ay) / (4 * data->quaternionW);

	// Calculate initial roll and pitch from accelerometer
	data->pitch = atan2f(-ax,  ay);
	data->yaw 	= atan2f(-az, -ay);
	data->roll 	= 0.0f;

	// Convert to quaternion
	float cr = cosf(data->roll * 0.5f);
    float sr = sinf(data->roll * 0.5f);
    float cp = cosf(data->pitch * 0.5f);
    float sp = sinf(data->pitch * 0.5f);
    float cy = cosf(data->yaw * 0.5f);
    float sy = sinf(data->yaw * 0.5f);

	// Quaternion from Euler angles
    data->quaternionW = cr * cp * cy + sr * sp * sy;
    data->quaternionX = cr * sp * cy + sr * cp * sy;
    data->quaternionY = sr * cp * cy - cr * sp * sy;
    data->quaternionZ = cr * cp * sy - sr * sp * cy;
}

void Navigation::initKalmanFilter()
{
	dt_s = NAVIGATION_TARGET_DT;
	dt2  = dt_s * dt_s;
	dt3  = dt2  * dt_s;
	dt4  = dt3  * dt_s;

	// Initialize Kalman Filter Matricies

	// Initial State
	x.setZero();
	x(0) = data->baroAltitudeOffset_m; // Pos Z Set the initial Barometric Altitude to the current altitude
	x(1) = 0; // Vertical Velocity
	x(2) = 0; // Vertical Acceleration

	// State Transition
	F.setIdentity();
	F(0, 0) = 1.0f;	F(0, 1) = dt_s;	F(0, 2) = 0.5f * dt2;
    F(1, 0) = 0.0f; F(1, 1) = 1.0f; F(1, 2) = dt_s;
    F(2, 0) = 0.0f; F(2, 1) = 0.0f; F(2, 2) = 1.0f;

	// Get Initial Measurements
	Z.setZero();
	Z(0) = data->baroAltitudeOffset_m;

    // state transition matrix H
	H.setZero();
    H(0, 0) = 1.0f; // Measurement directly corresponds to altitude
    H(1, 0) = 0.0f; // No direct measurement of velocity
    H(2, 0) = 0.0f; // No direct measurement of acceleration

	// Process Noise
	Q.setZero();
	Q(0, 0) = 0.25f * dt3;	Q(0, 1) = 0.5f * dt2;	Q(0, 2) = 0.5f * dt_s;
    Q(1, 0) = 0.5f  * dt2; 	Q(1, 1) = dt_s;			Q(1, 2) = 1.0f;
    Q(2, 0) = 0.5f  * dt_s;	Q(2, 1) = 1.0f;			Q(2, 2) = 1.0f;

	Q *= processNoise;

	// Measurement Noise
    R(0) = 5.0f; // Measurement noise variance

	// Estimate Error
	P.setIdentity();
	P *= 10.0;  // initial uncertainty

	I.setIdentity();

	isKalmanFilterInit = true;

	return;
}

void Navigation::updateKalmanFilter()
{
	// Update Measurements
	Z(1) = data->baroAltitudeOffset_m;

	// Update State Transition Matrix
	F(0, 0) = 1.0f; F(0, 1) = dt_s; F(0, 2) = 0.5f * dt2;
    F(1, 0) = 0.0f; F(1, 1) = 1.0f;	F(1, 2) = dt_s;
    F(2, 0) = 0.0f; F(2, 1) = 0.0f; F(2, 2) = 1.0f;

	// Update Process Noise
	Q.setZero();
	Q(0, 0) = 0.25f * dt3;	Q(0, 1) = 0.5f * dt2;	Q(0, 2) = 0.5f * dt_s;
    Q(1, 0) = 0.5f  * dt2; 	Q(1, 1) = dt_s;			Q(1, 2) = 1.0f;
    Q(2, 0) = 0.5f  * dt_s;	Q(2, 1) = 1.0f;			Q(2, 2) = 1.0f;

	Q *= processNoise;

	return;
}

void Navigation::runKalmanFilter()
{
	// Predict State ~0.5 ms
	x = F * x;

	// Predict Error Covariance ~3 ms
    S = H * P * H.transpose() + R;

	// Compute Kalman Gain ~5 ms
    // Prefer an LDLT or LLT solve over explicit inverse:
    K = (P * H.transpose()) * S.inverse();

	// Compute the Estimate ~0.5 ms
	x = x + K * (Z - H * x);

	// Computer the Error covariance ~10 ms
	P = (I - K * H) * P * (I - K * H).transpose() + K * R * K.transpose();

	return;
}
