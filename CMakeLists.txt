cmake_minimum_required(VERSION 3.14)

project(Runner
    VERSION 0.1.0
    LANGUAGES CXX)

add_executable(runner WIN32
    ${CMAKE_SOURCE_DIR}/src/runner.cpp
)