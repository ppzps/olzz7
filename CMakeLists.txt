cmake_minimum_required(VERSION 3.15)
project(olzz7)

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS "--std=c++11")

# include_directories("3rd")

add_executable(main main.cpp damage.cpp unit.cpp)

