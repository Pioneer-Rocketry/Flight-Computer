// Kalman.h
#ifndef KALMAN_H_
#define KALMAN_H_

#include <Arduino.h>
#include <math.h>

#include <BasicLinearAlgebra.h>

#include "Data.hpp"
#include "utils/Quaternion.hpp"

#define GRAVITY 9.8f 

class KalmanFilter {
	private:
		static const int N = 6; // Number of state variables
		// Velocity, Position
		static const int M = 4; // Number of measurement variables
		// Accelormeter, Barometer /  GPS

		BLA::Matrix<N, 1> X;  // State Variable
		BLA::Matrix<M, 1> Z;  // Measurement Variable
		BLA::Matrix<N, N> A;  // State Transition Matrix
		BLA::Matrix<M, N> H;  // State to Measurement Matrix
		BLA::Matrix<N, 1> W;  // State Transition Noise
		BLA::Matrix<M, 1> V;  // Measurement Noise
		BLA::Matrix<N, N> Q;  // Covariance of State Transition Noise
		BLA::Matrix<M, M> R;  // Covariance of Measurement Noise
		BLA::Matrix<M, M> R_GPS;  // Covariance of Measurement Noise for the GPS
		BLA::Matrix<N, N> P;  // Error Covariance
		BLA::Matrix<N, M> K;  // Kalman Filter Gain

		const BLA::Matrix<N, N> I = { // Identity Matrix
			1, 0, 0, 0, 0, 0,
			0, 1, 0, 0, 0, 0,
			0, 0, 1, 0, 0, 0,
			0, 0, 0, 1, 0, 0,
			0, 0, 0, 0, 1, 0,
			0, 0, 0, 0, 0, 1,
		};

		float dt = 1/30;

		Data *data;

		void save();

	public:
		KalmanFilter(Data *data);

		void init();

		void predict();
		void update();
		void updateGPS();

		void run();
};
#endif // KALMAN_H_