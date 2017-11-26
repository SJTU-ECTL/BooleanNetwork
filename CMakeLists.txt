cmake_minimum_required(VERSION 3.8)
project(BooleanNetwork)
set(CMAKE_CXX_STANDARD 17)

set(lib_directory ${PROJECT_SOURCE_DIR}/lib)
set(bnet_include_directory ${PROJECT_SOURCE_DIR}/include/bnet)

add_subdirectory(src)
add_subdirectory(playground)