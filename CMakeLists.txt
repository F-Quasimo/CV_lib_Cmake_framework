cmake_minimum_required(VERSION 3.0)

project(cmake-opencv-static)

set(CMAKE_CXX_STANDARD 11)

include(cmake/opencv.cmake)

add_subdirectory(src)