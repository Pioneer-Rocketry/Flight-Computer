// Implementation of the kalman filter defined in KalmanFilter.h
#include "KalmanFilter.h"

#include <math.h>

// Constructor
KalmanFilter::KalmanFilter() {

    // State Transition Noise
    W = {
        0.01,
        0.01,
    };

    // Covariance of State Transition Noise (diagonal)
    Q = { // Sensor Standard Deviation squared
        W(0), 0,
        0, W(1),
    };

    // Measurement Noise
    V = { // Sensor Standard Deviation squared
        sq(1.5f),
    };

    // Covariance of Measurement Noise (diagonal)
    R = {
        V(0)
    };

    // Error Covariance
    P = {
        1, 0,
        0, 1,
    };

    // State to Measurement Matrix
    H = {
        1, 0,
        // 1, 0,
    };
}

void KalmanFilter::init(Data *data) {
    /**
     * This setups the starting State, and starting Error Covariance
     * 
     * It will use accerometer data to find the starting Quaternion
    */

    X(0) = data->starting_alt;
    X(1) = 0.0f;

    dt = data->dt;

    // State Transistion Matrix
    A = {
        1, dt,
        0, 1,
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

    // Step 1: Predict State and Error Covariance
    X = A * X;
    P = A * P * ~A + Q;

    data->filted_alt = X(0);
    data->verticalVelocity = X(1);

    data->p1 = P(0);
    data->p2 = P(1);
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
    
    Z(0) = data->alt;

    // Step 2: Compute the Kalman Gain
    K = P * ~H * Inverse(H * P * ~H + R);

    // Step 3. Compute the estimate
    X = X + K * (Z - H * X);

    // Step 4. Compute the Error Covariance
    P = (I - K * H) * P;

    data->filted_alt = X(0);
    data->verticalVelocity = X(1);

    data->p1 = P(0);
    data->p2 = P(1, 1);
}

// Runs the estimation step of the kalman filter
void KalmanFilter::updateGPS(Data *data) {
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
    
    Z(0) = data->gps_alt;

    // Step 2: Compute the Kalman Gain
    K = P * ~H * Inverse(H * P * ~H + R_GPS);

    // Step 3. Compute the estimate
    X = X + K * (Z - H * X);

    // Step 4. Compute the Error Covariance
    P = (I - K * H) * P; // * ~(I - K * H) + K * R * ~K;

    data->filted_alt = X(0);
    data->verticalVelocity = X(1);

    data->p1 = P(0);
    data->p2 = P(1, 1);
}

void KalmanFilter::run(Data *data) {
    /**
     * Kalman Filter Flow
     * 
     * We are running the Kalman Filter at 30hz right now
     * At somepoint we should up it to 100hz, even if data is only 30hz
     * 
     * Starting with the basic altitude Kalman Filter
     * We run predict and update on each iteration of the loop
     * and if the GPS altitude is available then we run update with the releated GPS data
     * 
     */

    predict(data);

    if (data->newBaroData) {
        update(data);
    }

    if (data->newGPSdata) {
        R_GPS = {
            sq(data->gps_hdop)
        };

        updateGPS(data);
    }
}