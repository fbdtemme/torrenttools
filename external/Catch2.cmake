if (TARGET Catch2::Catch2)
    log_target_found(Catch2)
    return()
endif()

find_package(Catch2 QUIET)
if (Catch2_FOUND)
    log_module_found(Catch2)
    return()
endif()

if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/Catch2)
    log_dir_found(Catch2)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Catch2)
    set(catch2_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/Catch2)
else()
    log_fetch(Catch2)
    FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG        v2.x
    )
    FetchContent_MakeAvailable(Catch2)
endif()

if(IS_DIRECTORY "${catch2_SOURCE_DIR}")
    set_property(DIRECTORY ${catch2_SOURCE_DIR} PROPERTY EXCLUDE_FROM_ALL YES)
    list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/contrib")

    get_directory_property(has_parent PARENT_DIRECTORY)
    if(has_parent)
        set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} PARENT_SCOPE)
    endif()
endif()

