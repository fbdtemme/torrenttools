if (TARGET termcontrol::termcontrol)
    log_target_found(termcontrol)
    return()
endif()

find_package(termcontrol QUIET)
if (termcontrol_FOUND)
    log_module_found(termcontrol)
    return()
endif()


set(TERMCONTROL_BUILD_TESTS OFF)
set(TERMCONTROL_INSTALL OFF)

if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/termcontrol)
    log_dir_found(termcontrol)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/termcontrol)
    set(termcontrol_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/termcontrol)
else()
    log_fetch(termcontrol)
    FetchContent_Declare(
            termcontrol
            GIT_REPOSITORY   https://github.com/fbdtemme/termcontrol.git
            GIT_TAG          main
    )
    FetchContent_MakeAvailable(termcontrol)
endif()

if(IS_DIRECTORY "${termcontrol_SOURCE_DIR}")
    set_property(DIRECTORY ${termcontrol_SOURCE_DIR} PROPERTY EXCLUDE_FROM_ALL YES)
endif()
