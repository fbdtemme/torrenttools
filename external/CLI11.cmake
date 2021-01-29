if (TARGET CLI11::CLI11)
    log_target_found(CLI11)
    return()
endif()

find_package(CLI11 QUIET)
if (CLI11_FOUND)
    log_module_found(CLI11)
    return()
endif()

# disable tests
set(CLI11_BUILD_DOCS OFF)
set(CLI11_BUILD_EXAMPLES OFF)
set(CLI11_BUILD_TESTS OFF)

if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/CLI11)
    log_dir_found(CLI11)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/CLI11)
    set(CLI11_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/CLI11)
else()
    log_fetch(CLI11)

    FetchContent_Declare(
            CLI11
            GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
            GIT_TAG        master
    )
    FetchContent_MakeAvailable(CLI11)
endif()

if(IS_DIRECTORY "${CLI11_SOURCE_DIR}")
    set_property(DIRECTORY ${CLI11_SOURCE_DIR} PROPERTY EXCLUDE_FROM_ALL YES)
endif()
