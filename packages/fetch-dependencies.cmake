find_package(Git REQUIRED)


execute_process(
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMAND ${GIT_EXECUTABLE} clone https://github.com/microsoft/vcpkg.git
)
execute_process(
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMAND ./vcpkg/bootstrap-vcpkg.sh
)

# Force vcpkg to also cache registries into the vcpkg dir
set(ENV{XDG_CACHE_HOME} "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/cache")

execute_process(
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMAND ./vcpkg/vcpkg install --only-downloads --downloads-root ./vcpkg/downloads --x-buildtrees-root ./vcpkg/buildtrees
)