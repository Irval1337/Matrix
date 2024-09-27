#include <iostream>
#include "LinearEquationSystem.h"

using namespace std;

int main() {
    srand(time(0));
    int N = 20;
    Matrix<int> A(N, N), B(N, 1);
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            A(i, j) = rand() % 100;
        }
        B(i, 0) = rand() % 100;
    }
    LinearEquationSystem sys = {A, B};
    sys.Solve();
    cout << sys << endl << endl;

    auto solutions = sys.GetSolutions();
    if (solutions.empty()) {
        cout << "No solutions";
        return 0;
    }
    for (const auto &solution: solutions) {
        if (solution.variable.coeff == -1) {
            cout << "-";
        } else if (solution.variable.coeff != 1) {
            cout << solution.variable.coeff << "*";
        }
        cout << "x_" << solution.variable.var_index << " = ";

        bool is_first = true;
        for (const auto &var: solution.expression) {
            if (is_first && var.coeff < 0) {
                cout << "-";
            } else if (!is_first && var.coeff < 0) {
                cout << " - ";
            } else if (!is_first && var.coeff > 0) {
                cout << " + ";
            }

            if (var.var_index == 0) {
                cout << abs(var.coeff);
                continue;
            }

            if (abs(var.coeff) != 1) {
                cout << abs(var.coeff) << "*";
            }
            cout << "x_" << var.var_index;
            is_first = false;
        }
        cout << endl;
    }
}
