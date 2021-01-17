## some variables to generate package files, no cpack because it is impossible :-(

set(PACKAGE_NAME                                                ${CMAKE_PROJECT_NAME})
set(PACKAGE_VENDOR                                                           fbdtemme)
set(PACKAGE_LICENSE                                                               MIT)
set(PACKAGE_HOMEPAGE_URL                              "${CMAKE_PROJECT_HOMEPAGE_URL}")
set(PACKAGE_VERSION_MAJOR                            "${CMAKE_PROJECT_VERSION_MAJOR}")
set(PACKAGE_VERSION_MINOR                            "${CMAKE_PROJECT_VERSION_MINOR}")
set(PACKAGE_VERSION_PATCH                            "${CMAKE_PROJECT_VERSION_PATCH}")
set(PACKAGE_VERSION                                        "${CMAKE_PROJECT_VERSION}")
set(PACKAGE_SUMMARY                                    "${CMAKE_PROJECT_DESCRIPTION}")
set(PACKAGE_DESCRIPTION                                          "${PACKAGE_SUMMARY}")

# RPM variables
set(RPM_PACKAGE_RELEASE                                                             1)
set(CPACK_RPM_PACKAGE_RELEASE_DIST                                                OFF)

message(STATUS "Configuring package recepices")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/packages/rpm/torrenttools.spec.in
               ${CMAKE_CURRENT_SOURCE_DIR}/packages/rpm/torrenttools.spec
               @ONLY)
