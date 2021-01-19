
find_package(Catch2 QUIET)

if (Catch2_FOUND)
    log_found(Catch2)
else()
    if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/Catch2)
        log_dir_found(Catch2)
        add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Catch2)
        set(Catch2_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/Catch2)
    else()
        log_fetch(Catch2)
        FetchContent_Declare(
                Catch2
                GIT_REPOSITORY https://github.com/catchorg/Catch2.git
                GIT_TAG        v2.x
        )
        FetchContent_MakeAvailable(Catch2)
        set(CMAKE_MODULE_PATH "${Catch2_SOURCE_DIR}/contrib" ${CMAKE_MODULE_PATH})
    endif()
endif()

if(IS_DIRECTORY "${Catch2_SOURCE_DIR}")
    set_property(DIRECTORY ${Catch2_SOURCE_DIR} PROPERTY EXCLUDE_FROM_ALL YES)
endif()

