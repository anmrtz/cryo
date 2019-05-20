# Specify minimum required version of CMake.
cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -lpthread")

# Specify project and identify languages used.
project(cryo LANGUAGES CXX)

#find_package(Boost REQUIRED)
find_library(wiringPi REQUIRED)

include_directories(include)

add_executable(cryo cryo.cpp)

target_link_libraries(cryo wiringPi)

