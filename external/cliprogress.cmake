if (TARGET cliprogress::cliprogress)
    log_target_found(cliprogress)
    return()
endif()

find_package(cliprogress QUIET)
if (cliprogress_FOUND)
    log_module_found(cliprogress)
    return()
endif()

set(CLIPROGRESS_BUILD_TESTING OFF)

if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/cliprogress)
    log_dir_found("cliprogress")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/cliprogress)
    set(cliprogress_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/cliprogress)
else()
    log_fetch(cliprogress)
    FetchContent_Declare(
            cliprogress
            GIT_REPOSITORY   https://github.com/fbdtemme/cliprogress.git
            GIT_TAG          main
    )
    FetchContent_MakeAvailable(cliprogress)
endif()

if(IS_DIRECTORY "${cliprogress_SOURCE_DIR}")
    set_property(DIRECTORY ${cliprogress_SOURCE_DIR} PROPERTY EXCLUDE_FROM_ALL YES)
endif()
