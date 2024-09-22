#pragma once

#include <iomanip>
#include <format>
#include "Matrix.h"

template<arithmetic T>
void Matrix<T>::FitMatrix() {
    size_t max_col = 0;
    for (const auto& row : data_) {
        max_col = std::max(max_col, row.size());
    }
    cols_ = max_col;

    for (auto& row : data_) {
        row.resize(max_col);
    }
}

template<arithmetic T>
bool Matrix<T>::operator==(const Matrix<T>& other) const noexcept {
    if (rows_ != other.rows_ || cols_ != other.cols_) return false;
    for (size_t i = 0; i < rows_; ++i) {
        if (data_[i] != other.data_[i]) {
            return false;
        }
    }
    return true;
}

template<arithmetic T>
bool Matrix<T>::operator!=(const Matrix<T>& other) const noexcept {
    return !(*this == other);
}

template<arithmetic T>
void swap(Matrix<T>& lhs, Matrix<T>& rhs) {
    std::swap(lhs.rows_, rhs.rows_);
    std::swap(lhs.cols_, rhs.cols_);
    std::swap(lhs.data_, rhs.data_);
}

template<arithmetic T>
std::ostream& operator<<(std::ostream& out, const Matrix<T>& matrix) {
    int need_width = 0;
    for(const auto& row : matrix.data_) {
        for(const auto& element : row) {
            need_width = std::max(need_width, (int)std::format("{}", element).size());
        }
    }
    for (size_t i = 0; i < matrix.data_.size(); ++i) {
        for (size_t j = 0; j < matrix.data_[i].size(); ++j) {
            out << std::setw(need_width) << matrix.data_[i][j];
            if (j + 1 < matrix.data_[i].size()) {
                out << " ";
            }
        }
        if (i + 1 < matrix.data_.size()) {
            out << std::endl;
        }
    }
    return out;
}

template<arithmetic T>
Matrix<T> Matrix<T>::ForEach(std::function<void(size_t, size_t, T&)> func) const {
    Matrix<T> mat = *this;
    for(size_t i = 0; i < rows_; ++i) {
        for(size_t j = 0; j < cols_; ++j) {
            func(i, j, mat.data_[i][j]);
        }
    }
    return mat;
}

template<arithmetic T, arithmetic U>
Matrix<T> operator*(const U scalar, const Matrix<T>& mat) {
    return mat * scalar;
}

template<arithmetic T>
Matrix<T> Matrix<T>::operator+(const Matrix<T>& other) const {
    if (rows_ != other.rows_ || cols_ != other.cols_) {
        throw std::length_error("The matrices have different sizes");
    }
    Matrix<T> mat(rows_, cols_);
    return mat.ForEach([&](size_t i, size_t j, T& elem) {
        elem = data_[i][j] + other.data_[i][j];
    });
}

template<arithmetic T>
T Matrix<T>::Trace() const noexcept {
    T result = 0;
    ForEach([&](size_t i, size_t j, T& elem) {
        if (i == j) {
            result += elem;
        }
    });
    return result;
}

template<arithmetic T>
Matrix<T> Matrix<T>::Transpose() const noexcept {
    Matrix<T> mat(cols_, rows_);
    return mat.ForEach([&](size_t i, size_t j, T& elem) {
        elem = this->data_[j][i];
    });
}

template<arithmetic T>
Matrix<T> Matrix<T>::operator*(const Matrix& other) const {
    if (cols_ != other.rows_) {
        throw std::length_error("The number of columns in the first matrix must match the number of rows in the second");
    }
    Matrix<T> mat(rows_, other.cols_);
    return mat.ForEach([&](size_t i, size_t j, T& elem) {
       size_t value = 0;
       for(size_t k = 0; k < cols_; ++k) {
           value += data_[i][k] * other.data_[k][j];
       }
       elem = value;
    });
}

template<arithmetic T>
Matrix<T> pow(const Matrix<T>& matrix, size_t power) {
    if (matrix.rows_ != matrix.cols_) {
        throw std::length_error("Only square matrices can be raised to a power");
    }
    if (power == 0) {
        Matrix<T> mat = Matrix<T>(matrix.rows_, matrix.rows_);
        return mat.ForEach([](size_t i, size_t j, T& elem) {
            elem = static_cast<T>(i == j);
        });
    }
    if (power & 1) {
        return matrix * pow(matrix, power - 1);
    }
    Matrix<int> tmp = pow(matrix, power >> 1);
    return tmp * tmp;
}