cmake_minimum_required(VERSION 3.10)
project(laguna)

set(CMAKE_CXX_STANDARD 14)

add_executable(laguna main.cpp frame/Frame.h frame/series.h)