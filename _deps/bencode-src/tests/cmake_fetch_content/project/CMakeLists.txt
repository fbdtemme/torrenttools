cmake_minimum_required(VERSION 3.11)

project(CMakeFetchContentTest CXX)

include(FetchContent)

get_filename_component(GIT_REPOSITORY_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../.. ABSOLUTE)
FetchContent_Declare(bencode GIT_REPOSITORY ${GIT_REPOSITORY_DIRECTORY} GIT_TAG HEAD)

FetchContent_GetProperties(bencode)
if(NOT bencode_POPULATED)
    FetchContent_Populate(bencode)
    add_subdirectory(${bencode_SOURCE_DIR} ${bencode_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

add_executable(with_namespace_target main.cpp)
target_link_libraries(with_namespace_target bencode::bencode)

add_executable(without_namespace_target main.cpp)
target_link_libraries(without_namespace_target bencode)