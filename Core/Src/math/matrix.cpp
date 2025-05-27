#include "math/matrix.h"

template <uint8_t nRows, uint8_t nCols>
Matrix<nRows, nCols>::Matrix() {
    for (uint8_t r = 0; r < nRows; r++) {
        for (uint8_t c = 0; c < nCols; c++) {
            matrix[r][c] = 0;
        }
    }
}

template <uint8_t nRows, uint8_t nCols>
Matrix<nRows, nCols>::~Matrix() {}

template <uint8_t nRows, uint8_t nCols>
template <uint8_t otherRows, uint8_t otherCols>
Matrix<nRows, nCols> Matrix<nRows, nCols>::operator+(Matrix<otherRows, otherCols> &other) {
    static_assert(nRows == other.nRows && nCols == other.nCols, "Matrix dimensions must match"); // Checks to make sure matrix dimensions match

    Matrix<nRows, nCols> result;
    for (uint8_t r = 0; r < nRows; r++) {
        for (uint8_t c = 0; c < nCols; c++) {
            result[r][c] = matrix[r][c] + other[r][c];
        }
    }

    return result;
}

template <uint8_t nRows, uint8_t nCols>
template <uint8_t otherRows, uint8_t otherCols>
Matrix<nRows, nCols> Matrix<nRows, nCols>::operator-(Matrix<otherRows, otherCols> &other) {
    static_assert(nRows == other.nRows && nCols == other.nCols, "Matrix dimensions must match"); // Checks to make sure matrix dimensions match

    Matrix<nRows, nCols> result;
    for (uint8_t r = 0; r < nRows; r++) {
        for (uint8_t c = 0; c < nCols; c++) {
            result[r][c] = matrix[r][c] - other[r][c];
        }
    }

    return result;
}

template <uint8_t nRows, uint8_t nCols>
template <uint8_t otherRows, uint8_t otherCols>
Matrix<nRows, otherCols> Matrix<nRows, nCols>::operator*(Matrix<otherRows, otherCols> &other) {
    static_assert(nCols == other.nRows, "Matrix dimensions must match"); // Checks to make sure matrix dimensions match

    Matrix<nRows, otherCols> result;
    for (uint8_t r = 0; r < nRows; ++r) {
        for (uint8_t c = 0; c < otherCols; ++c) {
            result[r][c] = 0;
            for (uint8_t i = 0; i < nCols; ++i) {
                result[r][c] += matrix[r][i] * other[i][c];
            }
        }
    }

    return result;
}

template <uint8_t nRows, uint8_t nCols>
Matrix<nRows, nCols> Matrix<nRows, nCols>::operator*(int &other) {
    Matrix<nRows, nCols> result;
    for (uint8_t r = 0; r < nRows; r++) {
        for (uint8_t c = 0; c < nCols; c++) {
            result[r][c] = matrix[r][c] * other;
        }
    }

    return result;
}

template <uint8_t nRows, uint8_t nCols>
template <uint8_t otherRows, uint8_t otherCols>
Matrix<nRows, nCols> Matrix<nRows, nCols>::operator=(Matrix<otherRows, otherCols> &other) {
    for (uint8_t r = 0; r < nRows; r++) {
        for (uint8_t c = 0; c < nCols; c++) {
            matrix[r][c] = other[r][c];
        }
    }

    return *this;
}

template <uint8_t nRows, uint8_t nCols>
Matrix<nRows, nCols> Matrix<nRows, nCols>::operator=(int other[nRows][nCols]) {
    for (uint8_t r = 0; r < nRows; r++) {
        for (uint8_t c = 0; c < nCols; c++) {
            matrix[r][c] = other[r][c];
        }
    }

    return *this;
}

template <uint8_t nRows, uint8_t nCols>
template <uint8_t otherRows, uint8_t otherCols>
Matrix<nRows, nCols> Matrix<nRows, nCols>::operator+=(Matrix<otherRows, otherCols> &other) {
    *this = (*this + other);
    return *this;
}

template <uint8_t nRows, uint8_t nCols>
template <uint8_t otherRows, uint8_t otherCols>
Matrix<nRows, nCols> Matrix<nRows, nCols>::operator-=(Matrix<otherRows, otherCols> &other) {
    *this = (*this - other);
    return *this;
}

template <uint8_t nRows, uint8_t nCols>
template <uint8_t otherRows, uint8_t otherCols>
Matrix<nRows, nCols> Matrix<nRows, nCols>::operator*=(Matrix<otherRows, otherCols> &other) {
    *this = (*this * other);
    return *this;
}

template <uint8_t nRows, uint8_t nCols>
Matrix<nRows, nCols> Matrix<nRows, nCols>::operator*=(int &other) {
    *this = (*this * other);
    return *this;
}

template <uint8_t nRows, uint8_t nCols>
Matrix<nRows, nCols> Matrix<nRows, nCols>::transpose() {
    // TODO: implement transpose
}

template <uint8_t nRows, uint8_t nCols>
Matrix<nRows, nCols> Matrix<nRows, nCols>::inverse() {
    // TODO: implement inverse
}

template <uint8_t nRows, uint8_t nCols>
int* Matrix<nRows, nCols>::operator[](int row) {
    return matrix[row];
}


template <uint8_t nRows, uint8_t nCols>
const int* Matrix<nRows, nCols>::operator[](int row) const {
    return matrix[row];
}
