/*
 * Navigation.cpp
 *
 *  Created on: Oct 27, 2025
 *      Author: colin
 */

#include "Subsystems/Navigation.h"

Navigation::Navigation(DataContainer* data, SPI_HandleTypeDef* spiBus, UART_HandleTypeDef* uart)
	: Subsystem(data),
	  imu(data, spiBus, SPI_CS1_GPIO_Port, SPI_CS1_Pin),
	  baro(data, spiBus, BARO_CS_GPIO_Port, BARO_CS_Pin),
	  gps(data, uart)
{
	data->KalmanFilterPositionX = 0.0f;
	data->KalmanFilterPositionY = 0.0f;
	data->KalmanFilterPositionZ = 0.0f;

	data->KalmanFilterVelocityX = 0.0f;
	data->KalmanFilterVelocityY = 0.0f;
	data->KalmanFilterVelocityZ = 0.0f;

	data->KalmanFilterAccelerationX = 0.0f;
	data->KalmanFilterAccelerationY = 0.0f;
	data->KalmanFilterAccelerationZ = 0.0f;

	// orientation initial guess
	data->orientation_quat = Eigen::Quaternionf::Identity();

	data->roll = 0.0f;
	data->pitch = 0.0f;
	data->yaw = 0.0f;

	lastLoop = HAL_GetTick();
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

	return 0;
}


int Navigation::update()
{
	now 	= HAL_GetTick();
	dt_ms 	= now - lastLoop;
	dt_s  	= dt_ms / 1000.0f;
	freq 	= 1.0f / dt_s;
	lastLoop = now;

	dt2 = dt_s * dt_s;
	dt3 = dt2  * dt_s;
	dt4 = dt3  * dt_s;

	// -------------------------------------------------------------
	// Read Sensor Data
	// -------------------------------------------------------------

	imu.updateDevice();

	baro.updateDevice();

	// -------------------------------------------------------------
	// Quaterion Intergration
	// -------------------------------------------------------------
	rollRate_rad	= data->LSM6DSV320GyroY * DEG_TO_RAD;
	pitchRate_rad 	= data->LSM6DSV320GyroX * DEG_TO_RAD;
	yawRate_rad 	= data->LSM6DSV320GyroZ * DEG_TO_RAD;

	// integrate quaternion
	integrateQuaternion();

	data->orientation_eular = data->orientation_quat.toRotationMatrix().eulerAngles(2, 1, 0) * RAD_TO_DEG;

	data->yaw   = data->orientation_eular[0];   // yaw (Z) in degrees
	data->pitch = data->orientation_eular[1];   // pitch (Y) in degrees
	data->roll  = data->orientation_eular[2];   // roll (X) in degrees

	// -------------------------------------------------------------
	// Kalman Filter
	// -------------------------------------------------------------

	if (!isKalmanFilterInit)
		initKalmanFilter();

	updateKalmanFilter();
	runKalmanFilter();

	data->KalmanFilterPositionX 	= x(0);
	data->KalmanFilterAccelerationX = x(1);
	data->KalmanFilterVelocityX 	= x(2);

	data->KalmanFilterPositionY 	= x(3);
	data->KalmanFilterAccelerationY = x(4);
	data->KalmanFilterVelocityY 	= x(5);

	data->KalmanFilterPositionZ 	= x(6);
	data->KalmanFilterAccelerationZ = x(7);
	data->KalmanFilterVelocityZ 	= x(8);

	return 0;
}

void Navigation::integrateQuaternion()
{
    // Build the angular velocity quaternion: q_omega = [0, wx, wy, wz]
    omega_q = Eigen::Quaternionf(0.0f, pitchRate_rad, rollRate_rad, yawRate_rad);

    // q_dot = 0.5 * q * omega_q
    q_dot = Eigen::Quaternionf(0,0,0,0); // initialize
    q_dot.coeffs() = 0.5f * (data->orientation_quat * omega_q).coeffs();

    // integrate (explicit Euler)
    qcoeffs = data->orientation_quat.coeffs() + q_dot.coeffs() * dt_s;
    data->orientation_quat = Eigen::Quaternionf(qcoeffs);
    data->orientation_quat.normalize();
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
	x(2) = data->LSM6DSV320LowGAccelX; // Acc X
	x(5) = data->LSM6DSV320LowGAccelY; // Acc Y
	x(6) = data->MS560702BA03Altitude; // Pos Z Set the initial Barometric Altitude to the current altitude
	x(8) = data->LSM6DSV320LowGAccelZ; // Acc Z

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
	Z(0) = data->LSM6DSV320LowGAccelX;
	Z(1) = data->LSM6DSV320LowGAccelY;
	Z(2) = data->LSM6DSV320LowGAccelZ;
	Z(3) = data->LSM6DSV320HighGAccelX;
	Z(4) = data->LSM6DSV320HighGAccelY;
	Z(5) = data->LSM6DSV320HighGAccelZ;
	Z(6) = data->MS560702BA03Altitude;

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

	// copy blocks for Y (3..5) and Z (6..8)
	Q.block<3,3>(3,3) = Q.block<3,3>(0,0);
	Q.block<3,3>(6,6) = Q.block<3,3>(0,0);

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
	Z(0) = data->LSM6DSV320LowGAccelX;
	Z(1) = data->LSM6DSV320LowGAccelY;
	Z(2) = data->LSM6DSV320LowGAccelZ;
	Z(3) = data->LSM6DSV320HighGAccelX;
	Z(4) = data->LSM6DSV320HighGAccelY;
	Z(5) = data->LSM6DSV320HighGAccelZ;
	Z(6) = data->MS560702BA03Altitude;

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

	// copy blocks for Y (3..5) and Z (6..8)
	Q.block<3,3>(3,3) = Q.block<3,3>(0,0);
	Q.block<3,3>(6,6) = Q.block<3,3>(0,0);

	Q *= processNoise;

	return;
}

void Navigation::runKalmanFilter()
{
	// Predict State
	x = F * x;

	// Predict Error Covariance
    S = H * P * H.transpose() + R;

	// Compute Kalman Gain
    // Prefer an LDLT or LLT solve over explicit inverse:
    // K = (P * H.transpose()) * S.inverse(); // less stable
	K = P * H.transpose() * S.ldlt().solve(Eigen::Matrix<float, KALMAN_FILTER_NUM_OF_MEASUREMENTS, KALMAN_FILTER_NUM_OF_MEASUREMENTS>::Identity());

	// Compute the Estimate
	x = x + K * (Z - H * x);

	// Computer the Error covariance
	P = (I - K * H) * P * (I - K * H).transpose() + K * R * K.transpose();

	return;
}
