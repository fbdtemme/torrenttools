# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.19.1)

# We name the project and the target for the ExternalProject_Add() call
# to something that will highlight to the user what we are working on if
# something goes wrong and an error message is produced.

project(cliprogress-populate NONE)

include(ExternalProject)
ExternalProject_Add(cliprogress-populate
                     "UPDATE_DISCONNECTED" "False" "GIT_REPOSITORY" "https://github.com/fbdtemme/cliprogress.git" "GIT_TAG" "main"
                    SOURCE_DIR          "/home/runner/work/torrenttools/torrenttools/build/_deps/cliprogress-src"
                    BINARY_DIR          "/home/runner/work/torrenttools/torrenttools/build/_deps/cliprogress-build"
                    CONFIGURE_COMMAND   ""
                    BUILD_COMMAND       ""
                    INSTALL_COMMAND     ""
                    TEST_COMMAND        ""
                    USES_TERMINAL_DOWNLOAD  YES
                    USES_TERMINAL_UPDATE    YES
)


