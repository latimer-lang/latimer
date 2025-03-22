cmake_minimum_required(VERSION 3.31.5)
project(latimer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(ROOT ${CMAKE_SOURCE_DIR})

file(GLOB LEXICAL "${ROOT}/lexical_analysis/*.hpp" "${ROOT}/lexical_analysis/*.cpp")
file(GLOB PARSING "${ROOT}/ast/*.hpp" "${ROOT}/ast/*.cpp")
file(GLOB UTILS "${ROOT}/utils/*.hpp" "${ROOT}/utils/*.cpp")
file(GLOB main "${ROOT}/main.cpp")

add_executable(latimer ${LEXICAL} ${PARSING} ${UTILS} ${main})