set(EXAMPLE_NAME ExampleTopsortBlifNetlist)

message(STATUS "Building Example: '${EXAMPLE_NAME}'")

add_executable(${EXAMPLE_NAME}
        topsort.cpp
        )

target_link_libraries(${EXAMPLE_NAME}
        PRIVATE ectlrf
        PRIVATE finder
        PRIVATE stopwatch
        )

get_target_property(interface_include
        ${EXAMPLE_NAME} INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(private_include
        ${EXAMPLE_NAME} INCLUDE_DIRECTORIES )
get_target_property(link_libs
        ${EXAMPLE_NAME} LINK_LIBRARIES)

message(STATUS "\tLinking libraries: ${link_libs}")
foreach(dir ${private_include})
    message(STATUS "\t[Build Include    ] '${private_include}'")
endforeach()
foreach(dir ${interface_include})
    message(STATUS "\t[Interface Include] '${interface_include}'")
endforeach()