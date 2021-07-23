set(CPACK_WIX_PRODUCT_GUID  "ECB15625-7F24-41F1-8287-E5F7AE9D233E")
set(CPACK_WIX_UPGRADE_GUID  "E7E112B4-2A91-4E82-BBF8-377C1407CE8F")
set(CPACK_WIX_PRODUCT_ICON  ${CMAKE_CURRENT_LIST_DIR}/wix/torrenttools-icon.ico)
set(CPACK_WIX_UI_BANNER     ${CMAKE_CURRENT_LIST_DIR}/wix/torrenttools-banner.png)

# Necessary to propagate environment change to all windows without restarting
list(APPEND CPACK_WIX_EXTENSIONS        "WixUtilExtension")
list(APPEND CPACK_WIX_EXTRA_SOURCES     "${CMAKE_CURRENT_LIST_DIR}/wix/create-localappdata-folder.wxs")

list(APPEND CPACK_WIX_PATCH_FILE
        "${CMAKE_CURRENT_LIST_DIR}/wix/update_path.xml"
        "${CMAKE_CURRENT_LIST_DIR}/wix/broadcast_env_change.xml"
        "${CMAKE_CURRENT_LIST_DIR}/wix/copy-config.xml")