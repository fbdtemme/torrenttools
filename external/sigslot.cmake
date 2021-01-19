
find_package(PalSigslot QUIET)
if (PalSigslot_FOUND)
    log_found(PalSigslot)
else()
    set(SIGSLOT_COMPILE_EXAMPLES OFF)
    set(SIGSLOT_COMPILE_TESTS OFF)

    if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/sigslot)
        log_dir_found(sigslot)
        add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/sigslot)
        set(sigslot_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/sigslot)
    else()
        log_fetch(PalSigslot)

        FetchContent_Declare(
                PalSigslot
                GIT_REPOSITORY https://github.com/palacaze/sigslot.git
                GIT_TAG        master
        )
        FetchContent_MakeAvailable(PalSigslot)
    endif()
endif()