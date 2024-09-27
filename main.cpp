#include <iostream>
#include "LinearEquationSystem.h"

int main() {
    int N;
    std::cin >> N;
    Matrix<Large> A(N, N), B(N, 1);
    for(int i = 0; i < N; ++i) {
        for(int j = 0; j < N; ++j) {
            std::cin >> A(i, j);
        }
        std::cin >> B(i, 0);
    }
    LinearEquationSystem sys = {A, B};
    sys.Solve();
    std::cout << sys << std::endl << std::endl;

    auto solutions = sys.GetSolutions();
    if (solutions.empty()) {
        std::cout << "No solutions";
        return 0;
    }
    for (const auto &solution: solutions) {
        if (solution.variable.coeff == -1) {
            std::cout << "-";
        } else if (solution.variable.coeff != 1) {
            std::cout << solution.variable.coeff << "*";
        }
        std::cout << "x_" << solution.variable.var_index << " = ";

        bool is_first = true;
        for (const auto &var: solution.expression) {
            if (is_first && var.coeff < 0) {
                std::cout << "-";
            } else if (!is_first && var.coeff < 0) {
                std::cout << " - ";
            } else if (!is_first && var.coeff > 0) {
                std::cout << " + ";
            }

            if (var.var_index == 0) {
                std::cout << abs(var.coeff);
                continue;
            }

            if (abs(var.coeff) != 1) {
                std::cout << abs(var.coeff) << "*";
            }
            std::cout << "x_" << var.var_index;
            is_first = false;
        }
        std::cout << std::endl;
    }
}
