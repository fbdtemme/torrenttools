cmake_minimum_required(VERSION 3.15)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

project(dottorrent
        DESCRIPTION "A C++20 torrent metafile library"
        VERSION 0.1.0
        LANGUAGES CXX
        )

include(CTest)
include(SanitizersConfig)
include(GNUInstallDirs)

option(DOTTORRENT_BUILD_TESTS      "Build tests" ON)
option(DOTTORRENT_BUILD_COVERAGE   "Enable coverage flags" OFF)
set(DOTTORRENT_CRYPTO_LIB "openssl" CACHE STRING
                          "The cryptographic library to link agains. Options are: openssl libgcrypt, wincng")
option(DOTTORRENT_INSTALL "Generate an install target" ON)

# add cmake directory for Find* modules
cmake_policy(SET CMP0076 NEW)

add_library(dottorrent
        src/chunk_reader.cpp
        src/file_entry.cpp
        src/file_storage.cpp
        src/metafile.cpp
        src/metafile_parsing.cpp
        src/metafile_serialization.cpp
        src/storage_hasher.cpp
        src/v1_chunk_hasher.cpp
        src/v1_chunk_reader.cpp
        src/v1_chunk_verifier.cpp
        src/v1_checksum_hasher.cpp
        src/v2_chunk_reader.cpp
        src/v2_chunk_hasher.cpp
        src/v2_chunk_verifier.cpp
        src/v2_checksum_hasher.cpp
        src/hasher/backends/openssl.cpp
        src/hasher/backends/gcrypt.cpp
        src/hasher/backends/wincng.cpp
        src/storage_verifier.cpp
        )


add_library(dottorrent::dottorrent ALIAS dottorrent)

target_compile_features(dottorrent PUBLIC cxx_std_20)

if (WIN32)
    message(STATUS "Windows platform detected: Setting default crypto backend to WinCNG")
    set(DOTTORRENT_CRYPTO_LIB "WinCNG")
endif()

string(TOLOWER ${DOTTORRENT_CRYPTO_LIB} DOTTORRENT_CRYPTO_LIB)

if (DOTTORRENT_CRYPTO_LIB STREQUAL "openssl")
    find_package(OpenSSL REQUIRED)
    target_link_libraries(${PROJECT_NAME}      PUBLIC OpenSSL::Crypto)
    target_compile_definitions(${PROJECT_NAME} PUBLIC DOTTORRENT_USE_OPENSSL)
    message(STATUS "Using cryptographic library: OpenSSL")

elseif(DOTTORRENT_CRYPTO_LIB STREQUAL "libgcrypt")
    # TODO: Add FindModule for libgcrypt
elseif(DOTTORRENT_CRYPTO_LIB STREQUAL "wincng")
    target_link_libraries(${PROJECT_NAME}   PUBLIC bcrypt)
    target_compile_definitions(${PROJECT_NAME} PUBLIC DOTTORRENT_USE_WINCNG)
    message(STATUS "Using cryptographic library: WinCNG")
endif()

include(external/external.cmake)

find_package(Threads REQUIRED)

target_include_directories(dottorrent PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        )

set(dottorrent_public_include_dir  ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME})


target_link_libraries(dottorrent
    PUBLIC
        bencode::bencode
        fmt::fmt-header-only
        gsl::gsl-lite-v1
        Threads::Threads
        )

if (DOTTORRENT_BUILD_COVERAGE)
    message(STATUS "Building with coverage enabled")
    set(CMAKE_CXX_FLAGS --coverage)
    set(CMAKE_C_FLAGS --coverage)
endif()

if (DOTTORRENT_BUILD_TESTS)
    add_subdirectory(tests)
endif()

if (DOTTORRENT_INSTALL)
    set(dottorrent_cmake_install_dir          ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
    set(dottorrent_cmake_install_modules_dir  ${dottorrent_cmake_install_dir}/Modules)
    set(dottorrent_version_config             ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake)
    set(dottorrent_project_config             ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake)
    set(dottorrent_targets_export_name        ${PROJECT_NAME}-targets)
    set(dottorrent_targets_file               ${dottorrent_targets_export_name}.cmake)
    set(dottorrent_include_build_dir          ${PROJECT_SOURCE_DIR}/include/)

    include(CMakePackageConfigHelpers)

    write_basic_package_version_file(
            ${dottorrent_version_config}
            VERSION ${PACKAGE_VERSION}
            COMPATIBILITY AnyNewerVersion
    )
    configure_package_config_file(
            ${PROJECT_SOURCE_DIR}/cmake/dottorrent-config.cmake.in
            ${dottorrent_project_config}
            INSTALL_DESTINATION ${dottorrent_cmake_install_dir})


    # install project config file and config version file
    install(FILES ${dottorrent_project_config}
            ${dottorrent_version_config}
            DESTINATION ${dottorrent_cmake_install_dir}
            )

    # install public headers
    install(DIRECTORY ${dottorrent_public_include_dir} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    # install targets to an export set
    install(TARGETS dottorrent
            EXPORT   ${dottorrent_targets_export_name}
            INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR})

    # Install the export set to ena ble importing targets from the build tree
    export(EXPORT ${dottorrent_targets_export_name}
            FILE ${dottorrent_targets_file}
            NAMESPACE ${PROJECT_NAME}::)

    # Install the export set to enable importing targets from the install tree
    install(EXPORT ${dottorrent_targets_export_name}
            FILE ${dottorrent_targets_file}
            NAMESPACE ${PROJECT_NAME}::
            DESTINATION ${dottorrent_cmake_install_dir})
endif()
