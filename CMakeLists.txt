cmake_minimum_required(VERSION 3.15)

# Version string
cmake_policy(SET CMP0048 NEW)

project(torrenttools
        DESCRIPTION "A toolbox for bittorrent metafiles."
        LANGUAGES CXX
        VERSION 0.1.0)

# Make Find modules in cmake dir available
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

option(TORRENTTOOLS_BUILD_TESTS "Build tests" ON)

include(CTest)
include(GNUInstallDirs)

add_subdirectory(../cliprogress cliprogress)
add_subdirectory(../termcontrol termcontrol)
add_subdirectory(../bencode bencode)
add_subdirectory(../dottorrent dottorrent)

include(external/external.cmake)

add_executable(torrenttools
    src/main.cpp
    src/info.cpp
    src/verify.cpp
    src/create.cpp
    src/argument_parsers.cpp
    src/config_parser.cpp
    src/tracker_database.cpp
    src/escape_binary_fields.cpp
)

target_include_directories(torrenttools PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(torrenttools PUBLIC cxx_std_20)

if (UNIX)
    set(global_data_dir ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/torrenttools)
    set(user_data_dir   $ENV{HOME}/.local/${CMAKE_INSTALL_DATADIR}/torrenttools)
elseif (WIN32)
    set(global_data_dir $ENV{HOMEDRIVE}/ProgramData/torrenttools)
    set(user_data_dir   $ENV{USERPROFILE}/AppData/torrenttools)
endif ()


configure_file(${CMAKE_SOURCE_DIR}/include/config.hpp.in ${CMAKE_SOURCE_DIR}/include/config.hpp)



target_link_libraries(torrenttools PUBLIC
        dottorrent::dottorrent
        termcontrol::termcontrol
        cliprogress::cliprogress
        CLI11::CLI11
        re2::re2
        gsl::gsl-lite-v1
        fmt::fmt
        yaml-cpp
)

if ((BUILD_TESTING AND TORRENTTOOLS_BUILD_TESTS) OR TORRENTTOOLS_BUILD_TESTS)
    add_subdirectory(tests)
endif()

# install config files to global data dir
install(FILES resources/config.yaml
              resources/trackers.json
        DESTINATION ${global_data_dir})

## install config files to user specific dir
#install(FILES resources/config.yaml
#              resources/trackers.json
#        DESTINATION ${user_data_dir})

# install executable
install(TARGETS torrenttools RUNTIME
        DESTINATION ${CMAKE_INSTALL_BINDIR})
