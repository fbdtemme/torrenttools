
find_package(benchmark QUIET)
if (benchmark_FOUND)
    message(STATUS "Local installation of benchmark found.")
else()
    message(STATUS "Fetching dependency benchmark...")
    FetchContent_Declare(
            benchmark
            GIT_REPOSITORY https://github.com/google/benchmark.git
            GIT_TAG        master
    )
    set(BENCHMARK_ENABLE_TESTING OFF)
    set(BENCHMARK_ENABLE_INSTALL OFF)
    FetchContent_MakeAvailable(benchmark)

    if(IS_DIRECTORY "${benchmark_SOURCE_DIR}")
        set_property(DIRECTORY ${benchmark_SOURCE_DIR} PROPERTY EXCLUDE_FROM_ALL YES)
    endif()
endif()

