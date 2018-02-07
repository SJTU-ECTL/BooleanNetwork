set(LIB_NAME cudd_blif)

message(STATUS "Loading dependent libray: '${LIB_NAME}'")

set(HEADER_DIR      "include")
set(SOURCE_DIR      ".")

set(LIBCUDD_BLIF_SOURCE
        ${SOURCE_DIR}/cudd_bnet.cpp
        ${SOURCE_DIR}/safe_mem.c
        ${SOURCE_DIR}/st.c
        ${SOURCE_DIR}/strsav.c
        ${SOURCE_DIR}/ucbqsort.c
        )

set(LIBCUDD_BLIF_HEADER
        ${HEADER_DIR}/${LIB_NAME}/util.h
        ${HEADER_DIR}/${LIB_NAME}/cudd_bnet.h
        ${HEADER_DIR}/${LIB_NAME}/config.h
        ${HEADER_DIR}/${LIB_NAME}/st.h
        )

set(LIBCUDD_ALL ${LIBCUDD_BLIF_HEADER} ${LIBCUDD_BLIF_SOURCE})

add_library(cudd_blif       STATIC ${LIBCUDD_ALL})
add_library(cudd_blif_obj   OBJECT ${LIBCUDD_BLIF_SOURCE})

target_include_directories(cudd_blif
        PUBLIC ${HEADER_DIR}
        )

target_include_directories(cudd_blif_obj
        PUBLIC ${HEADER_DIR}
        )

get_target_property(interface_include
        cudd_blif INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(private_include
        cudd_blif INCLUDE_DIRECTORIES )

foreach(dir ${private_include})
    message(STATUS "\t[Build Include    ] '${dir}'")
endforeach()
foreach(dir ${interface_include})
    message(STATUS "\t[Interface Include] '${dir}'")
endforeach()
