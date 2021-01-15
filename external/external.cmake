cmake_minimum_required(VERSION 3.14)

include(FetchContent)

function(log_found library)
    message(STATUS "Local installation of dependency ${library} found.")
endfunction()

function(log_not_found library)
    message(STATUS "Fetching dependency ${library}...")
endfunction()

find_package(CLI11 QUIET)
if (CLI11_FOUND)
    log_found(CLI11)
else()
    log_not_found(CLI11)

    FetchContent_Declare(
            CLI11
            GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
            GIT_TAG        master
    )
    # disable tests
    set(CLI11_BUILD_DOCS OFF)
    set(CLI11_BUILD_EXAMPLES OFF)
    FetchContent_MakeAvailable(CLI11)
endif()

find_package(fmt QUIET)
if (fmt_FOUND)
    log_found(fmt)
else()
    log_not_found(fmt)
    FetchContent_Declare(
            fmt
            GIT_REPOSITORY https://github.com/fmtlib/fmt.git
            GIT_TAG        master
    )
    set(FORMAT_BUILD_TESTING OFF)
    set(FMT_INSTALL ON)
    FetchContent_MakeAvailable(fmt)
endif()

find_package(gsl-lite QUIET)
if (gsl-lite_FOUND)
    log_not_found(gsl-lite)
else()
    log_found(gsl-lite)
    FetchContent_Declare(
            gsl-lite
            GIT_REPOSITORY https://github.com/gsl-lite/gsl-lite.git
            GIT_TAG        master
    )
    FetchContent_MakeAvailable(gsl-lite)
endif()


find_package(expected-lite QUIET)
if (expected-lite_FOUND)
    log_found(expected-lite)
else()
    log_not_found(expected-lite)
    FetchContent_Declare(
            expected-lite
            GIT_REPOSITORY https://github.com/martinmoene/expected-lite.git
            GIT_TAG        master
    )
    FetchContent_MakeAvailable(expected-lite)
endif()


if (TORRENTTOOLS_BUILD_TESTS)
    find_package(Catch2 QUIET)
    if (Catch2_FOUND)
        log_found(Catch2)
    else()
        log_not_found(Catch2)
        FetchContent_Declare(
                Catch2
                GIT_REPOSITORY https://github.com/catchorg/Catch2.git
                GIT_TAG        v2.x
        )
        FetchContent_MakeAvailable(Catch2)
        set(CMAKE_MODULE_PATH "${Catch2_SOURCE_DIR}/contrib" ${CMAKE_MODULE_PATH})
    endif()
endif()

find_package(ctre QUIET)
if (ctre_FOUND OR TARGET ctre::ctre)
    log_found(ctre)
else()
    log_not_found(ctre)
    FetchContent_Declare(
            ctre
            GIT_REPOSITORY https://github.com/hanickadot/compile-time-regular-expressions.git
            GIT_TAG        master
    )
    FetchContent_MakeAvailable(ctre)
endif()


find_package(re2 QUIET)
if (re2_FOUND OR TARGET re2::re2)
    log_found(re2)
else()
    log_not_found(re2)

    include(FetchContent)
    FetchContent_Declare(
            re2
            GIT_REPOSITORY https://github.com/google/re2.git
            GIT_TAG        master
    )
    # Silence re2 CMP0077 warnings
    set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
    set(RE2_BUILD_TESTING OFF)
    FetchContent_MakeAvailable(re2)
    target_include_directories(re2 PUBLIC
            $<INSTALL_INTERFACE:>
            $<BUILD_INTERFACE:${re2_SOURCE_DIR}>)
endif()

find_package(nlohmann_json QUIET)
if (nlohmann_json_FOUND OR TARGET nlohmann_json::nlohmann_json)
    log_found(nlohmann_json)
else()
    log_not_found(nlohmann_json)

    include(FetchContent)
    FetchContent_Declare(
            nlohmann_json
            GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent.git
            GIT_TAG        master
    )
    set(JSON_BuildTests OFF)
    FetchContent_MakeAvailable(nlohmann_json)
endif()


find_package(yaml-cpp QUIET)
if (yaml-cpp_FOUND OR TARGET yaml-cpp)
    log_found(yaml-cpp)
else()
    log_not_found(yaml-cpp)

    include(FetchContent)
    FetchContent_Declare(
            yaml-cpp
            GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
            GIT_TAG        master
    )
    set(YAML_CPP_BUILD_TESTS OFF)
    FetchContent_MakeAvailable(yaml-cpp)
endif()

find_package(termcontrol QUIET)
if (termcontrol_FOUND OR TARGET termcontrol::termcontrol)
    log_found(termcontrol)
else()
    log_not_found(termcontrol)
    FetchContent_Declare(
            termcontrol
            GIT_REPOSITORY   https://github.com/fbdtemme/termcontrol.git
            GIT_TAG          main
    )
    set(TERMCONTROL_BUILD_TESTS OFF)
    FetchContent_MakeAvailable(termcontrol)
endif()


find_package(cliprogress QUIET)
if (cliprogress_FOUND OR TARGET cliprogress::cliprogress)
    log_found(cliprogress)
else()
    log_not_found(cliprogress)
    FetchContent_Declare(
            cliprogress
            GIT_REPOSITORY   https://github.com/fbdtemme/cliprogress.git
            GIT_TAG          main
    )
    set(CLI11_BUILD_DOCS OFF)
    set(CLI11_BUILD_TESTS OFF)
    FetchContent_MakeAvailable(cliprogress)
endif()


find_package(dottorrent QUIET)
if (dottorrent_FOUND OR TARGET dottorrent::dottorrent)
    log_found(dottorrent)
else()
    log_not_found(dottorrent)
    FetchContent_Declare(
            dottorrent
            GIT_REPOSITORY   https://github.com/fbdtemme/dottorrent.git
            GIT_TAG          master
    )
    set(DOTTORRENT_BUILD_TESTS OFF)
    FetchContent_MakeAvailable(dottorrent)
endif()