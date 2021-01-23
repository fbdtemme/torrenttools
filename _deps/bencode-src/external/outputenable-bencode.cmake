include(FetchContent)

message(STATUS "Fetching dependency outputenable/bencode...")

# outputenable-bencode needs an old version of the microsoft GSL
FetchContent_Declare(
        microsoft-gsl
        GIT_REPOSITORY https://github.com/microsoft/GSL.git
        GIT_TAG        v2.1.0
)
FetchContent_MakeAvailable(microsoft-gsl)

FetchContent_Declare(
        outputenable-chopsuey
        GIT_REPOSITORY https://gitlab.com/outputenable/chopsuey.git
        GIT_TAG        master
)
set(GSUPPL_ROOT ${microsoft-gsl_SOURCE_DIR})
set(BUILD_HEADER_ONLY ON)
set(ENABLE_TESTING OFF)
FetchContent_MakeAvailable(outputenable-chopsuey)
if(IS_DIRECTORY "${outputenable-chopsuey_SOURCE_DIR}")
    set_property(DIRECTORY ${outputenable-chopsuey_SOURCE_DIR} PROPERTY EXCLUDE_FROM_ALL YES)
endif()
unset(HEADER_ONLY)
unset(ENABLE_TESTING)

FetchContent_Declare(
        outputenable-bencode
        GIT_REPOSITORY https://gitlab.com/outputenable/bencode.git
        GIT_TAG        master
        EXCLUDE_FROM_ALL TRUE
)
set(CHOPSUEY_ROOT ${outputenable-chopsuey_BINARY_DIR})

# Check if population has already been performed
FetchContent_GetProperties(outputenable-bencode)

if(NOT ${outputenable-bencode}_POPULATED)
    # Fetch the content using previously declared details
    FetchContent_Populate(outputenable-bencode)
    add_library(outputenable-bencode INTERFACE)
    target_include_directories(outputenable-bencode INTERFACE ${outputenable-bencode_SOURCE_DIR}/include)
    target_link_libraries (outputenable-bencode INTERFACE
            chopsuey::header_only
            GSL)
endif()

