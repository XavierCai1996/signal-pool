cmake_minimum_required(VERSION 2.8)
project(example)
aux_source_directory(. DIR_EXAMPLE)
add_subdirectory(../src src.o)
add_executable(example ${DIR_EXAMPLE})
target_link_libraries(example src)
