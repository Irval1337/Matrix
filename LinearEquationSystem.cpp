#include "LinearEquationSystem.h"

std::ostream& operator<<(std::ostream& out, const LinearEquationSystem& matrix) {
    int need_width = 0;
    for(const auto& row : matrix.data_) {
        for(const auto& element : row) {
            need_width = std::max(need_width, (int)std::format("{}", element).size());
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

[[maybe_unused]] Matrix<double> LinearEquationSystem::GetRatio() const noexcept {
    Matrix<double> mat(rows(), columns() - 1);
    return mat.ForEach([&](size_t i, size_t j, double& elem) {
        elem = data_[i][j];
    });
}

[[maybe_unused]] Matrix<double> LinearEquationSystem::GetColumn() const noexcept {
    Matrix<double> mat(rows(), 1);
    return mat.ForEach([&](size_t i, size_t j, double& elem) {
        elem = data_[i][cols_ - 1];
    });
}

void LinearEquationSystem::SwapRows(size_t i, size_t j) noexcept {
    ForRow(i, [&](size_t col, double& elem) {
        std::swap(elem, data_[j][col]);
    });
}

void LinearEquationSystem::SimplifyRow(size_t row) noexcept {
    int gcd_ = 0;
    bool is_minus = false;
    ForRow(row, [&](size_t col, double& elem) {
       if (IsZero(elem)) {
           elem = 0;
           return;
       }
       if (!IsInteger(elem)) {
           gcd_ = 1;
           return;
       }
        if (elem < 0 && gcd_ == 0) {
            is_minus = true;
        }
       gcd_ = std::gcd(gcd_, static_cast<int>(std::abs(elem)));

    });
    if (is_minus) {
        gcd_ = -gcd_;
    }
    if (gcd_ != 1 && gcd_ != 0) {
        ForRow(row, [&](size_t col, double &elem) {
            if (!IsZero(elem)) {
                elem /= gcd_;
            }
        });
    }
}

void LinearEquationSystem::MakeStepwise() noexcept {
    size_t upper_row = 0;
    for(size_t column = 0; column + 1 < cols_; ++column) {
        int row_num = -1;
        for(size_t row = upper_row; row < rows_; ++row) {
            if (!IsZero(data_[row][column])) {
                row_num = static_cast<int>(row);
                break;
            }
        }
        if (row_num == -1) {
            continue;
        }
        SwapRows(upper_row, row_num);
        for(size_t row = upper_row + 1; row < rows_; ++row)  {
            int gcd_ = 1;
            double leading = data_[row][column];
            if (IsInteger(data_[upper_row][column]) && IsInteger(leading)) {
                gcd_ = std::gcd(static_cast<int>(data_[upper_row][column]), static_cast<int>(leading));
            }
            ForRow(row, [&](size_t j, double& elem) {
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
            if (!IsZero(data_[row][col])) {
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
            double leading = data_[other][non_zeros[row]];
            if (IsZero(leading)) {
                continue;
            }
            int gcd_ = 1;
            if (IsInteger(data_[row][non_zeros[row]]) && IsInteger(leading)) {
                gcd_ = std::gcd(static_cast<int>(data_[row][non_zeros[row]]), static_cast<int>(leading));
            }
            ForRow(other, [&](size_t col, double& elem) {
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
            if (IsZero(data_[row][col])) {
                continue;
            }
            found = true;
            solutions.emplace_back(data_[row][col], col + 1);
            for (size_t var_col = col + 1; var_col + 1 < cols_; ++var_col) {
                if (!IsZero(data_[row][var_col])) {
                    solutions.back().expression.emplace_back(-data_[row][var_col], var_col + 1);
                }
            }
            if (!IsZero(data_[row][cols_ - 1])) {
                solutions.back().expression.emplace_back(data_[row][cols_ - 1], 0);
            }
            break;
        }
        if (!found && !IsZero(data_[row][cols_ - 1])) return {};
    }

    return solutions;
}
