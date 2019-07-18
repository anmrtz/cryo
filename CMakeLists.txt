# Specify minimum required version of CMake.
cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")

# Specify project and identify languages used.
project(cryo LANGUAGES CXX)

find_library(pigpio REQUIRED)

include_directories(include)

add_executable(cryo src/main.cpp src/cryo_control.cpp src/control_ui.cpp src/console_ui.cpp src/adc_temp.cpp src/pwm_control.cpp src/server_ui.cpp)
add_executable(test_adc_temp test/test_adc_temp.cpp src/adc_temp.cpp)
add_executable(test_server test/test_server.cpp)

target_link_libraries(cryo pigpio stdc++fs atomic pthread zmq)
target_link_libraries(test_adc_temp pigpio atomic pthread)
target_link_libraries(test_server zmq)
