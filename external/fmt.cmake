
find_package(fmt QUIET)
if (fmt_FOUND)
    log_found(fmt)
else()
    set(FORMAT_BUILD_TESTING OFF)
    set(FMT_INSTALL OFF)

    if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/fmt)
        message(STATUS "fmt source directory found")
        add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/fmt)
        set(fmt_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/fmt)
    else()
        log_fetch(fmt)
        FetchContent_Declare(
                fmt
                GIT_REPOSITORY https://github.com/fmtlib/fmt.git
                GIT_TAG        master
        )
        FetchContent_MakeAvailable(fmt)
    endif()
endif()

if(IS_DIRECTORY "${fmt_SOURCE_DIR}")
    set_property(DIRECTORY ${fmt_SOURCE_DIR} PROPERTY EXCLUDE_FROM_ALL YES)
endif()