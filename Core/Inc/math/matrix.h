#ifndef MATRIX_H
#define MATRIX_H

#include "stm32f4xx_hal.h"

/**
 * A class to represent a mathematical matrix
 *
 * @tparam nRows The number of rows in the matrix
 * @tparam nCols The number of columns in the matrix
 */
template <uint8_t nRows, uint8_t nCols>
class Matrix {
private:
    int matrix[nRows][nCols];

public:
    Matrix();
    ~Matrix();

    int getNRows() const { return nRows; }
    int getNCols() const { return nCols; }

    // Operations
    template <uint8_t otherRows, uint8_t otherCols>
    Matrix operator+(Matrix<otherRows, otherCols> &other);

    template <uint8_t otherRows, uint8_t otherCols>
    Matrix operator-(Matrix<otherRows, otherCols> &other);

    template <uint8_t otherRows, uint8_t otherCols>
    Matrix<nRows, otherCols> operator*(Matrix<otherRows, otherCols> &other);
    Matrix operator*(int &other);

    template <uint8_t otherRows, uint8_t otherCols>
    Matrix operator=(Matrix<otherRows, otherCols> &other);
    Matrix operator=(int other[nRows][nCols]);


    template <uint8_t otherRows, uint8_t otherCols>
    Matrix operator+=(Matrix<otherRows, otherCols> &other);

    template <uint8_t otherRows, uint8_t otherCols>
    Matrix operator-=(Matrix<otherRows, otherCols> &other);

    template <uint8_t otherRows, uint8_t otherCols>
    Matrix operator*=(Matrix<otherRows, otherCols> &other);
    Matrix operator*=(int &other);

    Matrix transpose();
    Matrix inverse();

    // Accessors
    int* operator[](int row);
    const int* operator[](int row) const;
};

#endif /* MATRIX_H */
