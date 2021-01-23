include(FetchContent)

message(STATUS "Fetching dependency rakshasa/libtorrent...")
FetchContent_Declare(
        rakshasa-libtorrent
        GIT_REPOSITORY https://github.com/rakshasa/libtorrent.git
        GIT_TAG        master
)
FetchContent_MakeAvailable(rakshasa-libtorrent)
file(GLOB_RECURSE rakshasa-libtorrent_SOURCES "${rakshasa-libtorrent_SOURCE_DIR}/src/*.cc")

add_library(rakshasa-libtorrent SHARED ${rakshasa-libtorrent_SOURCES})

target_include_directories(rakshasa-libtorrent PUBLIC
        ${rakshasa-libtorrent_SOURCE_DIR}
        ${rakshasa-libtorrent_SOURCE_DIR}/src
        ${rakshasa-libtorrent_SOURCE_DIR}/src/torrent)

find_package(OpenSSL REQUIRED)
target_link_libraries(rakshasa-libtorrent PUBLIC OpenSSL::Crypto)

set(RAKSASHA_LIBTORRENT_CONFIG_H  [=[
/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* Default address space size. */
#define DEFAULT_ADDRESS_SPACE_SIZE 4096

/* Determine the block size */
#define FS_STAT_BLOCK_SIZE (m_stat.f_frsize)

/* Type of block count member in stat struct */
#define FS_STAT_COUNT_TYPE fsblkcnt_t

/* Function to determine filesystem stats from fd */
#define FS_STAT_FD fstatvfs(fd, &m_stat) == 0

/* Function to determine filesystem stats from filename */
#define FS_STAT_FN statvfs(fn, &m_stat) == 0

/* Type of block size member in stat struct */
#define FS_STAT_SIZE_TYPE unsigned long

/* Type of second argument to statfs function */
#define FS_STAT_STRUCT struct statvfs

/* The function to set pthread name has no pthread argument. */
/* #undef HAS_PTHREAD_SETNAME_NP_DARWIN */

/* The function to set pthread name has a pthread_t argumet. */
#define HAS_PTHREAD_SETNAME_NP_GENERIC 1

/* Defined if backtrace() could be fully identified. */
#define HAVE_BACKTRACE 1

/* true if config.h was included */
#define HAVE_CONFIG_H 1

/* define if the compiler supports basic C++11 syntax */
/* #undef HAVE_CXX11 */

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <execinfo.h> header file. */
#define HAVE_EXECINFO_H 1

/* Linux's fallocate supported. */
#define HAVE_FALLOCATE 1

/* sys/inotify.h exists and works correctly */
#define HAVE_INOTIFY 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have `z' library (-lz) */
#define HAVE_LIBZ 1

/* Define to 1 if you have the <mcheck.h> header file. */
#define HAVE_MCHECK_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `posix_memalign' function. */
#define HAVE_POSIX_MEMALIGN 1

/* Define if you have POSIX threads libraries and header files. */
#define HAVE_PTHREAD 1

/* Define to 1 if you have the <pthread.h> header file. */
#define HAVE_PTHREAD_H 1

/* Have PTHREAD_PRIO_INHERIT. */
#define HAVE_PTHREAD_PRIO_INHERIT 1

/* Define if compiler supports C++0x features. */
/* #undef HAVE_STDCXX_0X */

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/inotify.h> header file. */
#define HAVE_SYS_INOTIFY_H 1

/* Define to 1 if you have the <sys/mount.h> header file. */
/* #undef HAVE_SYS_MOUNT_H */

/* Define to 1 if you have the <sys/param.h> header file. */
/* #undef HAVE_SYS_PARAM_H */

/* Define to 1 if you have the <sys/statfs.h> header file. */
#define HAVE_SYS_STATFS_H 1

/* Define to 1 if you have the <sys/statvfs.h> header file. */
#define HAVE_SYS_STATVFS_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/vfs.h> header file. */
#define HAVE_SYS_VFS_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Big endian */
/* #undef IS_BIG_ENDIAN */

/* Little endian */
#define IS_LITTLE_ENDIAN 1

/* kqueue only supports sockets. */
/* #undef KQUEUE_SOCKET_ONLY */

/* enable instrumentation */
#define LT_INSTRUMENTATION 1

/* Define to the sub-directory where libtool stores uninstalled libraries. */
#define LT_OBJDIR ".libs/"

/* Largest L1 cache size we know of should work on all archs. */
#define LT_SMP_CACHE_BYTES 128

/* Name of package */
#define PACKAGE "libtorrent"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "sundell.software@gmail.com"

/* Define to the full name of this package. */
#define PACKAGE_NAME "libtorrent"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "libtorrent 0.13.8"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "libtorrent"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.13.8"

/* Identifier that is part of the default peer id */
#define PEER_NAME "-lt0D80-"

/* 4 byte client and version identifier for DHT */
#define PEER_VERSION "lt\x0D\x80"

/* Define to necessary symbol if this constant uses a non-standard name on
your system. */
/* #undef PTHREAD_CREATE_JOINABLE */

/* The size of `long', as computed by sizeof. */
#define SIZEOF_LONG 8

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define this if the compiler supports the unused attribute */
#define SUPPORT_ATTRIBUTE_UNUSED 1

/* Define this if the compiler supports the visibility attributes. */
#define SUPPORT_ATTRIBUTE_VISIBILITY 1

/* Require byte alignment */
/* #undef USE_ALIGNED */

/* Use __builtin_popcount. */
#define USE_BUILTIN_POPCOUNT 1

/* Using Cyrus RC4 implementation. */
/* #undef USE_CYRUS_RC4 */

/* Use epoll. */
#define USE_EPOLL 1

/* Enable extra debugging checks. */
/* #undef USE_EXTRA_DEBUG */

/* Use interrupt socket instead of pthread_kill */
/* #undef USE_INTERRUPT_SOCKET */

/* Use kqueue. */
/* #undef USE_KQUEUE */

/* Use madvise */
#define USE_MADVISE 1

/* Use mincore */
#define USE_MINCORE 1

/* use char* in mincore */
#define USE_MINCORE_UNSIGNED 1

/* Using Mozilla's SHA1 implementation. */
/* #undef USE_NSS_SHA */

/* Using OpenSSL. */
#define USE_OPENSSL 1

/* Using OpenSSL's SHA1 implementation. */
#define USE_OPENSSL_SHA 1

/* posix_fallocate supported. */
/* #undef USE_POSIX_FALLOCATE */

/* Version number of package */
#define VERSION "0.13.8"

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
/* #  undef WORDS_BIGENDIAN */
# endif
#endif

/* Enable large inode numbers on Mac OS X 10.5.  */
#ifndef _DARWIN_USE_64_BIT_INODE
# define _DARWIN_USE_64_BIT_INODE 1
#endif

/* Number of bits in a file offset, on hosts where this is settable. */
/* #undef _FILE_OFFSET_BITS */

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */

/* Null-wrapper if unused attribute is unsupported */
#define __UNUSED __attribute__((unused))

/* Defined to return type of backtrace(). */
#define backtrace_size_t int

/* LibTorrent defined cacheline aligned. */
#define lt_cacheline_aligned __attribute__((__aligned__(LT_SMP_CACHE_BYTES)))
]=]
)

file(WRITE "${rakshasa-libtorrent_SOURCE_DIR}/config.h" ${RAKSASHA_LIBTORRENT_CONFIG_H})