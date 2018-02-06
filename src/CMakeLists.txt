set(LIB_NAME        ${EXPORT_LIB_NAME})
set(HEADER_DIR      "include")
set(SOURCE_DIR      ".")

message(STATUS "Loading library for target '${LIB_NAME}':")

set(SOURCE
        #${SOURCE_DIR}/netlist.cpp
        ${SOURCE_DIR}/bnet.cpp
        ${SOURCE_DIR}/blif_netlist.cpp
        )

set(HEADER
        ${HEADER_DIR}/${LIB_NAME}/ectlrf.h
        ${HEADER_DIR}/${LIB_NAME}/blif_netlist.h
        ${HEADER_DIR}/${LIB_NAME}/internals/common.h
        ${HEADER_DIR}/${LIB_NAME}/internals/blif_netlist.h
        ${HEADER_DIR}/${LIB_NAME}/internals/indirect_array_list.h
        ${HEADER_DIR}/${LIB_NAME}/internals/indirect_array_list.hpp
        )

set(ALL_FILES ${HEADER} ${SOURCE})

add_library(${LIB_NAME} STATIC
        ${ALL_FILES}
        $<TARGET_OBJECTS:cudd_blif_obj>)

target_include_directories(ectlrf
        PUBLIC ${HEADER_DIR})

# Use object library to hide the Cudd* files
# No longer need to link these files for projects depending
# on this library

target_link_libraries(ectlrf PRIVATE cudd_blif)

get_target_property(interface_include
        ${LIB_NAME} INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(private_include
        ${LIB_NAME} INCLUDE_DIRECTORIES )

get_target_property(link_libs
        ${LIB_NAME} LINK_LIBRARIES)
message(STATUS "\tLinking libraries: ${link_libs}")
foreach(dir ${private_include})
    message(STATUS "\t[Build Include    ] '${dir}'")
endforeach()
foreach(dir ${interface_include})
    message(STATUS "\t[Interface Include] '${dir}'")
endforeach()