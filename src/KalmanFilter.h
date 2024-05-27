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
		static const int N = 4; // Number of state variables
		static const int M = 6; // Number of measurement variables

		BLA::Matrix<N, 1> X;  // State Variable
		BLA::Matrix<N, 1> Xp; // Predicted State Variable
		BLA::Matrix<M, 1> Z;  // Measurement Variable
		BLA::Matrix<N, N> A;  // State Transition Matrix
		BLA::Matrix<M, N> H;  // State to Measurement Matrix
		BLA::Matrix<N, 1> W;  // State Transition Noise
		BLA::Matrix<M, 1> V;  // Measurement Noise
		BLA::Matrix<N, N> Q;  // Covariance of State Transition Noise
		BLA::Matrix<M, M> R;  // Covariance of Measurement Noise
		BLA::Matrix<N, N> P;  // Error Covariance
		BLA::Matrix<N, N> Pp; // Predicted Error Covariance
		BLA::Matrix<N, M> K;  // Kalman Filter Gain

		const BLA::Matrix<N, N> I = { // Identity Matrix
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1,
		};

		float dt = 1/30;

		// ([% / 100] / 2) * full_range
        const float ACC_STD  = .11; // Accelerometer standard deviation (±1% to ±4% at measurement of ±16 is around )
        const float GYRO_STD = 20.0; // Gyroscope standard deviation (±1% to ±3%)

		void computeH(Data *data);
		void computeA(Data *data);
		
	public:
		KalmanFilter();

		void init(Data *data);

		void predict(Data *data);
		void update(Data *data);
};
#endif // KALMAN_H_