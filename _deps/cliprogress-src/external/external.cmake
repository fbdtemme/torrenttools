cmake_minimum_required(VERSION 3.14)

include(FetchContent)

function(log_found library)
    message(STATUS "Local installation of dependency ${library} found.")
endfunction()

function(log_not_found library)
    message(STATUS "Fetching dependency ${library}...")
endfunction()



find_package(PalSigslot QUIET)
if (PalSigslot_FOUND OR TARGET Pal::Sigslot)
    log_found(PalSigslot)
else()
    log_not_found(PalSigslot)

    FetchContent_Declare(
            PalSigslot
            GIT_REPOSITORY https://github.com/palacaze/sigslot.git
            GIT_TAG        master
    )
    set(SIGSLOT_COMPILE_EXAMPLES OFF)
    set(SIGSLOT_COMPILE_TESTS OFF)
    FetchContent_MakeAvailable(PalSigslot)
endif()

find_package(gsl-lite QUIET)
if (gsl-lite_FOUND OR TARGET gsl::gsl-lite-v1)
    message(STATUS "Local installation of gsl-lite found.")
else()
    message(STATUS "Fetching dependency gsl-lite...")
    FetchContent_Declare(
            gsl-lite
            GIT_REPOSITORY https://github.com/gsl-lite/gsl-lite.git
            GIT_TAG        master
    )
    FetchContent_MakeAvailable(gsl-lite)
endif()

find_package(termcontrol QUIET)
if (termcontrol_FOUND OR TARGET termcontrol::termcontrol)
    log_found(termcontrol)
else()
    log_not_found(termcontrol)
    FetchContent_Declare(
            termcontrol
            GIT_REPOSITORY https://github.com/fbdtemme/termcontrol.git
            GIT_TAG        main
    )
    set(TERMCONTROL_INSTALL ON)
    set(TERMCONTROL_BUILD_TESTS OFF)
    FetchContent_MakeAvailable(termcontrol)
endif()

if (CLIPROGRESS_BUILD_TESTING)
    find_package(Catch2 QUIET)
    if (Catch2_FOUND OR TARGET Catch2::Catch2)
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