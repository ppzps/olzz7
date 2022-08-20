cmake_minimum_required(VERSION 1.5.0)
project(olzz7)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS "--std=c++11")

# add_executable(attack attack.cpp damage.cpp unit.cpp)

# add_executable(test test.cpp damage.cpp unit.cpp)

add_executable(attack attack.cpp damage_new.cpp unit.cpp)
