// Implementation of the kalman filter defined in KalmanFilter.h
#include "KalmanFilter.h"

#include <math.h>

// Constructor
KalmanFilter::KalmanFilter() {
    // State Transition Matrix
    A = {
        1, 0, dt, 0, 
        0, 1, 0,  dt, 
        0, 0, 1,  sq(dt)/2, 
        0, 0, 0,  1
    };

    // State Transition Noise
    W = {
        0.001,
        0.001,
        0.001,
        0.001
    };

    // Covariance of State Transition Noise (diagonal)
    Q = { // Sensor Standard Deviation squared
        W(0), 0, 0, 0, 
        0, W(1), 0, 0, 
        0, 0, W(2), 0, 
        0, 0, 0, W(3)
    };

    // Measurement Noise
    V = { // Sensor Standard Deviation squared
        sq(ACC_STD), 
        sq(ACC_STD),
        sq(ACC_STD),
        sq(GYRO_STD),
        sq(GYRO_STD),
        sq(GYRO_STD)
    };

    // Covariance of Measurement Noise (diagonal)
    R = {
        V(0), 0, 0, 0, 0, 0,
        0, V(1), 0, 0, 0, 0,
        0, 0, V(2), 0, 0, 0,
        0, 0, 0, V(3), 0, 0,
        0, 0, 0, 0, V(4), 0,
        0, 0, 0, 0, 0, V(5)
    };
}

void KalmanFilter::computeH(Data *data) {
    Quaternion q = data->quat;

    // State to Measurement Matrix
    H = {
        1, -q.x, -q.y, -q.z,
        0,  q.w,  q.z, -q.y,
        0, -q.z,  q.w,  q.x,
        0,  1,    0,    0,
        0,  0,    1,    0,
        0,  0,    0,    1
    };
}

void KalmanFilter::init(Data *data) {
    /**
     * This setups the starting State, and starting Error Covariance
     * 
     * It will use accerometer data to find the starting Quaternion
    */

    float pitch = asin(data->accX.value / GRAVITY);
    float yaw = asin(-data->accY.value / GRAVITY);
    // roll needs magnetometer data to find the starting pitch and yaw
    float roll = 0;

    data->quat = Quaternion(roll, pitch, yaw);

    X = {
        data->quat.w,
        data->quat.x,
        data->quat.y,
        data->quat.z,
    };

    P = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };
}

/**
 * Kalman Filter Variables
 *  X: the state
 *  X̂: the estimation of the state
 *  X̂ₖ₋₁: the last state
 *  X̂ₖ-: the estimation before taking data
 *  X̂⁻ₖ₊₁: Estimate of X̂ₖ at time Tₖ₊₁
 *  Zₖ: The measurements
 *  A: The process we are interested in
 *  Q: Quantify the "Process" or "State Transition" noise
 *  H: How the measurement relates to the state
 *  R: Quantify the noise
 *  Pₖ: Error Covariance is the measurement of the inaccuracy of the estimation (Gaussian distribution)
 *  P⁻ₖ₊₁: Estimate of Pₖ at time Tₖ₊₁
 * 
 * Steps
 *  Predict state and Error covariance
 *  Compute Kalman gain
 *  Estimation Step
 *  Compute the Error covariance
*/

// Runs the prediction step of the kalman filter
void KalmanFilter::predict(Data *data) {
    /**
     * Prediction Step
     * 
     * Try and predict how X̂ₖ will evolve in the next time step
     * 
     * Formulas
     *  X̂⁻ₖ = AX̂ₖ₋₁
     *  P⁻ₖ = APₖ₋₁Aᵀ + Q
    */

    // Step 1: Predict State and Error Covariance
    X = A * X;
    P = A * P * A + Q;

    data->quat = Quaternion(X(1), X(2), X(3), X(4));
}

// Runs the estimation step of the kalman filter
void KalmanFilter::update(Data *data) {
    /**
     * Estimation Step
     *
     * We compute the Kalman Gain
     * the state estimation
     * as well as the error covariance
     * 
     * Formulas
     *  Kₖ = P⁻ₖHᵀ(HP⁻ₖHᵀ + R)⁻¹
     *  X̂ₖ = X̂⁻ₖ + Kₖ(Zₖ - HX̂⁻ₖ)
     *  Pₖ = P⁻ₖ - KₖHP⁻ₖ
    */ 

    // Store the measurement in the Z matrix
    Z(0) = data->accX.value;
    Z(1) = data->accY.value;
    Z(2) = data->accZ.value;

    Z(3) = data->gyrX.value;
    Z(4) = data->gyrY.value;
    Z(5) = data->gyrZ.value;

    computeH(data);

    // Step 2: Compute the Kalman Gain
    K = P * ~H * Inverse(H * P * ~H + R);

    // Step 3. Compute the estimate
    X = X + K * (Z - (H * X));

    // Step 4. Compute the Error Covariance
    P = P - K * H * P;
}
