include(FetchContent)

message(STATUS "Fetching dependency s3ponia/BencodeParser...")
FetchContent_Declare(
        s3ponia-bencodeparser
        GIT_REPOSITORY https://github.com/s3ponia/BencodeParser.git
        GIT_TAG        master
)
FetchContent_MakeAvailable(s3ponia-bencodeparser)

add_library(s3ponia-bencodeparser INTERFACE)
target_include_directories(s3ponia-bencodeparser INTERFACE ${s3ponia-bencodeparser_SOURCE_DIR})
