
find_package(nlohmann_json QUIET)
if (nlohmann_json_FOUND)
    log_found(nlohmann_json)
else()
    set(JSON_BuildTests OFF)

    if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/nlohmann_json)
        log_dir_found(nlohmann_json)
        add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/nlohmann_json)
        set(nlohmann_json_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/nlohmann_json)
    else()
        log_fetch(nlohmann_json)
        include(FetchContent)
        FetchContent_Declare(
                nlohmann_json
                GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent.git
                GIT_TAG        master
        )
        FetchContent_MakeAvailable(nlohmann_json)
    endif()
endif()

if(IS_DIRECTORY "${nlohmann_json_SOURCE_DIR}")
    set_property(DIRECTORY ${nlohmann_json_SOURCE_DIR} PROPERTY EXCLUDE_FROM_ALL YES)
endif()