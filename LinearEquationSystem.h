#pragma once

#include "Matrix.t.h"
#include <numeric>
#include <utility>

struct ExpressionPart {
    double coeff;
    int var_index;
    ExpressionPart(double coeff_, int var_index_) : coeff(coeff_), var_index(var_index_) { }
};

struct LinearSolution {
    ExpressionPart variable;
    std::vector<ExpressionPart> expression;

    explicit LinearSolution(double coeff, int ind) : variable(coeff, ind) { }
};

class LinearEquationSystem : Matrix<double> {
public:
    const double EPS = 1e-7;

    LinearEquationSystem(const Matrix<double>& ratio, const Matrix<double>& rcol) :
        Matrix<double>(ratio.rows(), ratio.columns() + 1) {
        ForEach([&](size_t i, size_t j, double& elem) {
            elem = j < ratio.columns() ? ratio(i, j) : rcol(i, 0);
        });
    }

    [[nodiscard]] Matrix<double> GetRatio() const noexcept;

    [[nodiscard]] Matrix<double> GetColumn() const noexcept;

    void Solve() noexcept;

    std::vector<LinearSolution> GetSolutions() const noexcept;

    friend std::ostream& operator<<(std::ostream& out, const LinearEquationSystem& les);
private:
    void SwapRows(size_t i, size_t j) noexcept;

    void MakeStepwise() noexcept;

    void MakeBetterStepwise() noexcept;

    void SimplifyRow(size_t row) noexcept;

    [[nodiscard]] bool IsZero(const double& val) const noexcept {
        return std::abs(val) <= EPS;
    }

    [[nodiscard]] bool IsInteger(const double& val) const noexcept {
        return IsZero(val - static_cast<int>(val));
    }
};
