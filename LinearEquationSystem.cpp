#include "LinearEquationSystem.h"

std::ostream& operator<<(std::ostream& out, const LinearEquationSystem& matrix) {
    int32_t need_width = 0;
    for(const auto& row : matrix.data_) {
        for(const auto& element : row) {
            need_width = std::max(need_width, static_cast<int32_t>(to_string(element).size()));
        }
    }
    for (size_t i = 0; i < matrix.data_.size(); ++i) {
        for (size_t j = 0; j < matrix.data_[i].size(); ++j) {
            if (j + 1 == matrix.data_[i].size()) {
                out << std::setw(need_width) << "| ";
            }
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

[[maybe_unused]] Matrix<Large> LinearEquationSystem::GetRatio() const noexcept {
    Matrix<Large> mat(rows(), columns() - 1);
    return mat.ForEach([&](size_t i, size_t j, Large& elem) {
        elem = data_[i][j];
    });
}

[[maybe_unused]] Matrix<Large> LinearEquationSystem::GetColumn() const noexcept {
    Matrix<Large> mat(rows(), 1);
    return mat.ForEach([&](size_t i, size_t j, Large& elem) {
        elem = data_[i][cols_ - 1];
    });
}

void LinearEquationSystem::SwapRows(size_t i, size_t j) noexcept {
    ForRow(i, [&](size_t col, Large& elem) {
        std::swap(elem, data_[j][col]);
    });
}

void LinearEquationSystem::SimplifyRow(size_t row) noexcept {
    Large gcd_ = 0;
    bool is_minus = false;
    ForRow(row, [&](size_t col, Large& elem) {
       if (elem == 0) {
           elem = 0;
           return;
       }
        if (elem < 0 && gcd_ == 0) {
            is_minus = true;
        }
       gcd_ = gcd(gcd_, abs(elem));

    });
    if (is_minus) {
        gcd_ = -gcd_;
    }
    if (gcd_ != 1 && gcd_ != 0) {
        ForRow(row, [&](size_t col, Large &elem) {
            if (elem != 0) {
                elem /= gcd_;
            }
        });
    }
}

void LinearEquationSystem::MakeStepwise() noexcept {
    size_t upper_row = 0;
    for(size_t column = 0; column + 1 < cols_; ++column) {
        int32_t row_num = -1;
        for(size_t row = upper_row; row < rows_; ++row) {
            if (data_[row][column] != 0) {
                row_num = static_cast<int32_t>(row);
                break;
            }
        }
        if (row_num == -1) {
            continue;
        }
        SwapRows(upper_row, row_num);
        for(size_t row = upper_row + 1; row < rows_; ++row)  {
            Large gcd_ = gcd(data_[upper_row][column], data_[row][column]);
            Large leading = data_[row][column];
            ForRow(row, [&](size_t j, Large& elem) {
                elem = elem * data_[upper_row][column] / gcd_ - data_[upper_row][j] * leading / gcd_;
            });
            SimplifyRow(row);
        }
        ++upper_row;
    }
}

void LinearEquationSystem::MakeBetterStepwise() noexcept {
    std::vector<size_t> non_zeros;
    for(size_t row = 0; row < rows_; ++row) {
        for(size_t col = 0; col + 1 < cols_; ++col) {
            if (data_[row][col] != 0) {
                non_zeros.push_back(col);
                break;
            }
        }
    }
    if (non_zeros.empty()) {
        return;
    }

    for(size_t row = non_zeros.size() - 1; row > 0; --row) {
        for(size_t other = 0; other < row; ++other) {
            if (data_[other][non_zeros[row]] == 0) {
                continue;
            }
            Large gcd_ = gcd(data_[row][non_zeros[row]], data_[other][non_zeros[row]]);
            Large leading = data_[other][non_zeros[row]];
            ForRow(other, [&](size_t col, Large& elem) {
                elem = elem * data_[row][non_zeros[row]] / gcd_ - data_[row][col] * leading / gcd_;
            });
            SimplifyRow(other);
        }
    }
}

void LinearEquationSystem::Solve() noexcept {
    for(size_t row = 0; row < rows_; ++row) {
        SimplifyRow(row);
    }
    MakeStepwise();
    MakeBetterStepwise();
}

std::vector<LinearSolution> LinearEquationSystem::GetSolutions() const noexcept {
    std::vector<LinearSolution> solutions;
    for(size_t row = 0; row < rows_; ++row) {
        bool found = false;
        for (size_t col = 0; col + 1 < cols_; ++col) {
            if (data_[row][col] == 0) {
                continue;
            }
            found = true;
            solutions.emplace_back(data_[row][col], col + 1);
            for (size_t var_col = col + 1; var_col + 1 < cols_; ++var_col) {
                if (data_[row][var_col] != 0) {
                    solutions.back().expression.emplace_back(-data_[row][var_col], var_col + 1);
                }
            }
            if (data_[row][cols_ - 1] != 0 || solutions.back().expression.empty()) {
                solutions.back().expression.emplace_back(data_[row][cols_ - 1], 0);
            }
            break;
        }
        if (!found && data_[row][cols_ - 1] != 0) return {};
    }

    return solutions;
}
