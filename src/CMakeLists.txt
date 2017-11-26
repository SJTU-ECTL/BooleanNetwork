set(bnet_source_files bnet.c)
set(bnet_header_files bnet.h config.h st.h util.h)

add_library(bnet_shared SHARED ${bnet_source_files})
add_library(bnet_static STATIC ${bnet_source_files})
set_target_properties(bnet_shared PROPERTIES OUTPUT_NAME "bnet")
set_target_properties(bnet_static PROPERTIES OUTPUT_NAME "bnet")

install(TARGETS bnet_shared bnet_static
        LIBRARY DESTINATION ${lib_directory}
        ARCHIVE DESTINATION ${lib_directory})
install(FILES ${bnet_header_files}
        DESTINATION ${bnet_include_directory})

add_custom_target(install_bnet
        "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target install
        COMMENT "Installing ${PROJECT_NAME}")