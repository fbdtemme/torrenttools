include(FetchContent)

find_package(expected-lite QUIET)
if (expected-lite_FOUND OR TARGET expected-lite)
    message(STATUS "Local installation of expected-lite found.")
else()
    message(STATUS "Fetching dependency expected-lite...")
    FetchContent_Declare(
            expected-lite
            GIT_REPOSITORY https://github.com/martinmoene/expected-lite.git
            GIT_TAG        master
    )
    FetchContent_MakeAvailable(expected-lite)
    if(IS_DIRECTORY "${expected-lite_SOURCE_DIR}")
        set_property(DIRECTORY ${expected-lite_SOURCE_DIR} PROPERTY EXCLUDE_FROM_ALL YES)
    endif()
endif()