# Specify minimum required version of CMake.
cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")

# Specify project and identify languages used.
project(cryo LANGUAGES CXX)

#find_package(Boost REQUIRED COMPONENTS signals)
find_library(wiringPi REQUIRED)
find_library(ncurses REQUIRED)

include_directories(include)

add_executable(cryo cryo.cpp ncurses_ui.cpp DS18B20.cpp rpiPWM1.cpp)

target_link_libraries(cryo wiringPi ncurses stdc++fs pthread)# Boost::signals)
