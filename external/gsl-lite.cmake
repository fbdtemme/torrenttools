
find_package(gsl-lite QUIET)
if (gsl-lite_FOUND)
    log_fetch(gsl-lite)
else()
    if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/gsl-lite)
        message(STATUS "gsl-lite source directory found")
        add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/gsl-lite)
        set(gsl-lite_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/gsl-lite)
    else()
        log_found(gsl-lite)
        FetchContent_Declare(
                gsl-lite
                GIT_REPOSITORY https://github.com/gsl-lite/gsl-lite.git
                GIT_TAG        master
        )
        FetchContent_MakeAvailable(gsl-lite)
    endif()
endif()

if(IS_DIRECTORY "${gsl-lite_SOURCE_DIR}")
    set_property(DIRECTORY ${gsl-lite_SOURCE_DIR} PROPERTY EXCLUDE_FROM_ALL YES)
endif()