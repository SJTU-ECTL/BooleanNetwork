set(bnet_source_files bnet.cpp
        cudd_bnet.cpp
        safe_mem.c
        st.c
        strsav.c
        ucbqsort.c
        config.h
        util.h
        netlist.cpp
        netlist.h
        )
set(bnet_header_files bnet.h cudd_bnet.h config.h st.h util.h)

add_library(bnet_shared SHARED ${bnet_source_files})
add_library(bnet_static STATIC ${bnet_source_files})
set_target_properties(bnet_shared PROPERTIES OUTPUT_NAME "bnet")
set_target_properties(bnet_static PROPERTIES OUTPUT_NAME "bnet")

add_executable(IndirectArrayListTest
        indirect_array_list_example.cpp
        ectl_internals.h ectl_internals.hpp
        netlist.h)

install(TARGETS bnet_shared bnet_static
        LIBRARY DESTINATION ${lib_directory}
        ARCHIVE DESTINATION ${lib_directory})
install(FILES ${bnet_header_files}
        DESTINATION ${bnet_include_directory})

add_custom_target(install_bnet
        "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target install
        COMMENT "Installing ${PROJECT_NAME}")