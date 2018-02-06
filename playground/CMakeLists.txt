set(playground_source_files playground.cpp sta.cpp sta.h type_erasure.cpp te_public_header.h)
set(CMAKE_CXX_STANDARD 17)

include_directories(${bnet_include_directory})
link_directories(${lib_directory})

#add_executable(playground ${playground_source_files})


#add_executable(te type_erasure.cpp te_client.cpp)


#target_link_libraries(playground bnet)