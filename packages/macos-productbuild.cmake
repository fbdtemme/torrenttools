set(CPACK_PACKAGING_INSTALL_PREFIX "/Library/torrenttools")
message(STATUS "Setting prefix to /Library/torrenttools")

set(CPACK_POSTFLIGHT_TORRENTTOOLS_SCRIPT           "${CMAKE_CURRENT_LIST_DIR}/productbuild/postflight.sh")
set(CPACK_PRODUCTBUILD_RESOURCES_DIR               "${CMAKE_CURRENT_LIST_DIR}/productbuild/Resources")
set(CPACK_PRODUCTBUILD_BACKGROUND                  "torrenttools-icon-transparent.png")
set(CPACK_PRODUCTBUILD_BACKGROUND_MIMETYPE         "image/png")
set(CPACK_PRODUCTBUILD_BACKGROUND_SCALING          "proportional")

set(CPACK_PRODUCTBUILD_BACKGROUND_DARKAQUA          "torrenttools-icon-transparent.png")
set(CPACK_PRODUCTBUILD_BACKGROUND_MIMETYPE_DARKAQUA "image/png")
set(CPACK_PRODUCTBUILD_BACKGROUND_SCALING_DARKAQUA  "proportional")
