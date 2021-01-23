cmake_minimum_required(VERSION 3.14)
project(bencode
        DESCRIPTION "A C++20 header-only bencode library."
        HOMEPAGE_URL https://github.com/fbdtemme/bencode
        LANGUAGES CXX
        VERSION 0.5.0)

set(PROJECT_AUTHOR "fbdtemme")
# Make Find modules in cmake dir available
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# includes
include(CTest)
include(CMakeDependentOption)
include(GNUInstallDirs)
include(SanitizersConfig)

# Determine if bencode is built as a subproject (using add_subdirectory)
set(BENCODE_MASTER_PROJECT FALSE)
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(BENCODE_MASTER_PROJECT TRUE)
endif()


add_library(bencode INTERFACE)
add_library(bencode::bencode ALIAS bencode)

target_compile_features(bencode INTERFACE cxx_std_20)

target_include_directories(bencode INTERFACE
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        )

include(external/fmt.cmake)
include(external/gsl-lite.cmake)
include(external/expected-lite.cmake)

option(BENCODE_BUILD_TESTS       "Build tests."          ${BENCODE_MASTER_PROJECT})
    option(BENCODE_BUILD_BENCHMARKS  "Build benchmarks"                        OFF)
option(BENCODE_BUILD_DOCS        "Build documentation"                         OFF)
option(BENCODE_ENABLE_COVERAGE   "Build tests with coverage flags enabled"     OFF)
option(BENCODE_ENABLE_INSTALL    "Generate an install target."                  ON)

# Changing these options from their defaults can lead to reduced performance.
# Benchmark before changing these on your target system!

set(BENCODE_FROM_CHARS_INTEGER_IMPL "swar" CACHE STRING
        "The implementation to use. Options: serial, swar or sse41, avx2")
set(BENCODE_FROM_CHARS_STRING_IMPL "serial" CACHE STRING
        "The implementation to use. Options: serial, swar, sse41, avx2")


string(TOLOWER ${BENCODE_FROM_CHARS_INTEGER_IMPL} BENCODE_FROM_CHARS_INTEGER_IMPL)
string(TOLOWER ${BENCODE_FROM_CHARS_STRING_IMPL}  BENCODE_FROM_CHARS_STRING_IMPL)

if (BENCODE_FROM_CHARS_INTEGER_IMPL STREQUAL swar)
    message(STATUS "Enabling SWAR integer parsing.")
elseif(BENCODE_FROM_CHARS_INTEGER_IMPL STREQUAL sse41)
    message(STATUS "Enabling SSE4.1 integer parsing.")
    target_compile_options(bencode INTERFACE -msse4)
elseif(BENCODE_FROM_CHARS_INTEGER_IMPL STREQUAL avx2)
    message(STATUS "Enabling AVX2 integer parsing.")
    target_compile_options(bencode INTERFACE -mavx2)
endif()

if (BENCODE_FROM_CHARS_STRING_IMPL STREQUAL swar)
    message(STATUS "Enabling SWAR string parsing.")
elseif(BENCODE_FROM_CHARS_STRING_IMPL STREQUAL sse41)
    message(STATUS "Enabling SSE4.1 string parsing.")
    target_compile_options(bencode INTERFACE -msse4)
elseif(BENCODE_FROM_CHARS_STRING_IMPL STREQUAL avx2)
    message(STATUS "Enabling AVX2 string parsing.")
    target_compile_options(bencode INTERFACE -mavx2)
endif()


target_compile_definitions(
        bencode INTERFACE
        BENCODE_FROM_CHARS_INTEGER_IMPL=${BENCODE_FROM_CHARS_INTEGER_IMPL}
        BENCODE_FROM_CHARS_STRING_IMPL=${BENCODE_FROM_CHARS_STRING_IMPL}
)

target_link_libraries(bencode
    INTERFACE
        fmt::fmt-header-only
        nonstd::expected-lite
        gsl::gsl-lite-v1)

string(TOLOWER ${CMAKE_BUILD_TYPE} BENCODE_BUILD_TYPE)

if (BENCODE_BUILD_TYPE STREQUAL release)
    message(STATUS "Disabling all runtime checking of contracts")
    target_compile_definitions(bencode INTERFACE gsl_CONFIG_CONTRACT_CHECKING_OFF)
endif()


if (BENCODE_BUILD_TESTS)
    enable_testing()
    message(STATUS "Building tests enabled")
    add_subdirectory(tests)
endif()

if (BENCODE_BUILD_BENCHMARKS)
    message(STATUS "Building benchmark enabled")
    add_subdirectory(benchmark)
endif()

if (BENCODE_BUILD_DOCS)
    message(STATUS "Building docs enabled")
    add_subdirectory(docs)
endif()



if (BENCODE_ENABLE_INSTALL)
    set(bencode_package_name        ${PROJECT_NAME})
    set(bencode_cmake_install_dir   ${CMAKE_INSTALL_LIBDIR}/cmake/bencode)
    set(bencode_version_config      ${PROJECT_BINARY_DIR}/bencode-config-version.cmake)
    set(bencode_project_config      ${PROJECT_BINARY_DIR}/bencode-config.cmake)
    set(bencode_targets_export_name bencode-targets)
    set(bencode_targets_file        ${bencode_targets_export_name}.cmake)
    set(bencode_include_build_dir   ${PROJECT_SOURCE_DIR}/include/)

    include(CMakePackageConfigHelpers)

    write_basic_package_version_file(
            ${bencode_version_config}
            VERSION ${PACKAGE_VERSION}
            COMPATIBILITY AnyNewerVersion
    )
    configure_package_config_file(
            ${PROJECT_SOURCE_DIR}/cmake/bencode-config.cmake.in
            ${bencode_project_config}
            INSTALL_DESTINATION ${bencode_cmake_install_dir})


    # install headers
    install(DIRECTORY ${bencode_include_build_dir}
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
    # install project config file and config version file
    install(FILES ${bencode_project_config}
                  ${bencode_version_config}
            DESTINATION ${bencode_cmake_install_dir}
    )

    # install targets to an export set
    install(TARGETS bencode
            EXPORT ${bencode_targets_export_name}
            INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    # Install the export set to enable importing targets from the build tree
    export(EXPORT ${bencode_targets_export_name}
            FILE ${bencode_targets_file}
            NAMESPACE ${PROJECT_NAME}::)

    # Install the export set to enable importing targets from the install tree
    install(EXPORT ${bencode_targets_export_name}
            FILE ${bencode_targets_file}
            NAMESPACE ${PROJECT_NAME}::
            DESTINATION ${bencode_cmake_install_dir})

endif()