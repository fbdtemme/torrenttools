include(FetchContent)

message(STATUS "Fetching dependency iRajul/bencode...")
FetchContent_Declare(
        irajul-bencode
        GIT_REPOSITORY https://github.com/iRajul/bencode.git
        GIT_TAG        master
)
FetchContent_MakeAvailable(irajul-bencode)
file(GLOB irajul-bencode_SOURCES "${irajul-bencode_SOURCE_DIR}/src/*.cxx")

add_library(irajul-bencode SHARED ${irajul-bencode_SOURCES})
target_include_directories(irajul-bencode PUBLIC ${irajul-bencode_SOURCE_DIR}/include)