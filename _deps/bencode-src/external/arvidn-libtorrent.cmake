include(FetchContent)

find_package(LibtorrentRasterbar QUIET)
if (LibtorrentRasterbar_FOUND)
    message(STATUS "Local installation of libtorrent-rasterbar found.")
else()
    message(STATUS "Fetching dependency libtorrent-rasterbar...")
    FetchContent_Declare(
            libtorrent-rasterbar
            GIT_REPOSITORY https://github.com/arvidn/libtorrent.git
            GIT_TAG        RC_2_0
    )
    FetchContent_MakeAvailable(libtorrent-rasterbar)

    if(IS_DIRECTORY "${libtorrent-rasterbar_SOURCE_DIR}")
        set_property(DIRECTORY ${libtorrent-rasterbar_SOURCE_DIR} PROPERTY EXCLUDE_FROM_ALL YES)
    endif()

endif()

