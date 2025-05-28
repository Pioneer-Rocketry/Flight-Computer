#pragma once

#include <iostream>
#include <iomanip>
#include <cmath>

template<typename T, int ROWS, int COLS>
class Matrix {
private:
    T data[ROWS][COLS];

public:
    // Default constructor (zero initialization)
    Matrix() {
        for (int i = 0; i < ROWS; ++i)
            for (int j = 0; j < COLS; ++j)
                data[i][j] = T(0);
    }

    Matrix(std::initializer_list<std::initializer_list<T>> init) {
        int i = 0;
        for (auto row : init) {
            if (i >= ROWS)
                break;
            int j = 0;
            for (auto val : row) {
                if (j >= COLS)
                    break;
                data[i][j++] = val;
            }
            ++i;
        }
        // Fill remaining elements with zero
        for (; i < ROWS; ++i)
            for (int j = 0; j < COLS; ++j)
                data[i][j] = T(0);
    }

    // Flat initializer list constructor (row-major order)
    Matrix(std::initializer_list<T> init) {
        int count = 0;
        for (T val : init) {
            if (count >= ROWS * COLS)
                break;
            data[count / COLS][count % COLS] = val;
            ++count;
        }
        // Fill any remaining elements with 0
        for (; count < ROWS * COLS; ++count) {
            data[count / COLS][count % COLS] = T(0);
        }
    }


    // Access operators
    T& operator()(int row, int col) {
        // if (row < 0 || row >= ROWS || col < 0 || col >= COLS)
        //     throw std::out_of_range("Matrix index out of bounds");
        return data[row][col];
    }

    const T& operator()(int row, int col) const {
        // if (row < 0 || row >= ROWS || col < 0 || col >= COLS)
        //     throw std::out_of_range("Matrix index out of bounds");
        return data[row][col];
    }

    T& operator()(int index) {
        // static_assert(ROWS == 1 || COLS == 1, "1D indexing only allowed for vectors");
        // if (index < 0 || index >= ROWS * COLS)
        //     throw std::out_of_range("Index out of bounds");
        return (ROWS == 1) ? data[0][index] : data[index][0];
    }

    const T& operator()(int index) const {
        static_assert(ROWS == 1 || COLS == 1, "1D indexing only allowed for vectors");
        if (index < 0 || index >= ROWS * COLS)
            throw std::out_of_range("Index out of bounds");
        return (ROWS == 1) ? data[0][index] : data[index][0];
    }


    // Addition
    Matrix<T, ROWS, COLS> operator+(const Matrix<T, ROWS, COLS>& rhs) const {
        Matrix<T, ROWS, COLS> result;
        for (int i = 0; i < ROWS; ++i)
            for (int j = 0; j < COLS; ++j)
                result(i, j) = data[i][j] + rhs(i, j);
        return result;
    }

    // Subtraction
    Matrix<T, ROWS, COLS> operator-(const Matrix<T, ROWS, COLS>& rhs) const {
        Matrix<T, ROWS, COLS> result;
        for (int i = 0; i < ROWS; ++i)
            for (int j = 0; j < COLS; ++j)
                result(i, j) = data[i][j] - rhs(i, j);
        return result;
    }

    // Multiplication
    template<int OTHER_COLS>
    Matrix<T, ROWS, OTHER_COLS> operator*(const Matrix<T, COLS, OTHER_COLS>& rhs) const {
        Matrix<T, ROWS, OTHER_COLS> result;
        for (int i = 0; i < ROWS; ++i)
            for (int j = 0; j < OTHER_COLS; ++j)
                for (int k = 0; k < COLS; ++k)
                    result(i, j) += data[i][k] * rhs(k, j);
        return result;
    }

    // Transpose
    Matrix<T, COLS, ROWS> transpose() const {
        Matrix<T, COLS, ROWS> result;
        for (int i = 0; i < ROWS; ++i)
            for (int j = 0; j < COLS; ++j)
                result(j, i) = data[i][j];
        return result;
    }

    // Inverse (only for square matrices)
    Matrix<T, ROWS, COLS> inverse() const {
        static_assert(ROWS == COLS, "Inverse only defined for square matrices");
        const int N = ROWS;
        Matrix<T, N, N> aug;

        // Copy the input matrix and append identity
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
                aug(i, j) = data[i][j];

        Matrix<T, N, N> inv;
        for (int i = 0; i < N; ++i)
            inv(i, i) = T(1);

        // Gauss-Jordan elimination
        for (int i = 0; i < N; ++i) {
            // Pivot
            T pivot = aug(i, i);
            // if (std::abs(pivot) < 1e-10)
            //     throw std::runtime_error("Matrix is singular and cannot be inverted");

            for (int j = 0; j < N; ++j) {
                aug(i, j) /= pivot;
                inv(i, j) /= pivot;
            }

            for (int k = 0; k < N; ++k) {
                if (k == i) continue;
                T factor = aug(k, i);
                for (int j = 0; j < N; ++j) {
                    aug(k, j) -= factor * aug(i, j);
                    inv(k, j) -= factor * inv(i, j);
                }
            }
        }

        return inv;
    }

    // Print utility
    void print() const {
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j)
                std::cout << std::setw(8) << data[i][j] << " ";
            std::cout << "\n";
        }
    }
};