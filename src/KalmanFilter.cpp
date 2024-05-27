// Implementation of the kalman filter defined in KalmanFilter.h
#include "KalmanFilter.h"

#include <math.h>

// Constructor
KalmanFilter::KalmanFilter() {
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
        sq(0.000795686), // Recorded data for 26.5 minutes and took the GUASE of the STDEV
        sq(0.000756131),
        sq(0.000426693),
        sq(0.020861282), 
        sq(0.020861282),
        sq(0.006379468),
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

    P = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1,
    };
}

void KalmanFilter::computeA(Data *data) {
    float x = data->gyrX * M_PI / 180;
    float y = data->gyrY * M_PI / 180;
    float z = data->gyrZ * M_PI / 180;

    // State Transition Matrix
    A = {
        0, -x, -y, -z,
        x,  0,  z, -y,
        y, -z,  0,  x,
        z,  y, -x,  0,
    };

    dt = data->dt;
    A *= I + dt * .5f;
}

void KalmanFilter::computeH(Data *data) {
    Quaternion q = data->quat;

    // State to Measurement Matrix
    H = {
         0,      -1/2,   0,     0,
         0,      0,      -1/2,  0,      // Gyro
         0,      0,      0,     -1/2,
        -2*q.y,  2*q.z, -2*q.w, 2*q.x,
         2*q.x,  2*q.w,  2*q.z, 2*q.y,  // Accel
         2*q.w, -2*q.x, -2*q.y, 2*q.z,
    };
}

void KalmanFilter::init(Data *data) {
    /**
     * This setups the starting State, and starting Error Covariance
     * 
     * It will use accerometer data to find the starting Quaternion
    */

    float pitch = asin(data->accX /  GRAVITY);
    float roll = asin(-data->accY / (GRAVITY * cos(pitch)));
    // roll needs magnetometer data to find the starting pitch and yaw
    float yaw = 0;

    data->quat = Quaternion(roll, pitch, yaw);

    X = {
        data->quat.w,
        data->quat.x,
        data->quat.y,
        data->quat.z,
    };

    P = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    this->predict(data); // Run the prediction
    this->update(data);  // Run the estimation
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

    computeA(data);

    // Step 1: Predict State and Error Covariance
    Xp = A * X;
    Pp = A * P * ~A + Q;

    data->quat = Quaternion(X(0), X(1), X(2), X(3));
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
    Z(0) = data->gyrX;
    Z(1) = data->gyrY;
    Z(2) = data->gyrZ;

    Z(3) = data->accX;
    Z(4) = data->accY;
    Z(5) = data->accZ;

    computeH(data);

    // Step 2: Compute the Kalman Gain
    K = Pp * ~H * Inverse(H * Pp * ~H + R);

    // Step 3. Compute the estimate
    X = Xp + K * (Z - (H * Xp));

    // Step 4. Compute the Error Covariance
    P = Pp - K * H * Pp;
}