include(FetchContent)

find_package(fmt QUIET)
if (fmt_FOUND OR TARGET fmt::fmt-header-only)
    message(STATUS "Local installation of fmt found.")
else()
    message(STATUS "Fetching dependency fmt...")
    FetchContent_Declare(
            fmt
            GIT_REPOSITORY https://github.com/fmtlib/fmt.git
            GIT_TAG        master
    )
    #Need to keep this on to make imports from build tree work
    set(FMT_INSTALL ON)
    set(BUILD_SHARED_LIBS ON)
    set(FMT_TEST OFF)
    FetchContent_MakeAvailable(fmt)
endif()

