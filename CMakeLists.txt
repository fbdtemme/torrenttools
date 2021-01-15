cmake_minimum_required(VERSION 3.15)

# Version string
cmake_policy(SET CMP0048 NEW)

project(torrenttools
        DESCRIPTION "A commandline tool for creating, inspecting and modifying bittorrent metafiles."
        LANGUAGES CXX
        VERSION 0.1.0
        HOMEPAGE_URL https://www.github.com/fbdtemme/torrenttools)

# Make Find modules in cmake dir available
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

option(TORRENTTOOLS_BUILD_TESTS "Build tests" OFF)
option(TORRENTTOOLS_INSTALL "Generate an install target" ON)

include(CTest)
include(GNUInstallDirs)
#
#add_subdirectory(../cliprogress cliprogress)
#add_subdirectory(../termcontrol termcontrol)
#add_subdirectory(../bencode bencode)
#add_subdirectory(../dottorrent dottorrent)

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


configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/config.hpp.in ${CMAKE_CURRENT_SOURCE_DIR}/include/config.hpp)

target_link_libraries(torrenttools PUBLIC
        dottorrent::dottorrent
        termcontrol::termcontrol
        cliprogress::cliprogress
        fmt::fmt
        gsl::gsl-lite-v1
        CLI11::CLI11
        re2::re2
        yaml-cpp
        nlohmann_json::nlohmann_json
)

if (TORRENTTOOLS_BUILD_TESTS)
    add_subdirectory(tests)
endif()

if (TORRENTTOOLS_INSTALL)
    # install config files to global data dir
    install(FILES resources/config.yaml
                  resources/trackers.json
            COMPONENT torrenttools
            DESTINATION ${global_data_dir})

    # install executable
    install(TARGETS torrenttools
            COMPONENT torrenttools
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()


set(CPACK_SOURCE_GENERATOR "TGZ")
#set(CPACK_GENERATOR "RPM")

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY        "${CMAKE_PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_FILE_NAME                  "${PROJECT_NAME}-${CMAKE_PROJECT_VERSION}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME           "${PROJECT_NAME}-${CMAKE_PROJECT_VERSION}.src")
set(CPACK_PACKAGE_NAME                       "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR                     "fbdtemme")
set(CPACK_PACKAGE_VERSION_MAJOR              "${CMAKE_PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR              "${CMAKE_PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH              "${CMAKE_PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION                    "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_BUILD_SOURCE_DIRS                  "${CMAKE_CURRENT_SOURCE_DIR}")

# Get the list of ignored files from .gitignore.
set(gitignore ${CMAKE_CURRENT_SOURCE_DIR}/.gitignore)
if (EXISTS ${gitignore})
    file (STRINGS ${gitignore} lines)
    list(REMOVE_ITEM lines /doc/html)
    foreach (line ${lines})
        string(REPLACE "." "[.]" line "${line}")
        string(REPLACE "*" ".*" line "${line}")
        set(ignored_files ${ignored_files} "${line}$" "${line}/")
    endforeach ()
endif()
list(APPEND ignored_files ".git.*")
list(APPEND CPACK_SOURCE_IGNORE_FILES        ${ignored_files})


set(CPACK_COMPONENTS_ALL                     "torrenttools")

# RPM
set(CPACK_RPM_COMPONENT_INSTALL               ON)
set(CPACK_RPM_DEBUGINFO_PACKAGE               ON)
set(CPACK_RPM_PACKAGE_RELEASE                 1)
set(CPACK_RPM_PACKAGE_LICENSE                 MIT)
set(CPACK_RPM_PACKAGE_NAME                    ${CPACK_PACKAGE_NAME})
set(CPACK_RPM_PACKAGE_VERSION                 ${CPACK_PACKAGE_VERSION})
set(CPACK_RPM_PACKAGE_VENDOR                  ${CPACK_PACKAGE_VENDOR})
set(CPACK_RPM_PACKAGE_SUMMARY                 "${CMAKE_PROJECT_DESCRIPTION}")
set(CPACK_RPM_PACKAGE_URL                     "${CMAKE_PROJECT_HOMEPAGE_URL}")

set(CPACK_RPM_torrenttools_FILE_NAME          "${CPACK_RPM_FILE_NAME}.rpm")
set(CPACK_RPM_torrenttools_PACKAGE_URL        "${CMAKE_PROJECT_HOMEPAGE_URL}")
set(CPACK_RPM_torrenttools_BINARY_SPECFILE    "${CMAKE_CURRENT_SOURCE_DIR}/packages/rpm/torrenttools.spec.in")


set(RPM_PACKAGE_BUILD_REQUIRES_LIST           "openssl-devel"
                                              "tbb-devel"
                                              "libstdc++-devel >= 10.0.0"
                                              "cmake"
                                              "re2-devel")
set(RPM_PACKAGE_REQUIRES_LIST                 "openssl >= 1.0.0"
                                              "tbb"
                                              "re2"
                                              "libstdc++ >= 10.0.0")

string(REPLACE ";" ", " RPM_PACKAGE_BUILD_REQUIRES  "${RPM_PACKAGE_BUILD_REQUIRES_LIST}")
string(REPLACE ";" ", " RPM_PACKAGE_REQUIRES        "${RPM_PACKAGE_REQUIRES_LIST}")

set(CPACK_RPM_SOURCE_PKG_BUILD_PARAMS   "-DBUILD_SHARED_LIBS=OFF")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/packages/rpm/torrenttools.spec.in
               ${CMAKE_CURRENT_SOURCE_DIR}/packages/rpm/torrenttools.spec
               @ONLY)

include(CPack)