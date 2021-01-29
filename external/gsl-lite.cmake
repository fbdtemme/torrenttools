if (TARGET gsl::gsl-lite-v1)
    log_target_found(gsl-lite)
    return()
endif()

find_package(gsl-lite QUIET)
if (gsl-lite_FOUND)
    log_module_found(gsl-lite)
    return()
endif()

if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/gsl-lite)
    log_dir_found(gsl-lite)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/gsl-lite)
    set(gsl-lite_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/gsl-lite)
else()
    log_fetch(gsl-lite)
    FetchContent_Declare(
            gsl-lite
            GIT_REPOSITORY https://github.com/gsl-lite/gsl-lite.git
            GIT_TAG        master
    )
    FetchContent_MakeAvailable(gsl-lite)
endif()


if(IS_DIRECTORY "${gsl-lite_SOURCE_DIR}")
    set_property(DIRECTORY ${gsl-lite_SOURCE_DIR} PROPERTY EXCLUDE_FROM_ALL YES)
endif()