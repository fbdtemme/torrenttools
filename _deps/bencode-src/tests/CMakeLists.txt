cmake_minimum_required(VERSION 3.14)

include(../external/catch2.cmake)

include(Catch)

add_executable(bencode-tests)
target_sources(bencode-tests
    PUBLIC
        main.cpp
        basic_bvalue/test_accessors.cpp
        basic_bvalue/test_conversion.cpp
        basic_bvalue/test_comparison.cpp
        basic_bvalue/test_constructor.cpp
        basic_bvalue/test_basic_bvalue.cpp
        basic_bvalue/test_events.cpp

        bview/test_accessors.cpp
        bview/test_conversion.cpp
        bview/test_integer_bview.cpp
        bview/test_string_bview.cpp
        bview/test_list_bview.cpp
        bview/test_dict_bview.cpp
        bview/test_bview.cpp

        parser/test_from_chars.cpp
        parser/test_from_iters.cpp
        parser/push_parser.cpp
        parser/descriptor_parser.cpp
#        parser/pull_parser.cpp
#        parser/test_from_chars_sse41.cpp

        test_concepts.cpp
        test_encoder.cpp
        test_itoa.cpp
        test_bencode_type.cpp
        test_connect.cpp
        test_bpointer.cpp

        traits/memory.cpp
        traits/span.cpp
)

target_link_libraries(bencode-tests PRIVATE
        bencode
        Catch2::Catch2
)

if (BENCODE_ENABLE_COVERAGE)
    message(STATUS "Building with coverage flags enabled")
    set(CMAKE_CXX_FLAGS --coverage)
    set(CMAKE_C_FLAGS --coverage)
endif()

target_compile_definitions(bencode-tests PRIVATE RESOURCES_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/resources\")

#############################################################################
# Test the generated build configs
#############################################################################

catch_discover_tests(bencode-tests)

add_subdirectory(cmake_import)
add_subdirectory(cmake_add_subdirectory)
add_subdirectory(cmake_fetch_content)
