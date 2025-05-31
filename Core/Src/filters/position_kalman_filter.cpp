#include "filters/position_kalman_filter.h"

Position_Kalman_Filter::Position_Kalman_Filter(Data *data)
    : Filter(data) {


    // State Transition Matrix
    W = {
        0.01f,
        0.01f,
        0.01f,
        0.01f,
        0.01f,
        0.01f,
    };

    // Covariance of State Transition Noise (Diagonal)
    Q = {
        { W(0), 0,    0,    0,    0,    0    },
        { 0,    W(1), 0,    0,    0,    0    },
        { 0,    0,    W(2), 0,    0,    0    },
        { 0,    0,    0,    W(3), 0,    0    },
        { 0,    0,    0,    0,    W(4), 0    },
        { 0,    0,    0,    0,    0,    W(5) }
    };

    // Measurement Noise
    V = {
        sqrt(1.5f),
        sqrt(1.5f),
        sqrt(1.5f),
        sqrt(1.5f)
    };

    // Covariance of Measurement Noise (Diagonal)
    R = {
        { V(0), 0,    0,    0 },
        { 0,    V(1), 0,    0 },
        { 0,    0,    V(2), 0 },
        { 0,    0,    0,    V(3) }
    };

    // Error Covariance
    P = I;

    // State to measuremnet Matrix
    H = {
        { 0, 0, 0, 1, 0, 0 },
		{ 0, 0, 0, 0, 1, 0 },
		{ 0, 0, 0, 0, 0, 1 },
		{ 0, 0, 1, 0, 0, 0 }
    };
}

void Position_Kalman_Filter::init() {

    // Set initial state
    X(0) = 0; // X Position
    X(1) = 0; // Y Position
    X(2) = 0; // Z Position
    X(3) = 0; // X Velocity
    X(4) = 0; // Y Velocity
    X(5) = 0; // Z Velocity

    // State Transition Matrix
    A = {
        { 1, 0, 0, 0.01, 0,    0    },
        { 0, 1, 0, 0,    0.01, 0    },
        { 0, 0, 1, 0,    0,    0.01 },
        { 0, 0, 0, 1,    0,    0    },
        { 0, 0, 0, 0,    1,    0    },
        { 0, 0, 0, 0,    0,    1    }
    };

    this->predict(); // Run the prediction step
    this->update();  // Run the estimation step
}

/**
 * Prediction Step
 *
 * Try and predict how X̂ₖ will evolve in the next time step
 *
 * Formulas
 *  X̂⁻ₖ = AX̂ₖ₋₁
 *  P⁻ₖ = APₖ₋₁Aᵀ + Q
 */
void Position_Kalman_Filter::predict() {
    // Step 1: Predict State and Error Covariance
    X = A * X;
    P = A * P * A.transpose() + Q;
}

/**
 * Estimation Step
 *
 * We compute the Kalman Gain, state estimation, and error covariance
 *
 * Formulas
 *  Kₖ = P⁻ₖHᵀ(HP⁻ₖHᵀ + R)⁻¹
 *  X̂ₖ = X̂⁻ₖ + Kₖ(Zₖ - HX̂⁻ₖ)
 *  Pₖ = P⁻ₖ - KₖHP⁻ₖ
 */
void Position_Kalman_Filter::update() {

    // Store the current measurement into the Z matrix
    Z(0) = data->LSM6DSV320_LowG_Accel.x; // X Acceleration
    Z(1) = data->LSM6DSV320_LowG_Accel.y; // Y Acceleration
    Z(2) = data->LSM6DSV320_LowG_Accel.z; // Z Acceleration
    Z(3) = data->MS5607_Altitude - data->startingAltitude; // Barometric Altitude

    // Step 2: Compute the Kalman Gain
	K = P * H.transpose() * (H * P * H.transpose() + R).inverse();

	// Step 3. Compute the estimate
	X = X + K * (Z - H * X);

	// Step 4. Compute the Error Covariance
	P = (I - K * H) * P;

}

void Position_Kalman_Filter::compute() {

    this->predict(); // Run the prediction step
    this->update();  // Run the estimation step

    data->position.x = X(0); // X Position
    data->position.y = X(1); // Y Position
    data->position.z = X(2); // Z Position

    data->velocity.x = X(3); // X Velocity
    data->velocity.y = X(4); // Y Velocity
    data->velocity.z = X(5); // Z Velocity
}