cmake_minimum_required(VERSION 3.8)
project(BooleanNetwork VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)


set(EXPORT_LIB_NAME ectlrf)

# LOADING UP BOOST
find_package(Boost REQUIRED)
add_library(boost INTERFACE IMPORTED)
set_property(TARGET boost PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})

set(LIB_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(SRC_DIRECTORY ${PROJECT_SOURCE_DIR}/src)
set(EXAMPLES_PATH ${PROJECT_SOURCE_DIR}/examples)
set(PLAYGROUND_PATH ${PROJECT_SOURCE_DIR}/playground)

add_definitions(-DPROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}")

message(STATUS "Project Source Directory: ${PROJECT_SOURCE_DIR}")

add_subdirectory(${LIB_DIRECTORY})
add_subdirectory(${SRC_DIRECTORY})
add_subdirectory(${EXAMPLES_PATH})
add_subdirectory(${PLAYGROUND_PATH})