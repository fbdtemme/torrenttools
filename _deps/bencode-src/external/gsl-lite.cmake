include(FetchContent)

find_package(gsl-lite QUIET)
if (gsl-lite_FOUND OR TARGET gsl::gsl-lite-v1)
    message(STATUS "Local installation of gsl-lite found.")
else()
    message(STATUS "Fetching dependency gsl-lite...")
    FetchContent_Declare(
            gsl-lite
            GIT_REPOSITORY https://github.com/gsl-lite/gsl-lite.git
            GIT_TAG        master
    )
    FetchContent_MakeAvailable(gsl-lite)
    if(IS_DIRECTORY "${gsl-lite_SOURCE_DIR}")
        set_property(DIRECTORY ${gsl-lite_SOURCE_DIR} PROPERTY EXCLUDE_FROM_ALL YES)
    endif()
endif()