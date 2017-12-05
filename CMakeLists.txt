cmake_minimum_required(VERSION 3.8)
project(BooleanNetwork)
set(CMAKE_CXX_STANDARD 17)

set(lib_directory ${PROJECT_SOURCE_DIR}/lib)
set(bnet_include_directory ${PROJECT_SOURCE_DIR}/include/bnet)

add_definitions(-DPROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}")

message("Project Source Directory: ${PROJECT_SOURCE_DIR}")

add_subdirectory(src)
add_subdirectory(playground)