set(RPM_PACKAGE_RELEASE  2)

message(STATUS "Configuring RPM .spec file")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/packages/rpm/torrenttools.spec.in
        ${CMAKE_CURRENT_SOURCE_DIR}/packages/rpm/torrenttools.spec
        @ONLY)