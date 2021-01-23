cmake_minimum_required(VERSION 3.15)

include(Catch)

add_executable(dottorrent-tests
        main.cpp
        metafile_parsing.cpp
        serialization.cpp
        test_announce_url_list.cpp
        test_decode.cpp
        test_merkle_tree.cpp
        test_metafile.cpp
        test_piece_hash.cpp
        test_storage_hasher.cpp
        test_checksum_hasher.cpp
        test_storage_verifier.cpp
        test_hashers.cpp
        test_hex.cpp
        test_file_storage.cpp
        test_file_attributes.cpp)


target_link_libraries(dottorrent-tests Catch2::Catch2 dottorrent)

target_include_directories(dottorrent-tests PRIVATE ../src)
target_compile_definitions(dottorrent-tests PRIVATE
        TEST_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\")

if (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
    find_program(HAS_LLD_LINKER "lld")
    if (HAS_LLD_LINKER)
        message(STATUS "Using lld linker for MinGW generator.")
        target_link_options(dottorrent-tests PRIVATE -fuse-ld=lld)
    endif()
endif()

catch_discover_tests(dottorrent-tests)


