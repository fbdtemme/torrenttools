if (TARGET dottorrent::dottorrent)
    log_target_found(dottorrent)
    return()
endif()

find_package(dottorrent QUIET)
if (dottorrent_FOUND)
    log_module_found(dottorrent)
    return()
endif()

set(DOTTORRENT_TESTS OFF)
set(DOTTORRENT_INSTALL OFF)

if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/dottorrent)
    log_dir_found(dottorrent)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/dottorrent)
    set(dottorrent_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/dottorrent)
else()
    log_fetch(dottorrent)
    FetchContent_Declare(
            dottorrent
            GIT_REPOSITORY   https://github.com/fbdtemme/dottorrent.git
            GIT_TAG          master
    )
    FetchContent_MakeAvailable(dottorrent)
endif()

if(IS_DIRECTORY "${dottorrent_SOURCE_DIR}")
    set_property(DIRECTORY ${dottorrent_SOURCE_DIR} PROPERTY EXCLUDE_FROM_ALL YES)
endif()