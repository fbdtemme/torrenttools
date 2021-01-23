include(FetchContent)

message(STATUS "Fetching dependency jimporter/bencode...")
FetchContent_Declare(
        jimporter-bencode
        GIT_REPOSITORY https://github.com/jimporter/bencode.hpp.git
        GIT_TAG        master
)
FetchContent_MakeAvailable(jimporter-bencode)
add_library(jimporter-bencode INTERFACE)
file(COPY "${jimporter-bencode_SOURCE_DIR}/include/bencode.hpp"
     DESTINATION "${jimporter-bencode_SOURCE_DIR}/include/jimporter")
target_include_directories(jimporter-bencode INTERFACE "${jimporter-bencode_SOURCE_DIR}/include")
