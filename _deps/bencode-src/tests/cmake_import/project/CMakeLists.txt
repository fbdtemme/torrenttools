cmake_minimum_required(VERSION 3.17)

project(CmakeImportTest CXX)

find_package(bencode REQUIRED)

add_executable(with_namespace_target main.cpp)
target_link_libraries(with_namespace_target PRIVATE bencode::bencode)