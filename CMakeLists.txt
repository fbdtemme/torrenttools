cmake_minimum_required(VERSION 3.15)

# Version string
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0077 NEW)

if (UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

project(torrenttools
        DESCRIPTION "A commandline tool for creating, inspecting and modifying BitTorrent metafiles."
        LANGUAGES CXX
        VERSION 0.5.0
        HOMEPAGE_URL https://www.github.com/fbdtemme/torrenttools)

# Make Find modules in cmake dir available
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

if (APPLE)
    set(CMAKE_INSTALL_NAME_DIR "@executable_path/../lib")
endif()

if (TORRENTTOOLS_PACKAGES_ONLY)
    include(packages/packages.cmake)
    return()
endif()

include(CTest)
include(GNUInstallDirs)

# Path including images, config files etc
set(RESOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/resources")

## Load the sysconf install dir as set by the RPM .spec file
if(NOT DEFINED SYSCONF_INSTALL_DIR)
    set(SYSCONF_INSTALL_DIR ${CMAKE_INSTALL_SYSCONFDIR})
    message(DEBUG "Setting SYSCONF_INSTALL_DIR: ${SYSCONF_INSTALL_DIR}")
else()
    message(DEBUG "Setting SYSCONF_INSTALL_DIR: ${SYSCONF_INSTALL_DIR}")
endif()

include(CMakeDependentOption)
option(TORRENTTOOLS_BUILD_TESTS
       "Build tests" OFF)
cmake_dependent_option(TORRENTTOOLS_BUILD_TESTS_COVERAGE
                       "Build tests with coverage." OFF "TORRENTTOOLS_BUILD_TESTS" OFF)
option(TORRENTTOOLS_BUILD_DOCS
        "Generate documentation" OFF)
option(TORRENTTOOLS_INSTALL
        "Generate an install target" ON)

option(TORRENTTOOLS_TBB "Accelerate using Intel TBB library." ON)

#add_subdirectory(../cliprogress cliprogress)
#add_subdirectory(../dottorrent dottorrent)
#add_subdirectory(../termcontrol termcontrol)
#add_subdirectory(../bencode bencode)

include(external/external.cmake)
if (TORRENTTOOLS_TBB)
    find_package(TBB REQUIRED)
endif()

add_executable(torrenttools 
        src/app_data.cpp
        src/argument_parsers.cpp
        src/common.cpp
        src/config_parser.cpp
        src/create.cpp
        src/main_app.cpp
        src/edit.cpp
        src/escape_binary_fields.cpp
        src/formatters.cpp
        src/indicator.cpp
        src/info.cpp
        src/magnet.cpp
        src/main.cpp
        src/pad.cpp
        src/progress.cpp
        src/show.cpp
        src/tracker_database.cpp
        src/tree_view.cpp
        src/verify.cpp
        include/profile_config_formatter.hpp src/profile.cpp include/exceptions.hpp)

target_include_directories(torrenttools PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(torrenttools PUBLIC cxx_std_20)

target_link_libraries(torrenttools PRIVATE
        dottorrent::dottorrent
        termcontrol::termcontrol
        cliprogress::cliprogress
        fmt::fmt
        date::date
        gsl::gsl-lite-v1
        CLI11::CLI11
        re2::re2
        yaml-cpp
        nlohmann_json::nlohmann_json
    )

if (TORRENTTOOLS_TBB)
    message(STATUS "Using Intel TBB library.")
    target_link_libraries(torrenttools PRIVATE TBB::tbb)
    target_compile_definitions(torrenttools PRIVATE TORRENTTOOLS_USE_TBB)
endif()

# Set the linker to lld to get decent link times on MinGW
if (MINGW)
    find_program(HAS_LLD_LINKER "lld")
    if (HAS_LLD_LINKER)
        message(STATUS "Using lld linker for MinGW generator.")
        target_link_options(torrenttools PRIVATE -fuse-ld=lld)
    endif()
endif()

if (TORRENTTOOLS_BUILD_TESTS)
    add_subdirectory(tests)
endif()

if(TORRENTTOOLS_BUILD_DOCS)
    add_subdirectory(docs)
endif()



# Set the install prefix to /usr/local/bin/torrenttools
message(DEBUG ${CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT})

if (MINGW OR WIN32)
    # Set the install prefix to Program Files instead of Program Files (x86)
    message(DEBUG ${CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT})
    if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        message(DEBUG "Changing default install prefix")
        SET(CMAKE_INSTALL_PREFIX  "C:/Program Files/${PROJECT_NAME}" CACHE PATH
                "Install path prefix, prepended onto install directories."  FORCE)
    endif()
elseif (APPLE)
    if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        message(DEBUG "Changing default install prefix")
        set(CMAKE_INSTALL_PREFIX  "/Library/${PROJECT_NAME}" CACHE PATH
                "Install path prefix, prepended onto install directories."  FORCE)
    endif()
endif()


if (LINUX)
    set(torrenttools_sysconf_dir         "${SYSCONF_INSTALL_DIR}/torrenttools")
    set(torrenttools_install_sysconf_dir "${CMAKE_INSTALL_PREFIX}/${torrenttools_sysconf_dir}")
elseif (WIN32)
    # Dump everything in the install prefix on windows
    # We need full paths
    set(torrenttools_sysconf_dir         ".")
    set(torrenttools_install_sysconf_dir "${CMAKE_INSTALL_PREFIX}")
elseif (APPLE)
    set(torrenttools_sysconf_dir         "etc")
    set(torrenttools_install_sysconf_dir "${CMAKE_INSTALL_PREFIX}/${torrenttools_sysconf_dir}")
<<<<<<< HEAD
endif ()
=======
endif()


>>>>>>> d6a13d5295f369f1a7abb1b5e00f5dcb7c41ea46



configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/config.hpp.in ${CMAKE_CURRENT_SOURCE_DIR}/include/config.hpp)


if (TORRENTTOOLS_INSTALL)
    if (LINUX)
        # install trackers to system config dir since it is global data.
        install(FILES resources/trackers.json
                resources/config.yml
                COMPONENT torrenttools
                DESTINATION ${torrenttools_sysconf_dir})

        # install executable
        install(TARGETS torrenttools
                COMPONENT torrenttools
                RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
    endif()

    if (MINGW OR WIN32)
        # Set the install prefix to Program Files instead of Program Files (x86)
        message(DEBUG ${CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT})
        if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
            message(DEBUG "Changing default install prefix")
            SET(CMAKE_INSTALL_PREFIX  "C:/Program Files/${PROJECT_NAME}" CACHE PATH
                "Install path prefix, prepended onto install directories."  FORCE)
        endif()

        # install resources in the install prefix
        install(FILES resources/trackers.json
                resources/config.yml
                COMPONENT torrenttools
                DESTINATION ".")

        # install executable
        install(TARGETS torrenttools
                COMPONENT torrenttools
                RUNTIME DESTINATION ".")

        # Install mingw shared libraries to the project dir
        # Filter out core windows libraries with regex

        install(CODE [[
            set(LIBRARY_PATH $ENV{PATH})
            string(REPLACE "\\" "/" LIBRARY_PATH "${LIBRARY_PATH}")
            message(STATUS "Searching for libraries in: ${LIBRARY_PATH}")

            file(GET_RUNTIME_DEPENDENCIES
                EXECUTABLES $<TARGET_FILE:torrenttools>
                DIRECTORIES ${LIBRARY_PATH}
                RESOLVED_DEPENDENCIES_VAR _r_deps
                UNRESOLVED_DEPENDENCIES_VAR _u_deps
                PRE_EXCLUDE_REGEXES ".*-ms-win-.*" ".*ext-ms-.*"
                POST_EXCLUDE_REGEXES ".*system32.*"
            )
            foreach(_file ${_r_deps})
                file(INSTALL ${_file}
                     DESTINATION "${CMAKE_INSTALL_PREFIX}"
                     FOLLOW_SYMLINK_CHAIN)
            endforeach()

            list(LENGTH _u_deps _u_length)
            if("${_u_length}" GREATER 0)
                message(STATUS "${_u_deps}")
                message(WARNING "Unresolved dependencies detected!")

                foreach(_u_dep ${_u_deps}) 
                    message(WARNING "Dependency unresolved: ${_u_dep}")
                endforeach()
            endif()
        ]]
        COMPONENT torrenttools)
    endif()

    if (APPLE)
        # Set the install prefix to /usr/local/bin/torrenttools
        message(DEBUG ${CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT})
        if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
            message(DEBUG "Changing default install prefix")
            set(CMAKE_INSTALL_PREFIX  "/Library/${PROJECT_NAME}" CACHE PATH
                "Install path prefix, prepended onto install directories."  FORCE)
        endif()

        # install executable
        install(TARGETS torrenttools
                COMPONENT torrenttools
                DESTINATION "bin")

        # install configuration
        install(FILES resources/trackers.json
                resources/config.yml
                COMPONENT torrenttools
                DESTINATION "etc")

        install(CODE [[
            file(GET_RUNTIME_DEPENDENCIES
                EXECUTABLES $<TARGET_FILE:torrenttools>
                DIRECTORIES "/usr/bin/local/Cellar" "/usr/bin/local"
                RESOLVED_DEPENDENCIES_VAR _r_deps
                UNRESOLVED_DEPENDENCIES_VAR _u_deps
            )

            foreach(_file ${_r_deps})
                file(INSTALL ${_file}
                    DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
                    FOLLOW_SYMLINK_CHAIN)
                get_filename_component(_file_name "${_file}" NAME) 
                execute_process(COMMAND install_name_tool -change "${_file}" "@executable_path/../lib/${_file_name}" "${CMAKE_INSTALL_PREFIX}/bin/torrenttools" )
            endforeach()

            list(LENGTH _u_deps _u_length)
            if("${_u_length}" GREATER 0)
                message(STATUS "${_u_deps}")
                message(WARNING "Unresolved dependencies detected!")

                foreach(_u_dep ${_u_deps}) 
                    message(WARNING "Dependency unresolved: ${_u_dep}")
                endforeach()
            endif()
        ]]
        COMPONENT torrenttools)

    endif()
endif()


include(packages/packages.cmake)