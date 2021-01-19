
find_package(re2 QUIET)
if (re2_FOUND)
    log_found(re2)
else()
    # Silence re2 CMP0077 warnings
    set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
    set(RE2_BUILD_TESTING OFF)

    if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/re2)
        log_dir_found(re2)
        add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/re2)
        set(re2_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/re2)
    else()
        log_fetch(re2)
        include(FetchContent)
        FetchContent_Declare(
                re2
                GIT_REPOSITORY https://github.com/google/re2.git
                GIT_TAG        master
        )
        FetchContent_MakeAvailable(re2)
    endif()
endif()

if(IS_DIRECTORY "${re2_SOURCE_DIR}")
    set_property(DIRECTORY ${re2_SOURCE_DIR} PROPERTY EXCLUDE_FROM_ALL YES)
endif()