include(FetchContent)

find_package(Qt5 COMPONENTS Core REQUIRED)

message(STATUS "Fetching dependency Aetf/QBencode...")
FetchContent_Declare(
        aetf-qbencode
        GIT_REPOSITORY https://github.com/Aetf/QBencode.git
        GIT_TAG        master
)

# Check if population has already been performed
FetchContent_GetProperties(aetf-qbencode)

if(NOT ${aetf-qbencode}_POPULATED)
    # Fetch the content using previously declared details
    FetchContent_Populate(aetf-qbencode)
    file(GLOB_RECURSE aetf-qbencode_SOURCES ${aetf-qbencode_SOURCE_DIR}/src/*.cpp)
    add_library(aetf-qbencode SHARED ${aetf-qbencode_SOURCES})
    target_include_directories(aetf-qbencode PUBLIC ${aetf-qbencode_SOURCE_DIR}/src/include)
    target_link_libraries(aetf-qbencode PUBLIC Qt5::Core)
endif()