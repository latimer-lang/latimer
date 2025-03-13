cmake_minimum_required(VERSION 3.31.5)
project(latimer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB LEXICAL "../lexical_analysis/*.hpp" "../lexical_analysis/*.cpp")
file(GLOB main "../main.cpp")

add_executable(latimer ${LEXICAL} ${main})