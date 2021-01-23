
# Copy the license file to a txt file as required by WIX
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/LICENSE" "${CMAKE_CURRENT_LIST_DIR}/LICENSE.txt")

set(CPACK_SOURCE_GENERATOR                   TGZ RPM)
set(CPACK_GENERATOR                          WIX)
set(CPACK_PACKAGE_DESCRIPTION_FILE           "${CMAKE_CURRENT_LIST_DIR}/package_summary.txt")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY        "${CMAKE_PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_FILE_NAME                  "${PROJECT_NAME}-${CMAKE_PROJECT_VERSION}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME           "${PROJECT_NAME}-${CMAKE_PROJECT_VERSION}")
set(CPACK_RESOURCE_FILE_LICENSE              "${CMAKE_CURRENT_LIST_DIR}/LICENSE.txt")
set(CPACK_PACKAGE_NAME                       "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR                     "Florian De Temmerman")
set(CPACK_PACKAGE_CONTACT                    "Florian De Temmerman")
set(CPACK_PACKAGE_VERSION_MAJOR              "${CMAKE_PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR              "${CMAKE_PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH              "${CMAKE_PROJECT_VERSION_PATCH}")
set(CPACK_BUILD_SOURCE_DIRS                  "${CMAKE_CURRENT_SOURCE_DIR}")
set(CPACK_CREATE_DESKTOP_LINKS               OFF)
set(CPACK_PACKAGE_INSTALL_DIRECTORY          torrenttools)

set(CPACK_ARCHIVE_COMPONENT_INSTALL          ON)
set(CPACK_COMPONENTS_ALL                     torrenttools)
set(CPACK_COMPONENTS_TORRENTTOOLS_REQUIRED   ON)

cpack_add_component(torrenttools
        DISPLAY_NAME core
        DESCRIPTION "The core application."
        REQUIRED)

cpack_add_component(undefined HIDDEN)

# Get the list of ignored files from .gitignore.
set(gitignore ${CMAKE_CURRENT_SOURCE_DIR}/.gitignore)
if (EXISTS ${gitignore})
    file (STRINGS ${gitignore} lines)
    list(REMOVE_ITEM lines /doc/html)
    foreach (line ${lines})
        string(REPLACE "." "[.]" line "${line}")
        string(REPLACE "*" ".*" line "${line}")
        set(ignored_files ${ignored_files} "${line}$" "${line}/")
    endforeach ()
endif()

list(APPEND ignored_files ".git.*")
list(APPEND CPACK_SOURCE_IGNORE_FILES        ${ignored_files})
## some variables to generate packaging files, seperate from CPACK variables


set(PACKAGE_NAME                ${CMAKE_PROJECT_NAME})
set(PACKAGE_VENDOR              fbdtemme)
set(PACKAGE_VENDOR_FULL_NAME    "Florian De Temmerman")
set(PACAKGE_VENDOR_EMAIL        floriandetemmerman@gmail.com)
set(PACKAGE_MAINTAINER          ${PACKAGE_VENDOR_FULL_NAME})
set(PACKAGE_MAINTAINER_EMAIL    ${PACAKGE_VENDOR_EMAIL})
set(PACAKGE_CONTACT             floriandetemmerman@gmail.com)
set(PACKAGE_LICENSE             MIT)
set(PACKAGE_LICENSE_YEAR        2021)
set(PACKAGE_LICENSE_FILE        "${CMAKE_SOURCE_DIR}/LICENSE")
set(PACKAGE_HOMEPAGE_URL        "${CMAKE_PROJECT_HOMEPAGE_URL}")
set(PACKAGE_VERSION_MAJOR       "${CMAKE_PROJECT_VERSION_MAJOR}")

set(PACKAGE_VERSION_MINOR       "${CMAKE_PROJECT_VERSION_MINOR}")
set(PACKAGE_VERSION_PATCH       "${CMAKE_PROJECT_VERSION_PATCH}")
set(PACKAGE_VERSION             "${CMAKE_PROJECT_VERSION}")
set(PACKAGE_DESCRIPTION_SUMMARY "${CMAKE_PROJECT_DESCRIPTION}")
set(PACKAGE_DESCRIPTION         "${PACKAGE_DESCRIPTION_SUMMARY}")

# Read in LICENSE
file(READ ${CMAKE_SOURCE_DIR}/LICENSE PACKAGE_LICENSE_STATEMENT)

# Remove trailing whitespace and ending lines.  They are sometimes hard to
# see or remove in editors.
string(REGEX REPLACE "[ \t]*\n" "\n" PACKAGE_LICENSE_STATEMENT "${PACKAGE_LICENSE_STATEMENT}")
string(REGEX REPLACE "\n+$" "" PACKAGE_LICENSE_STATEMENT "${PACKAGE_LICENSE_STATEMENT}")

include(${CMAKE_CURRENT_LIST_DIR}/rpm/rpm.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/windows-wix.cmake)

include(CPack)

