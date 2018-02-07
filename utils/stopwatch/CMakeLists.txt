set(UTIL_NAME stopwatch)

message(STATUS "Loading util libray: '${UTIL_NAME}'")

set(HEADER_DIR      "include")
set(SOURCE_DIR      ".")

set(SOURCE
        ${SOURCE_DIR}/dummy.cpp
        )

set(HEADER
        ${HEADER_DIR}/${UTIL_NAME}/stopwatch.h
        )

set(ALL ${HEADER} ${SOURCE})

add_library(${UTIL_NAME} STATIC ${ALL})

target_include_directories(${UTIL_NAME}
        PUBLIC ${HEADER_DIR}
        )


get_target_property(interface_include
        ${UTIL_NAME} INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(private_include
        ${UTIL_NAME} INCLUDE_DIRECTORIES )

foreach(dir ${private_include})
    message(STATUS "\t[Build Include    ] '${dir}'")
endforeach()
foreach(dir ${interface_include})
    message(STATUS "\t[Interface Include] '${dir}'")
endforeach()
