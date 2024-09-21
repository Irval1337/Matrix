#include <iostream>
#include "Matrix.t.h"

using namespace std;

Matrix<double> g() {
    return {{1, 2, 3}, {4, 5, 6}};
}

int main() {
    auto gay = g();
    Matrix<int> mat = gay;
    auto mat2 = Matrix<int>(5, 5);
    swap(mat, mat2);
    auto t = -g() * 5;
    cout << t / 2 << endl << endl;
    Matrix<int> bruh = {{ 1, 2, 3 }};
    Matrix<double> hah = {{ 0.5, 0.5, 0.5 }};
    cout << (hah + bruh).Transpose() << endl;
    cout << Matrix<int>({
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    }).Trace();
}
