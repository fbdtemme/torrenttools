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
include(FetchContent)

find_package(NASM REQUIRED)


if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/isa-l_crypto)
    log_dir_found(isa-l_crypto)
    set(isa-l_crypto_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/isa-l_crypto)
else()
    log_fetch(isa-l_crypto)
    FetchContent_Declare(
            isa-l_crypto
            GIT_REPOSITORY https://github.com/intel/isa-l_crypto.git
            GIT_TAG        master
    )
    if (NOT isa-l_crypto-POPULATES)
        FetchContent_Populate(isa-l_crypto)
    endif()
endif()

if (UNIX AND NOT MINGW)
    ExternalProject_Add(compile-isa-l_crypto
            SOURCE_DIR          ${isa-l_crypto_SOURCE_DIR}
            BUILD_IN_SOURCE     ON
            UPDATE_DISCONNECTED ON
            CONFIGURE_COMMAND   ./autogen.sh
            COMMAND             ./configure --prefix=${isa-l_crypto_BINARY_DIR} \\
            --libdir=${isa-l_crypto_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR} \\
            CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER}
            BUILD_COMMAND       ${CMAKE_MAKE_PROGRAM} .
            INSTALL_COMMAND     ${CMAKE_MAKE_PROGRAM} install
            TEST_COMMAND        ""
            )
elseif (MINGW)
    # MINGW path
    # For some reason the autotools version does not work so we fallback to the Makefile
    if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL AMD64)
        set(host_cpu x86_64)
    else()
        set(host_cpu ${CMAKE_SYSTEM_PROCESSOR})
    endif()

    file(GLOB ISAL_CRYPTO_PUBLIC_HEADERS "${isa-l_crypto_SOURCE_DIR}/include/*.h")

    ExternalProject_Add(compile-isa-l_crypto
            SOURCE_DIR          ${isa-l_crypto_SOURCE_DIR}
            BUILD_IN_SOURCE     ON
            UPDATE_DISCONNECTED ON
            CONFIGURE_COMMAND   ""
            BUILD_COMMAND       ${CMAKE_MAKE_PROGRAM} -f ${isa-l_crypto_SOURCE_DIR}/Makefile.unx arch=mingw host_cpu=${host_cpu} have_as_w_avx512= CC=gcc AS=yasm AR=ar STRIP=strip LDFLAGS= CFLAGS_mingw=-m64
            COMMAND             ${CMAKE_MAKE_PROGRAM} -f ${isa-l_crypto_SOURCE_DIR}/Makefile.unx isa-l_crypto.h
            INSTALL_COMMAND     mkdir -p ${isa-l_crypto_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}
            COMMAND             mkdir -p ${isa-l_crypto_BINARY_DIR}/include/isa-l_crypto
            COMMAND             cp ${isa-l_crypto_SOURCE_DIR}/bin/isa-l_crypto.a ${isa-l_crypto_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}/libisal_crypto.a
            COMMAND             ${CMAKE_COMMAND} -E copy ${ISAL_CRYPTO_PUBLIC_HEADERS} ${isa-l_crypto_BINARY_DIR}/include/isa-l_crypto
            COMMAND             ${CMAKE_COMMAND} -E copy  ${isa-l_crypto_SOURCE_DIR}/isa-l_crypto.h ${isa-l_crypto_BINARY_DIR}/include
            TEST_COMMAND        ""
            )
elseif (WIN32 AND NOT MINGW)
    # MINGW path
    # For some reason the autotools version does not work so we fallback to the Makefile
    if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL AMD64)
        set(host_cpu x86_64)
    else()
        set(host_cpu ${CMAKE_SYSTEM_PROCESSOR})
    endif()

    ExternalProject_Add(compile-isa-l_crypto
            SOURCE_DIR          ${isa-l_crypto_SOURCE_DIR}
            BUILD_IN_SOURCE     OFF
            UPDATE_DISCONNECTED ON
            CONFIGURE_COMMAND   ${CMAKE_COMMAND} -E copy_directory ${isa-l_crypto_SOURCE_DIR} ${isa-l_crypto_BINARY_DIR}
            BUILD_COMMAND       ${CMAKE_COMMAND} -E chdir ${isa-l_crypto_BINARY_DIR} ${CMAKE_MAKE_PROGRAM} -f ${isa-l_crypto_BINARY_DIR}/Makefile.nmake NASM=${CMAKE_NASM_EXECUTABLE}
            INSTALL_COMMAND     ${CMAKE_COMMAND} -E make_directory "${isal_crypto_install_libdir}"
            COMMAND             ${CMAKE_COMMAND} -E copy ${isa-l_crypto_BINARY_DIR}/isa-l_crypto.lib ${isal_crypto_install_libdir}/
            COMMAND             ${CMAKE_COMMAND} -E rename ${isal_crypto_install_libdir}/isa-l_crypto.lib ${isal_crypto_install_libdir}/libisal_crypto.lib
            TEST_COMMAND        ""
            )
endif()

# Make sure include directory exists, it will be populated at the build stage
file(MAKE_DIRECTORY ${isa-l_crypto_BINARY_DIR}/include)
add_library(ISAL::Crypto STATIC IMPORTED GLOBAL)
set_target_properties(ISAL::Crypto PROPERTIES
        IMPORTED_LOCATION ${isa-l_crypto_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}/libisal_crypto.a
        INTERFACE_INCLUDE_DIRECTORIES "${isa-l_crypto_BINARY_DIR}/include"
        )
add_dependencies(ISAL::Crypto compile-isa-l_crypto)