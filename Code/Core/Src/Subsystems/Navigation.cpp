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
	data->KalmanFilterPositionX_m = 0.0f;
	data->KalmanFilterPositionY_m = 0.0f;
	data->KalmanFilterPositionZ_m = 0.0f;

	data->KalmanFilterVelocityX_mps = 0.0f;
	data->KalmanFilterVelocityY_mps = 0.0f;
	data->KalmanFilterVelocityZ_mps = 0.0f;

	data->KalmanFilterAccelerationX_mps2 = 0.0f;
	data->KalmanFilterAccelerationY_mps2 = 0.0f;
	data->KalmanFilterAccelerationZ_mps2 = 0.0f;

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

	data->orientation_eular = data->orientation_quat.toRotationMatrix().eulerAngles(2, 1, 0) * RAD_TO_DEG;

	data->yaw   = data->orientation_eular[0]; // yaw (Z) in degrees
	data->pitch = data->orientation_eular[1]; // pitch (Y) in degrees
	data->roll  = data->orientation_eular[2]; // roll (X) in degrees

	// Rotate Accelerometer data by quaterion to get it to earth reference frame
	lowG  = data->orientation_quat * lowG;
	highG = data->orientation_quat * highG;

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

	// copy blocks for Y (3..5) and Z (6..8)
	Q.block<3,3>(3,3) = Q.block<3,3>(0,0);
	Q.block<3,3>(6,6) = Q.block<3,3>(0,0);

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
    // K = (P * H.transpose()) * S.inverse(); // less stable
	K = P * H.transpose() * S.ldlt().solve(Eigen::Matrix<float, KALMAN_FILTER_NUM_OF_MEASUREMENTS, KALMAN_FILTER_NUM_OF_MEASUREMENTS>::Identity());

	// Compute the Estimate ~0.5 ms
	x = x + K * (Z - H * x);

	// Computer the Error covariance ~10 ms
	P = (I - K * H) * P * (I - K * H).transpose() + K * R * K.transpose();

	return;
}
