#ifndef POSITION_KALMAN_FILTER_H
#define POSITION_KALMAN_FILTER_H

#include "filters/filter.hpp"

#include "datatypes/vector.h"
#include "math/matrix.hpp"

#include "micros.h"

class Position_Kalman_Filter : public Filter {
private:
    static const int N = 6; // Number of states
    // 3x Velocity, 3x Position

    static const int M = 4; // Number of measurements
    // 3x Acceloration, Barometric Altitude
    // Todo: 3x GPS (Altitude, Latitude, Longitude)

    Matrix<float, N, 1> X; // State Variables
    Matrix<float, M, 1> Z; // Measurement Variables
    Matrix<float, N, N> A; // State Transition Matrix
    Matrix<float, M, N> H; // State to Measurement Matrix
    Matrix<float, N, 1> W; // State Transition Noise
    Matrix<float, M, 1> V; // Measurement Noise
    Matrix<float, N, N> Q; // Covariance of State Transition Noise
    Matrix<float, M, M> R; // Covariance of Measurement Noise
    Matrix<float, N, N> P; // Error Covariance
    Matrix<float, N, M> K; // Kalman Filter Gain

    Matrix<float, N, N> I = { // Identity Matrix
        { 1, 0, 0, 0, 0, 0, },
        { 0, 1, 0, 0, 0, 0, },
        { 0, 0, 1, 0, 0, 0, },
        { 0, 0, 0, 1, 0, 0, },
        { 0, 0, 0, 0, 1, 0, },
        { 0, 0, 0, 0, 0, 1, },
    };


public:
    Position_Kalman_Filter(Data *data);

    void init() override;

    void predict();
    void update();

    void compute() override;
};

#endif /* POSITION_KALMAN_FILTER_H */
