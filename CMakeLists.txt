cmake_minimum_required(VERSION 3.3)
project(SeedLESSCompiler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp LessParser.cpp LessParser.h ParseNode.cpp ParseNode.h Calculator.cpp Calculator.h)
add_executable(SeedLESSCompiler ${SOURCE_FILES})