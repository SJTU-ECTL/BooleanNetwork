set(playground_source_files playground.cpp)

include_directories(${bnet_include_directory})
link_directories(${lib_directory})

add_executable(playground ${playground_source_files})

target_link_libraries(playground bnet)