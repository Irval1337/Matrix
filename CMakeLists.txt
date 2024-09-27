cmake_minimum_required(VERSION 3.28)
project(Matrix)

set(CMAKE_CXX_STANDARD 20)

add_executable(Matrix main.cpp
        Matrix.t.h
        Matrix.h
        LinearEquationSystem.cpp
        LinearEquationSystem.h
        Large.cpp
        Large.h
)
