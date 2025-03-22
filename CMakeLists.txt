cmake_minimum_required(VERSION 3.31.5)
project(latimer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# .cpp source files in src/
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

add_executable(latimer ${SRC_FILES})

# .hpp header files in include/
# only adds them to the latimer binary
target_include_directories(latimer PRIVATE ${CMAKE_SOURCE_DIR}/include)