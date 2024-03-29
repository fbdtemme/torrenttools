if (TARGET yaml-cpp)
    log_target_found(yaml-cpp)
    return()
endif()

find_package(yaml-cpp QUIET)
if (yaml-cpp_FOUND)
    log_module_found(yaml-cpp)
    return()
endif()

set(YAML_CPP_BUILD_TESTS OFF)
set(YAML_CPP_BUILD_TOOLS OFF)
set(YAML_CPP_INSTALL OFF)
# Silence CMP0077 warnings
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/yaml-cpp)
    log_dir_found(yaml-cpp)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/yaml-cpp)
    set(yaml-cpp_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/yaml-cpp)
else()
    log_fetch(yaml-cpp)

    include(FetchContent)
    FetchContent_Declare(
            yaml-cpp
            GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
            GIT_TAG        master
    )
    FetchContent_MakeAvailable(yaml-cpp)
endif()

if(IS_DIRECTORY "${yaml-cpp_SOURCE_DIR}")
    set_property(DIRECTORY ${yaml-cpp_SOURCE_DIR} PROPERTY EXCLUDE_FROM_ALL YES)
endif()