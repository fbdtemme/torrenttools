cmake_minimum_required(VERSION 3.15)

# Version string
cmake_policy(SET CMP0048 NEW)

project(torrenttools
        DESCRIPTION "A commandline tool for creating, inspecting and modifying bittorrent metafiles."
        LANGUAGES CXX
        VERSION 0.1.2
        HOMEPAGE_URL https://www.github.com/fbdtemme/torrenttools)

# Make Find modules in cmake dir available
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

if (TORRENTTOOLS_PACKAGES_ONLY)
    include(packages/packages.cmake)
    return()
else()
    include(packages/packages.cmake)
endif()

include(CTest)
include(GNUInstallDirs)

# Load the sysconf install dir as set by the RPM .spec file
if(NOT DEFINED SYSCONF_INSTALL_DIR)
    set(SYSCONF_INSTALL_DIR ${CMAKE_INSTALL_FULL_SYSCONFDIR})
    message(DEBUG Setting SYSCONF_INSTALL_DIR: ${SYSCONF_INSTALL_DIR})
else()
    message(DEBUG SYSCONF_INSTALL_DIR: ${SYSCONF_INSTALL_DIR})
endif(NOT DEFINED SYSCONF_INSTALL_DIR)


option(TORRENTTOOLS_BUILD_TESTS "Build tests" OFF)
option(TORRENTTOOLS_INSTALL "Generate an install target" ON)
option(TORRENTTOOLS_BUILD_DOCS "Generate documentation" OFF)
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
    set(torrenttools_sysconf_dir        ${SYSCONF_INSTALL_DIR}/torrenttools)
    set(torrenttools_local_config_dir   $ENV{HOME}/.config/torrenttools)
elseif (WIN32)
    set(torrenttools_sysconf_dir        $ENV{HOMEDRIVE}\\ProgramData\\torrenttools)
    set(torrenttools_local_config_dir   $ENV{USERPROFILE}\\AppData\\torrenttools)
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

if(TORRENTTOOLS_BUILD_DOCS)
    add_subdirectory(docs)
endif()

if (TORRENTTOOLS_INSTALL)
    # install config files to global data dir
    install(FILES resources/config.yaml
                  resources/trackers.json
            COMPONENT torrenttools
            DESTINATION ${torrenttools_sysconf_dir})

    # install executable
    install(TARGETS torrenttools
            COMPONENT torrenttools
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()


set(CPACK_SOURCE_GENERATOR                   TGZ RPM)
set(CPACK_GENERATOR                          DEB)
set(CPACK_ARCHIVE_COMPONENT_INSTALL          ON)
set(CPACK_COMPONENTS_ALL                     torrenttools)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY        "${CMAKE_PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_FILE_NAME                  "${PROJECT_NAME}-${CMAKE_PROJECT_VERSION}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME           "${PROJECT_NAME}-${CMAKE_PROJECT_VERSION}")
set(CPACK_PACKAGE_NAME                       "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR                     "fbdtemme")
set(CPACK_PACKAGE_CONTACT                    "fbdtemme")
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


# DEBIAN Specific packagin
set(CPACK_DEB_COMPONENT_INSTALL                       ON)
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE                     "${CMAKE_PROJECT_HOMEPAGE_URL}")
set(CPACK_DEBIAN_PACKAGE_VERSION                      ${CPACK_PACKAGE_VERSION})
set(CPACK_DEBIAN_PACKAGE_RELEASE                      1)
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION                  ${PACKAGE_DESCRIPTION})
set(CPACK_DEBIAN_TORRENTTOOLS_PACKAGE_NAME            ${PACKAGE_NAME})
set(CPACK_DEBIAN_TORRENTTOOLS_FILE_NAME               "DEB-DEFAULT")
set(CPACK_DEBIAN_TORRENTTOOLS_PACKAGE_DEPENDS         "libtbb2, libssl1.1, libstdc++-10-pic, libc6")
set(CPACK_DEBIAN_TORRENTTOOLS_PACKAGE_SHLIBDEPS       ON)

# RPM SOURCE PACKAGE
set(CPACK_RPM_PACKAGE_RELEASE_DIST                                                OFF)

set(CPACK_RPM_USER_BINARY_SPECFILE                    ${CMAKE_CURRENT_SOURCE_DIR}/packages/rpm/torrenttools.spec)
set(CPACK_RPM_PACKAGE_RELEASE_DIST                    "OFF")
set(CPACK_RPM_FILE_NAME                               "${PACKAGE_SOURCE_PACKAGE_FILE_NAME}")

include(CPack)