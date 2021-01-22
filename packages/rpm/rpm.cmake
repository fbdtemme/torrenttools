# RPM SOURCE PACKAGE
set(CPACK_RPM_PACKAGE_RELEASE_DIST                                                OFF)

set(CPACK_RPM_USER_BINARY_SPECFILE                    ${CMAKE_CURRENT_SOURCE_DIR}/packages/rpm/torrenttools.spec)
set(CPACK_RPM_PACKAGE_RELEASE_DIST                    "OFF")
set(CPACK_RPM_PACKAGE_RELEASE                         1)


message(STATUS "Configuring RPM .spec file")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/packages/rpm/torrenttools.spec.in
        ${CMAKE_CURRENT_SOURCE_DIR}/packages/rpm/torrenttools.spec
        @ONLY)