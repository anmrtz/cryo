# Specify minimum required version of CMake.
cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")

# Specify project and identify languages used.
project(cryo LANGUAGES CXX)

#find_library(wiringPi REQUIRED)
find_library(pigpio REQUIRED)

include_directories(include)

add_executable(cryo main.cpp cryo_control.cpp control_ui.cpp console_ui.cpp DS18B20.cpp pwm_control.cpp)

target_link_libraries(cryo pigpio stdc++fs atomic pthread)
