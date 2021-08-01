#.rst:
# FindISALCrypto
# -----------
#
# Find the isal-l_crypto library.
#
# IMPORTED Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines :prop_tgt:`IMPORTED` targets:
#
# ``ISAL::Crypto``
#   The isa-l_crypto library, if found.
#
# Result variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables:
#
# ::
#
#   ISALCrypto_FOUND          - true if the headers and library were found
#   ISALCrypto_INCLUDE_DIRS   - where to find headers
#   ISALCrypto_LIBRARIES      - list of libraries to link
#   ISALCrypto_VERSION        - library version that was found, if any

# use pkg-config to get the directories and then use these values
# in the find_path() and find_library() calls
find_package(PkgConfig QUIET)
pkg_check_modules(PC_ISALCrypto QUIET isa-l_crypto)

# find the headers
find_path(ISALCrypto_INCLUDE_DIR
        NAMES isa-l_crypto.h
        HINTS
        ${PC_ISALCrypto_INCLUDEDIR}
        ${PC_ISALCrypto_INCLUDE_DIRS}
        PATH_SUFFIXES isa-l_crypto
        )

# find the library
find_library(ISALCrypto
        NAMES isal_crypto libisal_crypto
        HINTS
        ${PC_ISALCrypto_LIBDIR}
        ${PC_ISALCrypto_DIRS}
        )

# determine the version
if(PC_ISALCrypto_VERSION)
    set(ISALCrypto_VERSION ${PC_ISALCrypto_VERSION})
elseif(ISALCrypto_INCLUDE_DIR AND EXISTS "${ISALCrypto_INCLUDE_DIR}/isa-l_crypto.h")
    file(STRINGS "${ISALCrypto_INCLUDE_DIR}/isa-l_crypto.h" isal_crypto_version_str
            REGEX "^#define[\t ]+(ISALCrypto_VERSION_[A-Z]+)[\t ]+[0-9]+")

    string(REGEX REPLACE ".*#define[\t ]+ISAL_CRYPTO_VERSION_MAJOR[\t ]+([0-9]+).*"
            "\\1" _isal_crypto_version_major "${isal_crypto_version_str}")
    string(REGEX REPLACE ".*#define[\t ]+ISAL_CRYPTO_VERSION_MINOR[\t ]+([0-9]+).*"
            "\\1" _isal_crypto_version_minor "${isal_crypto_version_str}")
    string(REGEX REPLACE ".*#define[\t ]+ISAL_CRYPTO_VERSION_PATCH[\t ]+([0-9]+).*"
            "\\1" _isal_crypto_version_patch "${isal_crypto_version_str}")
    set(ISALCrypto_VERSION "${_isal_crypto_version_major}.${_isal_crypto_version_minor}.${_isal_crypto_version_patch}"
            CACHE INTERNAL "The version of ISAL_CRYPTO which was detected")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ISALCrypto
        REQUIRED_VARS ISALCrypto ISALCrypto_INCLUDE_DIR
        VERSION_VAR ISALCrypto_VERSION
        )

if (ISALCrypto_FOUND)
    set(ISALCrypto_INCLUDE_DIRS ${ISALCrypto_INCLUDE_DIR} ${PC_ISALCrypto_INCLUDE_DIRS})
    set(ISALCrypto_LIBRARIES ${ISALCrypto})
endif()

if (ISALCrypto_FOUND AND NOT TARGET ISAL::Crypto)
    # create the new library target
    add_library(ISAL::Crypto UNKNOWN IMPORTED)
    # set the required include dirs for the target
    if (ISALCrypto_INCLUDE_DIRS)
        set_target_properties(ISAL::Crypto
                PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${ISALCrypto_INCLUDE_DIRS}"
                )
    endif()
    # set the required libraries for the target
    if (EXISTS "${ISALCrypto}")
        set_target_properties(ISAL::Crypto
                PROPERTIES
                IMPORTED_LINK_INTERFACE_LANGUAGES "C"
                IMPORTED_LOCATION "${ISALCrypto}"
                )
    endif()
endif()

mark_as_advanced(ISALCrypto_INCLUDE_DIR ISALCrypto_LIBRARIES ISALCrypto)