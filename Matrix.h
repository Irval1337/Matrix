#pragma once

#include <vector>
#include <utility>
#include <iostream>
#include <functional>
#include "MatrixIterator.h"

template<arithmetic T>
class Matrix {
public:
#pragma region Constructors
    Matrix() : rows_(0), cols_(0) {};

    explicit Matrix(const std::vector<std::vector<T>>& other) : data_(other), rows_(other.size()) {
        FitMatrix();
        auto t = other[0].begin();
        t++;
    }

    Matrix(const size_t rows, const size_t cols) : rows_(rows), cols_(cols) {
        data_.assign(rows_, std::vector<T>(cols_));
    }

    Matrix(std::initializer_list<std::initializer_list<T>> list) : rows_(list.size()) {
        size_t row_ind = 0;
        data_.resize(rows_);
        for (auto row_it = list.begin(); row_it != list.end(); ++row_it) {
            data_[row_ind++] = std::vector<T>(*row_it);
        }
        FitMatrix();
    }

    Matrix(const Matrix<T>& other) : data_(other.data_), rows_(other.rows_), cols_(other.cols_) {}

    template<arithmetic U>
    Matrix(const Matrix<U>& other) : rows_(other.rows()), cols_(other.columns()) {
        data_.resize(rows_);
        for(size_t i = 0; i < rows_; ++i) {
            data_[i].resize(cols_);
            for(size_t j = 0; j < cols_; ++j) {
                data_[i][j] = static_cast<T>(other(i,j));
            }
        }
    }

    Matrix(Matrix<T>&& other) noexcept : data_(std::exchange(other.data_, std::vector<std::vector<T>>())),
                                         rows_(std::exchange(other.rows_, 0)),
                                         cols_(std::exchange(other.cols_, 0)) {}

    virtual ~Matrix() = default;
#pragma endregion

#pragma region Getters
    [[nodiscard]] size_t rows() const noexcept {
        return rows_;
    }

    [[nodiscard]] size_t columns() const noexcept {
        return cols_;
    }

    T& operator()(size_t row, size_t col) {
        if (row >= rows_ || col >= cols_) {
            throw std::out_of_range("Index out of range");
        }
        return data_[row][col];
    }

    const T& operator()(size_t row, size_t col) const {
        if (row >= rows_ || col >= cols_) {
            throw std::out_of_range("Index out of range");
        }
        return data_[row][col];
    }


#pragma endregion

#pragma region Operators
    bool operator==(const Matrix<T>& other) const noexcept;

    bool operator!=(const Matrix<T>& other) const noexcept;

    Matrix<T> operator-() const {
        return ForEach([](size_t i, size_t j, T& elem) {
            elem = -elem;
        });
    }

    Matrix<T> operator+() const noexcept {
        return *this;
    }

    template<arithmetic U>
    Matrix<T> operator*(U scalar) const noexcept {
        return ForEach([&](size_t i, size_t j, T& elem) {
            elem = elem * scalar;
        });
    }

    Matrix<T> operator/(T scalar) const {
        return ForEach([&](size_t i, size_t j, T& elem) {
            elem = elem / scalar;
        });
    }

    Matrix<T> operator+(const Matrix& other) const;

    Matrix<T> operator-(const Matrix& other) const {
        return *this + -1 * other;
    }

    Matrix<T> operator*(const Matrix& other) const;

    Matrix<T>& operator=(const Matrix<T>& other) noexcept {
        return *this = Matrix<T>(other);
    }

    Matrix<T>& operator=(Matrix<T>&& other) noexcept {
        swap(*this, other);
        return *this;
    }

    Matrix<T>& operator+=(const Matrix<T>& other) {
        return *this = *this + other;
    }

    Matrix<T>& operator-=(const Matrix<T>& other) {
        return *this = *this - other;
    }

    Matrix<T>& operator*=(const Matrix<T>& other) {
        return *this = *this * other;
    }

    Matrix<T>& operator*=(const T scalar) {
        return *this = *this * scalar;
    }

    Matrix<T>& operator/=(const T scalar) {
        return *this = *this / scalar;
    }
#pragma endregion

    Matrix<T> ForEach(std::function<void(size_t, size_t, T&)> func) const;

    T Trace() const noexcept;

    Matrix<T> Transpose() const noexcept;

#pragma Friends
    template<arithmetic U, arithmetic V>
    friend Matrix<U> operator*(V scalar, const Matrix<U>& mat);

    template<arithmetic U>
    friend void swap(Matrix<U>& lhs, Matrix<U>& rhs);

    template<arithmetic U>
    friend std::ostream& operator<<(std::ostream& out, const Matrix<U>& matrix);

    template<arithmetic U>
    friend Matrix<U> pow(const Matrix<U>& matrix, size_t power);
#pragma endregion

protected:
    std::vector<std::vector<T>> data_;
    size_t rows_, cols_{};

    void FitMatrix();
};
