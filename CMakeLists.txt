cmake_minimum_required(VERSION 3.8)
project(BooleanNetwork)
set(CMAKE_CXX_STANDARD 17)

set(lib_directory ${PROJECT_SOURCE_DIR}/lib)
set(bnet_include_directory ${PROJECT_SOURCE_DIR}/include/bnet)

# option(USE_SYSTEM_BOOST ON)
set(BOOST_ROOT /usr/local/Cellar/boost/1.66.0/)

add_definitions(-DPROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}")

message("Project Source Directory: ${PROJECT_SOURCE_DIR}")

include_directories(lib)

add_subdirectory(src)
add_subdirectory(playground)