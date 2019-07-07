# Specify minimum required version of CMake.
cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")

# Specify project and identify languages used.
project(cryo LANGUAGES CXX)

find_library(pigpio REQUIRED)

include_directories(include)

add_executable(cryo main.cpp cryo_control.cpp control_ui.cpp console_ui.cpp adc_temp.cpp pwm_control.cpp server_ui.cpp)
add_executable(test_adc_temp test_adc_temp.cpp adc_temp.cpp)
add_executable(test_server test_server.cpp)

target_link_libraries(cryo pigpio stdc++fs atomic pthread zmq)
target_link_libraries(test_adc_temp pigpio atomic pthread)
target_link_libraries(test_server zmq)
