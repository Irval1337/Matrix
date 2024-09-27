#pragma once

#include "Matrix.t.h"
#include "Large.h"
#include <numeric>
#include <utility>
#include <cstdint>

struct ExpressionPart {
    Large coeff;
    int32_t var_index;
    ExpressionPart(Large coeff_, int32_t var_index_) : coeff(std::move(coeff_)), var_index(var_index_) { }
};

struct LinearSolution {
    ExpressionPart variable;
    std::vector<ExpressionPart> expression;

    explicit LinearSolution(Large coeff, int32_t ind) : variable(std::move(coeff), ind) { }
};

class LinearEquationSystem : Matrix<Large> {
public:
    LinearEquationSystem(const Matrix<Large>& ratio, const Matrix<Large>& rcol) :
        Matrix<Large>(ratio.rows(), ratio.columns() + 1) {
        ForEach([&](size_t i, size_t j, Large& elem) {
            elem = j < ratio.columns() ? ratio(i, j) : rcol(i, 0);
        });
    }

    [[nodiscard]] Matrix<Large> GetRatio() const noexcept;

    [[nodiscard]] Matrix<Large> GetColumn() const noexcept;

    void Solve() noexcept;

    [[nodiscard]] std::vector<LinearSolution> GetSolutions() const noexcept;

    friend std::ostream& operator<<(std::ostream& out, const LinearEquationSystem& les);
private:
    void SwapRows(size_t i, size_t j) noexcept;

    void MakeStepwise() noexcept;

    void MakeBetterStepwise() noexcept;

    void SimplifyRow(size_t row) noexcept;

    [[nodiscard]] Large gcd(const Large& a, const Large& b) const noexcept {
        if (b == 0) {
            return a;
        }
        return gcd(b, a % b);
    }
};
