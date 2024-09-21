#pragma once

#include <vector>
#include <exception>

template <typename T> concept arithmetic = std::is_arithmetic_v<T>;

template<arithmetic T>
class MatrixIterator {
public:
    MatrixIterator(std::vector<std::vector<T>>* m_ptr) : m_ptr_(m_ptr), curr_row_(0) {
        if (m_ptr == nullptr) {
            throw std::out_of_range("Matrix pointer was null");
        }
        rows_ = m_ptr_->size();
        cols_ = rows_ == 0 ? 0 : m_ptr_->at(0).size();
        if (rows_ * cols_ == 0) {
            return;
        }
        iter_ = m_ptr->at(0).begin();
    }

    virtual bool operator==(const MatrixIterator& other) const noexcept {
        return m_ptr_ == other.m_ptr_ && iter_ == other.iter_;
    }

    bool operator!=(const MatrixIterator& other) const noexcept {
        return !(*this == other);
    }

    virtual MatrixIterator& GetBegin() {
        if (rows_ * cols_ == 0) {
            return nullptr;
        }
        return m_ptr_->at(0);
    }

    virtual MatrixIterator& GetEnd() {
        if (rows_ * cols_ == 0) {
            return nullptr;
        }
        return m_ptr_->at(rows_ - 1).end();
    }

    virtual MatrixIterator& operator++() {
        if (rows_ * cols_ == 0 || curr_row_ == rows_ - 1 && iter_ == m_ptr_->at(curr_row_).end()) {
            throw std::out_of_range("Iterator out of range");
        }
        if (iter_ == m_ptr_->at(curr_row_).end()) {
            ++curr_row_;
            iter_ = m_ptr_->at(curr_row_).begin();
        } else {
            ++iter_;
        }
    }

    virtual MatrixIterator& operator--() {
        if (rows_ * cols_ == 0 || curr_row_ == 0 && iter_ == m_ptr_->at(curr_row_).begin()) {
            throw std::out_of_range("Iterator out of range");
        }
        if (iter_ == m_ptr_->at(curr_row_).begin()) {
            --curr_row_;
            iter_ = m_ptr_->at(curr_row_).end();
        }
        --iter_;
    }

    virtual T *operator->(){
        if (rows_ * cols_ == 0 || curr_row_ == rows_ - 1 && iter_ == m_ptr_->at(curr_row_).end()) {
            throw std::out_of_range("Iterator out of range");
        }
        return *iter_;
    }

private:
    std::vector<std::vector<T>>* m_ptr_;
    int curr_row_;
    int rows_, cols_;
    std::vector<T>::iterator iter_;
};

template<arithmetic T>
class MatrixRowIterator : MatrixIterator<T> {

};

template<arithmetic T>
class MatrixColumnIterator : MatrixIterator<T> {

};