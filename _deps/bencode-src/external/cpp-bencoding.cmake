message(STATUS "Fetching dependency s3rvac/cpp-bencoding ...")
FetchContent_Declare(
        cpp-bencoding
        GIT_REPOSITORY https://github.com/s3rvac/cpp-bencoding.git
        GIT_TAG        master
)
FetchContent_MakeAvailable(cpp-bencoding)

# cpp-bencoding does not make a cmake 3 target available so define it here
add_library(cpp-bencoding STATIC)
target_include_directories(cpp-bencoding PUBLIC ${cpp-bencoding_SOURCE_DIR}/include)
file(GLOB cpp-bencoding_SOURCES "${cpp-bencoding_SOURCE_DIR}/src/*.cpp")
target_sources(cpp-bencoding PRIVATE ${cpp-bencoding_SOURCES})

file(READ "${cpp-bencoding_SOURCE_DIR}/include/Decoder.h" CPP_BENCODING_DECODING_HEADER)
string(REPLACE "#include <exception>" "#include <stdexcept>"
       CPP_BENCODING_DECODING_HEADER_FIXED
       "${CPP_BENCODING_DECODING_HEADER}")
file(WRITE "${cpp-bencoding_SOURCE_DIR}/include/Decoder.h" "${CPP_BENCODING_DECODING_HEADER_FIXED}")
