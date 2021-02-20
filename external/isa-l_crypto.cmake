if (TARGET ISAL::Crypto)
    log_target_found(ISAL::Crypto)
    return()
endif()

find_package(ISALCrypto QUIET)
if (ISALCrypto_FOUND)
    log_module_found(ISALCrypto)
    return()
endif()

include(ExternalProject)
include(GNUInstallDirs)
find_program(MAKE_EXECUTABLE NAMES make nmake)

set(isal_crypto_install_dir "${CMAKE_CURRENT_BINARY_DIR}/_deps/isa-l_crypto-install")
set(isal_crypto_install_libdir "${isal_crypto_install_dir}/${CMAKE_INSTALL_LIBDIR}")


if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/isa-l_crypto)
    log_dir_found(isa-l_crypto)
    set(isal_source_dir "${CMAKE_CURRENT_LIST_DIR}/isa-l_crypto")

    ExternalProject_Add(build-isa-l_crypto
            SOURCE_DIR          ${isal_source_dir}
            BUILD_IN_SOURCE     ON
            UPDATE_DISCONNECTED ON
            CONFIGURE_COMMAND   "./autogen.sh"
            COMMAND             ./configure --prefix=${isal_crypto_install_dir} --libdir=${isal_crypto_install_libdir}
            BUILD_COMMAND       ${MAKE_EXECUTABLE} .
            INSTALL_COMMAND     ${MAKE_EXECUTABLE} install
            TEST_COMMAND        ""
            USES_TERMINAL_DOWNLOAD  YES
            USES_TERMINAL_UPDATE    YES
    )
else()
    log_fetch("isa-l_crypto")
    set(isal_source_dir "${CMAKE_CURRENT_BINARY_DIR}/_deps/isa-l_crypto-src")
    # Create include directory at configure time to make sure the INTERFACE_INCLUDE_DIRECTORY exists
    file(MAKE_DIRECTORY ${isal_source_dir}/include)

    ExternalProject_Add(build-isa-l_crypto
            GIT_REPOSITORY "https://www.github.com/intel/isa-l_crypto.git"
            GIT_TAG "master"
            SOURCE_DIR          ${isal_source_dir}
            BUILD_IN_SOURCE     ON
            CONFIGURE_COMMAND   "./autogen.sh"
            COMMAND             ./configure --prefix=${isal_crypto_install_dir} --libdir=${isal_crypto_install_libdir}
            BUILD_COMMAND       ${MAKE_EXECUTABLE} .
            INSTALL_COMMAND     ${MAKE_EXECUTABLE} install
            TEST_COMMAND        ""
            USES_TERMINAL_DOWNLOAD  YES
            USES_TERMINAL_UPDATE    YES
    )
endif()

add_library(ISAL::Crypto STATIC IMPORTED GLOBAL)
set_property(
    TARGET ISAL::Crypto
    PROPERTY IMPORTED_LOCATION ${isal_crypto_install_libdir}/libisal_crypto.a
)
set_property(
    TARGET ISAL::Crypto
    PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${isal_source_dir}/include"
)

add_dependencies(ISAL::Crypto build-isa-l_crypto)