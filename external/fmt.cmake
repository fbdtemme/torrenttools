if (TARGET fmt::fmt)
    log_target_found(fmt)
    return()
endif()


find_package(fmt 8.1.1 QUIET)
if (fmt_FOUND)
    log_module_found(fmt)
    return()
endif()

set(FORMAT_BUILD_TESTING OFF)
set(FMT_INSTALL OFF)

if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/fmt)
    log_dir_found(fmt)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/fmt)
    set(fmt_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/fmt)
else()
    log_fetch(fmt)
    FetchContent_Declare(
            fmt
            GIT_REPOSITORY https://github.com/fmtlib/fmt.git
            GIT_TAG        8.1.1
    )
    FetchContent_MakeAvailable(fmt)
endif()

if(IS_DIRECTORY "${fmt_SOURCE_DIR}")
    set_property(DIRECTORY ${fmt_SOURCE_DIR} PROPERTY EXCLUDE_FROM_ALL YES)
endif()