cmake_minimum_required(VERSION 3.16)

include(FetchContent)
include(GNUInstallDirs)

find_package(ctre QUIET)
if (ctre_FOUND OR TARGET ctre::ctre)
    message(STATUS "Local installation of dependency ctre found.")
elseif(EXISTS ${CMAKE_CURRENT_LIST_DIR}/ctre)
    message(STATUS "ctre source directory found")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/ctre)
    set(ctre_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/ctre)
else()
    message(STATUS "Fetching dependency ctre ...")
    FetchContent_Declare(
            ctre
            GIT_REPOSITORY https://github.com/hanickadot/compile-time-regular-expressions.git
            GIT_TAG        master
    )
    # disable tests
    set(CTRE_BUILD_TESTS OFF)
    set(CTRE_BUILD_PACAKGES OFF)
    FetchContent_MakeAvailable(ctre)
endif()

if(IS_DIRECTORY "${ctre_SOURCE_DIR}")
set_property(DIRECTORY ${ctre_SOURCE_DIR} PROPERTY EXCLUDE_FROM_ALL YES)
endif()


find_package(gsl-lite QUIET)
if (gsl-lite_FOUND OR TARGET gsl::gsl-lite-v1)
    message(STATUS "Local installation of gsl-lite found.")
elseif(EXISTS ${CMAKE_CURRENT_LIST_DIR}/gsl-lite)
    message(STATUS "gsl-lite source directory found")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/gsl-lite)
    set(gsl-lite_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/gsl-lite)
else()
    message(STATUS "Fetching dependency gsl-lite...")
    FetchContent_Declare(
            gsl-lite
            GIT_REPOSITORY https://github.com/gsl-lite/gsl-lite.git
            GIT_TAG        master
    )
    FetchContent_MakeAvailable(gsl-lite)
endif()

if(IS_DIRECTORY "${gsl-lite_SOURCE_DIR}")
    set_property(DIRECTORY ${gsl-lite_SOURCE_DIR} PROPERTY EXCLUDE_FROM_ALL YES)
endif()


find_package(fmt QUIET)
if (fmt_FOUND OR TARGET fmt::fmt-header-only)
    message(STATUS "Local installation of dependency fmt found.")
elseif(EXISTS ${CMAKE_CURRENT_LIST_DIR}/fmt)
    message(STATUS "fmt source directory found")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/fmt)
    set(fmt_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/fmt)
else()
    message(STATUS "Fetching dependency fmt...")
    FetchContent_Declare(
            fmt
            GIT_REPOSITORY https://github.com/fmtlib/fmt.git
            GIT_TAG        master
    )
    set(FMT_INSTALL ON)
    set(BUILD_SHARED_LIBS ON)
    FetchContent_MakeAvailable(fmt)
endif()

if(IS_DIRECTORY "${fmt_SOURCE_DIR}")
    set_property(DIRECTORY ${gsl-lite_SOURCE_DIR} PROPERTY EXCLUDE_FROM_ALL YES)
endif()


if (TERMCONTROL_BUILD_TESTS)
    find_package(Catch2 QUIET)
    if (Catch2_FOUND OR TARGET Catch2::Catch2)
        message(STATUS "Local installation of Catch2 found.")
    elseif(EXISTS ${CMAKE_CURRENT_LIST_DIR}/Catch2)
        message(STATUS "Catch2 source directory found")
        add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Catch2)
        set(Catch2_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/Catch2)
    else()
        message(STATUS "Fetching dependency Catch2...")
        FetchContent_Declare(
                Catch2
                GIT_REPOSITORY https://github.com/catchorg/Catch2.git
                GIT_TAG        v2.x
        )
        FetchContent_MakeAvailable(Catch2)
    endif()

    list(APPEND CMAKE_MODULE_PATH "${Catch2_SOURCE_DIR}/contrib")

    if(IS_DIRECTORY "${Catch2_SOURCE_DIR}")
        set_property(DIRECTORY ${Catch2_SOURCE_DIR} PROPERTY EXCLUDE_FROM_ALL YES)
    endif()
endif()