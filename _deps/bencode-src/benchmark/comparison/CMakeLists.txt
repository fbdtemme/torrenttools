cmake_minimum_required(VERSION 3.15)

project("bencode-comparision-benchmark")

find_package(Boost REQUIRED COMPONENTS regex)

include(../../external/arvidn-libtorrent.cmake)
include(../../external/benchmark.cmake)
include(../../external/cpp-bencoding.cmake)
include(../../external/jimporter-bencode.cmake)
include(../../external/rakshasa-libtorrent.cmake)
include(../../external/kriben-bencode.cmake)
include(../../external/theanti9-cppbencode.cmake)
include(../../external/outputenable-bencode.cmake)
include(../../external/aetf-qbencode.cmake)
include(../../external/irajul-bencode.cmake)
include(../../external/s3ponia-bencodeparser.cmake)

add_executable(benchmark-fbdtemme-bencode           fbdtemme_bencode.cpp)
add_executable(benchmark-jimporter-bencode          jimporter_bencode.cpp)
add_executable(benchmark-jimporter-bencode-boost    jimporter_bencode_boost.cpp)
add_executable(benchmark-arvidn-libtorrent          arvidn_libtorrent.cpp)
add_executable(benchmark-s3rvac-cpp-bencoding       s3rvac_cpp_bencoding.cpp)
add_executable(benchmark-rakshasa-libtorrent        rakshasa_libtorrent.cpp)
add_executable(benchmark-kriben-bencode             kriben_bencode.cpp)
add_executable(benchmark-theanti9-cppbencode        theanti9_cppbencode.cpp)
add_executable(benchmark-outputenable-bencode       outputenable_bencode.cpp)
add_executable(benchmark-aetf-qbencode              aetf_qbencode.cpp)
add_executable(benchmark-irajul-bencode             irajul_bencode.cpp)
add_executable(benchmark-s3ponia-bencodeparser      s3ponia_bencodeparser.cpp)

list(APPEND ALL_BENCHMARK_TARGETS benchmark-fbdtemme-bencode
                                  benchmark-jimporter-bencode
                                  benchmark-jimporter-bencode-boost
                                  benchmark-arvidn-libtorrent
                                  benchmark-s3rvac-cpp-bencoding
                                  benchmark-rakshasa-libtorrent
                                  benchmark-kriben-bencode
                                  benchmark-theanti9-cppbencode
                                  benchmark-outputenable-bencode
                                  benchmark-aetf-qbencode
                                  benchmark-irajul-bencode
                                  benchmark-s3ponia-bencodeparser)

foreach(target IN LISTS ALL_BENCHMARK_TARGETS)
    target_link_libraries(${target} PRIVATE
            benchmark::benchmark
            benchmark::benchmark_main
    )
    target_compile_features(${target} PUBLIC cxx_std_20)
    target_compile_definitions(${target} PRIVATE RESOURCES_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/../resources\")

endforeach(target)

target_link_libraries(benchmark-fbdtemme-bencode        PRIVATE bencode::bencode)
target_link_libraries(benchmark-jimporter-bencode       PRIVATE jimporter-bencode)
target_link_libraries(benchmark-jimporter-bencode-boost PRIVATE jimporter-bencode Boost::headers)
target_link_libraries(benchmark-arvidn-libtorrent       PRIVATE torrent-rasterbar)
target_link_libraries(benchmark-s3rvac-cpp-bencoding    PRIVATE cpp-bencoding)
target_link_libraries(benchmark-rakshasa-libtorrent     PRIVATE rakshasa-libtorrent)
target_link_libraries(benchmark-kriben-bencode          PRIVATE kriben-bencode Boost::headers Boost::regex)
target_link_libraries(benchmark-theanti9-cppbencode     PRIVATE theanti9-cppbencode)
target_link_libraries(benchmark-outputenable-bencode    PRIVATE outputenable-bencode)
target_link_libraries(benchmark-aetf-qbencode           PRIVATE aetf-qbencode)
target_link_libraries(benchmark-irajul-bencode          PRIVATE irajul-bencode)
target_link_libraries(benchmark-s3ponia-bencodeparser   PRIVATE s3ponia-bencodeparser)

add_custom_target(benchmark-all)
add_dependencies(benchmark-all ${ALL_BENCHMARK_TARGETS})


