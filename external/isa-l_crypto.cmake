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

find_package(NASM REQUIRED)

set(isal_crypto_install_dir "${CMAKE_CURRENT_BINARY_DIR}/_deps/isa-l_crypto-install")
set(isal_crypto_build_dir "${CMAKE_CURRENT_BINARY_DIR}/_deps/isa-l_crypto-build")
set(isal_crypto_install_libdir "${isal_crypto_install_dir}/${CMAKE_INSTALL_LIBDIR}")


if (UNIX)
    if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/isa-l_crypto)
        log_dir_found(isa-l_crypto)
        set(isal_source_dir "${CMAKE_CURRENT_LIST_DIR}/isa-l_crypto")

        ExternalProject_Add(build-isa-l_crypto
                SOURCE_DIR          ${isal_source_dir}
                BUILD_IN_SOURCE     ON
                UPDATE_DISCONNECTED ON
                CONFIGURE_COMMAND   ./autogen.sh
                COMMAND             ./configure --prefix=${isal_crypto_install_dir} --libdir=${isal_crypto_install_libdir} CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER}
                BUILD_COMMAND       ${CMAKE_MAKE_PROGRAM} .
                INSTALL_COMMAND     ${CMAKE_MAKE_PROGRAM} install
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
                GIT_REPOSITORY "https://github.com/intel/isa-l_crypto.git"
                GIT_TAG "master"
                SOURCE_DIR          ${isal_source_dir}
                BUILD_IN_SOURCE     ON
                CONFIGURE_COMMAND   ./autogen.sh
                COMMAND             ./configure --prefix=${isal_crypto_install_dir} --libdir=${isal_crypto_install_libdir} CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER}
                BUILD_COMMAND       ${CMAKE_MAKE_PROGRAM} .
                INSTALL_COMMAND     ${CMAKE_MAKE_PROGRAM} install
                TEST_COMMAND        ""
                USES_TERMINAL_DOWNLOAD  YES
                USES_TERMINAL_UPDATE    YES
                )
    endif()
elseif (MINGW)
    # MINGW path
    # For some reason the autotools version does not work so we fallback to the Makefile
    if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL AMD64)
        set(host_cpu x86_64)
    else()
        set(host_cpu ${CMAKE_SYSTEM_PROCESSOR})
    endif()

    if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/isa-l_crypto)
        log_dir_found(isa-l_crypto)
        set(isal_source_dir "${CMAKE_CURRENT_LIST_DIR}/isa-l_crypto")

        ExternalProject_Add(build-isa-l_crypto
                SOURCE_DIR          ${isal_source_dir}
                BUILD_IN_SOURCE     ON
                UPDATE_DISCONNECTED ON
                CONFIGURE_COMMAND   ""
                BUILD_COMMAND       ${CMAKE_MAKE_PROGRAM} -f ${isal_source_dir}/Makefile.unx arch=mingw host_cpu=${host_cpu} have_as_w_avx512= CC=gcc AS=yasm AR=ar STRIP=strip LDFLAGS= CFLAGS_mingw=-m64
                INSTALL_COMMAND     mkdir -p  ${isal_crypto_install_libdir}
                COMMAND             cp bin/isa-l_crypto.a ${isal_crypto_install_libdir}/libisal_crypto.a
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
                GIT_REPOSITORY     "https://github.com/intel/isa-l_crypto.git"
                GIT_TAG            "master"
                SOURCE_DIR          ${isal_source_dir}
                BUILD_IN_SOURCE     ON
                CONFIGURE_COMMAND   ""
                BUILD_COMMAND       ${CMAKE_MAKE_PROGRAM} -f ${isal_source_dir}/Makefile.unx arch=mingw host_cpu=${host_cpu} have_as_w_avx512= CC=gcc AS=NASM AR=ar STRIP=strip LDFLAGS= CFLAGS_mingw=-m64
                INSTALL_COMMAND     mkdir -p  ${isal_crypto_install_libdir}
                COMMAND             cp bin/isa-l_crypto.a ${isal_crypto_install_libdir}/libisal_crypto.a
                TEST_COMMAND        ""
                USES_TERMINAL_DOWNLOAD  YES
                USES_TERMINAL_UPDATE    YES
                )
    endif()
elseif (WIN32 AND NOT MINGW)
        # MINGW path
        # For some reason the autotools version does not work so we fallback to the Makefile
        if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL AMD64)
            set(host_cpu x86_64)
        else()
            set(host_cpu ${CMAKE_SYSTEM_PROCESSOR})
        endif()

        if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/isa-l_crypto)
            log_dir_found(isa-l_crypto)
            set(isal_source_dir "${CMAKE_CURRENT_LIST_DIR}/isa-l_crypto")

            ExternalProject_Add(build-isa-l_crypto
                    SOURCE_DIR          ${isal_source_dir}
                    BUILD_IN_SOURCE     OFF
                    UPDATE_DISCONNECTED ON
                    CONFIGURE_COMMAND   ${CMAKE_COMMAND} -E copy_directory ${isal_source_dir} ${isal_crypto_build_dir}
                    BUILD_COMMAND       ${CMAKE_COMMAND} -E chdir ${isal_crypto_build_dir} ${CMAKE_MAKE_PROGRAM} -f ${isal_crypto_build_dir}/Makefile.nmake NASM=${CMAKE_NASM_EXECUTABLE}
                    INSTALL_COMMAND     ${CMAKE_COMMAND} -E make_directory "${isal_crypto_install_libdir}"
                    COMMAND             ${CMAKE_COMMAND} -E copy ${isal_crypto_build_dir}/isa-l_crypto.lib ${isal_crypto_install_libdir}/
                    COMMAND             ${CMAKE_COMMAND} -E rename ${isal_crypto_install_libdir}/isa-l_crypto.lib ${isal_crypto_install_libdir}/libisal_crypto.lib
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
                    GIT_REPOSITORY     "https://github.com/intel/isa-l_crypto.git"
                    GIT_TAG            "master"
                    SOURCE_DIR          ${isal_source_dir}
                    BUILD_IN_SOURCE     OFF
                    CONFIGURE_COMMAND   ${CMAKE_COMMAND} -E copy_directory ${isal_source_dir} ${isal_crypto_build_dir}
                    BUILD_COMMAND       ${CMAKE_COMMAND} -E chdir ${isal_crypto_build_dir} ${CMAKE_MAKE_PROGRAM} -f ${isal_crypto_build_dir}/Makefile.nmake NASM=${CMAKE_NASM_EXECUTABLE}
                    INSTALL_COMMAND     ${CMAKE_COMMAND} -E make_directory "${isal_crypto_install_libdir}"
                    COMMAND             ${CMAKE_COMMAND} -E copy ${isal_crypto_build_dir}/isa-l_crypto.lib ${isal_crypto_install_libdir}/
                    COMMAND             ${CMAKE_COMMAND} -E rename ${isal_crypto_install_libdir}/isa-l_crypto.lib ${isal_crypto_install_libdir}/libisal_crypto.lib
                    TEST_COMMAND        ""
                    USES_TERMINAL_DOWNLOAD  YES
                    USES_TERMINAL_UPDATE    YES
                    )
        endif()
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
