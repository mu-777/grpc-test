cmake_minimum_required(VERSION 3.7)
project(grpc_test)

set(CMAKE_CXX_STANDARD 14)
find_package(gRPC CONFIG REQUIRED)

add_executable(grpc_test main.cpp)