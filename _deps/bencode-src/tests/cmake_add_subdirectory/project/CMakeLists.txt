cmake_minimum_required(VERSION 3.17)

project(CmakeAddSubdirectoryTest CXX)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(${bencode_source_dir} ${CMAKE_CURRENT_BINARY_DIR}/bencode)

add_executable(with_namespace_target main.cpp)
target_link_libraries(with_namespace_target PRIVATE bencode::bencode)

add_executable(without_namespace_target main.cpp)
target_link_libraries(without_namespace_target PRIVATE bencode)