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

Navigation::Navigation(DataContainer* data, SPI_HandleTypeDef* spiBus, UART_HandleTypeDef* uart)
	: Subsystem(data),
	  imu(data, spiBus, SPI_CS1_GPIO_Port, SPI_CS1_Pin),
	  baro(data, spiBus, BARO_CS_GPIO_Port, BARO_CS_Pin),
	  gps(data, uart)
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

	lastLoop = HAL_GetTick();
	last_us = micros();
}


int Navigation::init()
{

	if (imu.deviceInit() < 0)
	{
		return -1;
	}

	if (baro.deviceInit() < 0)
	{
		return -1;
	}

	if (gps.deviceInit() < 0)
	{
		return -1;
	}

	initializeQuaternion();

	return 0;
}


int Navigation::update()
{
	now 	= micros();

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

	baro.startConversion();

	imu.updateDevice();

	lowG[0] = data->LSM6DSV320LowGAccelX_mps2;
	lowG[1] = data->LSM6DSV320LowGAccelY_mps2;
	lowG[2] = data->LSM6DSV320LowGAccelZ_mps2;

	highG[0] = data->LSM6DSV320HighGAccelX_mps2;
	highG[1] = data->LSM6DSV320HighGAccelY_mps2;
	highG[2] = data->LSM6DSV320HighGAccelZ_mps2;

	// -------------------------------------------------------------
	// Quaterion Intergration
	// -------------------------------------------------------------
	rollRate_rad	= data->LSM6DSV320GyroY_dps * DEG_TO_RAD;
	pitchRate_rad 	= data->LSM6DSV320GyroX_dps * DEG_TO_RAD;
	yawRate_rad 	= data->LSM6DSV320GyroZ_dps * DEG_TO_RAD;

	// integrate quaternion
	integrateQuaternion();

	// Rotate Accelerometer data by quaterion to get it to earth reference frame
	// lowG  = data->orientation_quat * lowG;
	// highG = data->orientation_quat * highG;

	baro.updateDevice();

	// -------------------------------------------------------------
	// Kalman Filter
	// -------------------------------------------------------------

	if (!isKalmanFilterInit)
		initKalmanFilter();

	updateKalmanFilter();
	runKalmanFilter();

	data->KalmanFilterPositionX_m			= x(0);
	data->KalmanFilterAccelerationX_mps2	= x(1);
	data->KalmanFilterVelocityX_mps			= x(2);

	data->KalmanFilterPositionY_m			= x(3);
	data->KalmanFilterAccelerationY_mps2	= x(4);
	data->KalmanFilterVelocityY_mps			= x(5);

	data->KalmanFilterPositionZ_m			= x(6);
	data->KalmanFilterAccelerationZ_mps2	= x(7);
	data->KalmanFilterVelocityZ_mps			= x(8);

	return 0;
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
	x(2) = lowG[0]; // Acc X
	x(5) = lowG[1]; // Acc Y
	x(6) = data->MS560702BA03Altitude_m; // Pos Z Set the initial Barometric Altitude to the current altitude
	x(8) = lowG[2]; // Acc Z

	// State Transition
	F.setIdentity();
    // For each axis block:
    // pos' = pos + vel*dt + 0.5*acc*dt^2
    // vel' = vel + acc*dt
    // acc' = acc

	// X Axis
	F(0,1) = dt_s;
	F(0,2) = (dt_s * dt_s) / 2.0f;
	F(1,2) = dt_s;

	// Y Axis
    F(3,4) = dt_s;
    F(3,5) = (dt_s * dt_s) / 2.0f;
    F(4,5) = dt_s;

    // Z Axis
    F(6,7) = dt_s;
    F(6,8) = (dt_s * dt_s) / 2.0f;
    F(7,8) = dt_s;

	// Get Initial Measurements
	Z.setZero();
	Z(0) = data->LSM6DSV320LowGAccelX_mps2;
	Z(1) = data->LSM6DSV320LowGAccelY_mps2;
	Z(2) = data->LSM6DSV320LowGAccelZ_mps2;
	Z(3) = data->LSM6DSV320HighGAccelX_mps2;
	Z(4) = data->LSM6DSV320HighGAccelY_mps2;
	Z(5) = data->LSM6DSV320HighGAccelZ_mps2;
	Z(6) = data->MS560702BA03Altitude_m;

	// Observation
	H.setZero();
	H(0, 2) = H(1, 5) = H(2, 8) = H(3, 2) = H(4, 5) = H(5, 8) = H(6, 6) = 1.0f;

	// Process Noise
	Q.setZero();
	Q(0,0) = dt4 / 4.0f;
	Q(0,1) = dt3 / 2.0f; Q(1,0) = Q(0,1);
	Q(0,2) = dt2 / 2.0f; Q(2,0) = Q(0,2);
	Q(1,1) = dt2;
	Q(1,2) = dt_s; Q(2,1) = Q(1,2);
	Q(2,2) = 1.0f; // keep small baseline

	Q(3,3) = dt4 / 4.0f;
	Q(3,4) = dt3 / 2.0f; Q(4,3) = Q(3,4);
	Q(3,5) = dt2 / 2.0f; Q(5,3) = Q(3,5);
	Q(4,4) = dt2;
	Q(4,5) = dt_s; Q(5,4) = Q(4,5);
	Q(5,5) = 1.0f; // keep small baseline

	Q(6,6) = dt4 / 4.0f;
	Q(6,7) = dt3 / 2.0f; Q(7,6) = Q(6,7);
	Q(6,8) = dt2 / 2.0f; Q(8,6) = Q(6,8);
	Q(7,7) = dt2;
	Q(7,8) = dt_s; Q(8,7) = Q(7,8);
	Q(8,8) = 1.0f; // keep small baseline

	// copy blocks for Y (3..5) and Z (6..8)

	Q *= processNoise;

	// Measurement Noise
	R.setIdentity();
	R(0, 0) = R(1, 1) = R(2, 2) = powf(lowGNoise, 2);
	R(3, 3) = R(4, 4) = R(5, 5) = powf(highGNoise, 2);
	R(6, 6) = powf(baroNoise, 2);

	// Estimate Error
	P.setIdentity();
	P *= 1.0;  // initial uncertainty

	I.setIdentity();

	isKalmanFilterInit = true;

	return;
}

void Navigation::updateKalmanFilter()
{
	// Update Measurements
	Z(0) = lowG[0];
	Z(1) = lowG[1];
	Z(2) = lowG[2];
	Z(3) = highG[0];
	Z(4) = highG[1];
	Z(5) = highG[2];
	Z(6) = data->MS560702BA03Altitude_m;

	// Update State Transition Matrix

	// X Axis
	F(0,1) = dt_s;
	F(0,2) = (dt_s * dt_s) / 2.0f;
	F(1,2) = dt_s;

	// Y Axis
    F(3,4) = dt_s;
    F(3,5) = (dt_s * dt_s) / 2.0f;
    F(4,5) = dt_s;

    // Z Axis
    F(6,7) = dt_s;
    F(6,8) = (dt_s * dt_s) / 2.0f;
    F(7,8) = dt_s;

	// Update Process Noise
	Q.setZero();
	Q(0,0) = dt4 / 4.0f;
	Q(0,1) = dt3 / 2.0f; Q(1,0) = Q(0,1);
	Q(0,2) = dt2 / 2.0f; Q(2,0) = Q(0,2);
	Q(1,1) = dt2;
	Q(1,2) = dt_s; Q(2,1) = Q(1,2);
	Q(2,2) = 1.0f; // keep small baseline

	Q(3,3) = dt4 / 4.0f;
	Q(3,4) = dt3 / 2.0f; Q(4,3) = Q(3,4);
	Q(3,5) = dt2 / 2.0f; Q(5,3) = Q(3,5);
	Q(4,4) = dt2;
	Q(4,5) = dt_s; Q(5,4) = Q(4,5);
	Q(5,5) = 1.0f; // keep small baseline

	Q(6,6) = dt4 / 4.0f;
	Q(6,7) = dt3 / 2.0f; Q(7,6) = Q(6,7);
	Q(6,8) = dt2 / 2.0f; Q(8,6) = Q(6,8);
	Q(7,7) = dt2;
	Q(7,8) = dt_s; Q(8,7) = Q(7,8);
	Q(8,8) = 1.0f; // keep small baseline

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
