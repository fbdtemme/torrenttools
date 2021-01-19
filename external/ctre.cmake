
find_package(ctre QUIET)
if (ctre_FOUND OR TARGET ctre::ctre)
    message(STATUS "Local installation of dependency ctre found.")
else()
    set(CTRE_BUILD_TESTS OFF)
    set(CTRE_BUILD_PACAKGES OFF)

    if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/ctre)
        log_dir_found(ctre)
        add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/ctre)
        set(ctre_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/ctre)
    else()
        message(STATUS "Fetching dependency ctre ...")
        FetchContent_Declare(
                ctre
                GIT_REPOSITORY https://github.com/hanickadot/compile-time-regular-expressions.git
                GIT_TAG        master
        )
        FetchContent_MakeAvailable(ctre)
    endif()
endif()

if(IS_DIRECTORY "${ctre_SOURCE_DIR}")
    set_property(DIRECTORY ${ctre_SOURCE_DIR} PROPERTY EXCLUDE_FROM_ALL YES)
endif()