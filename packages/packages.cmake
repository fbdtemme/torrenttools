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
