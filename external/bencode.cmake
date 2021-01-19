
find_package(bencode QUIET)
if (bencode_FOUND)
    log_found(bencode)
else()
    set(BENCODE_BUILD_TESTS OFF)
    set(BENCODE_ENABLE_INSTALL OFF)
    if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/bencode)
        log_dir_found(bencode)
        add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/bencode)
        set(bencode_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/bencode)
    else()
        log_fetch(bencode)
        FetchContent_Declare(
                bencode
                GIT_REPOSITORY   https://github.com/fbdtemme/bencode.git
                GIT_TAG          master
        )
        FetchContent_MakeAvailable(bencode)
    endif()
endif()

if(IS_DIRECTORY "${bencode_SOURCE_DIR}")
    set_property(DIRECTORY ${bencode_SOURCE_DIR} PROPERTY EXCLUDE_FROM_ALL YES)
endif()

