
find_package(Catch2 QUIET)
if (Catch2_FOUND)
    message(STATUS "Local installation of Catch2 found.")
else()
    message(STATUS "Fetching dependency Catch2...")
    FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG        v2.x
    )
    FetchContent_MakeAvailable(Catch2)
    list(APPEND CMAKE_MODULE_PATH "${Catch2_SOURCE_DIR}/contrib")

    if(IS_DIRECTORY "${Catch2_SOURCE_DIR}")
        set_property(DIRECTORY ${Catch2_SOURCE_DIR} PROPERTY EXCLUDE_FROM_ALL YES)
    endif()
endif()