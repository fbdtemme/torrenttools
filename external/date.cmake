if (TARGET date::date)
    log_target_found(date)
    return()
endif()

find_package(date QUIET)
if (date_FOUND)
    log_module_found(date)
    return()
endif()

set(ENABLE_DATE_TESTING OFF)

if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/date)
    log_dir_found(date)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/date)
    set(date_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/date)
else()
    log_fetch(date)
    FetchContent_Declare(
            date
            GIT_REPOSITORY   https://github.com/HowardHinnant/date.git
            GIT_TAG          master
    )
    FetchContent_MakeAvailable(date)
endif()

if(IS_DIRECTORY "${date_SOURCE_DIR}")
    set_property(DIRECTORY ${date_SOURCE_DIR} PROPERTY EXCLUDE_FROM_ALL YES)
endif()
