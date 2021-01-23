cmake_minimum_required(VERSION 3.14)

include(Catch)

add_executable(cliprogress-tests
        main.cpp
        widgets/label.cpp
        widgets/bar.cpp
        application.cpp
)

target_link_libraries(cliprogress-tests cliprogress Catch2::Catch2)
catch_discover_tests(cliprogress-tests)