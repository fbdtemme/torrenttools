include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/FindSphinx.cmake)
find_package(Sphinx REQUIRED)

#This will be the main output of our command
set(SPHINX_CONF_IN       ${CMAKE_CURRENT_SOURCE_DIR}/conf.py.in)
set(SPHINX_CONF_OUT      ${CMAKE_CURRENT_BINARY_DIR}/conf.py)
set(SPHINX_CONFIG_DIR    ${CMAKE_CURRENT_BINARY_DIR})

#Replace variables inside @@ with the current values
configure_file(${SPHINX_CONF_IN} ${SPHINX_CONF_OUT} @ONLY)

set(SPHINX_SOURCE     ${CMAKE_CURRENT_SOURCE_DIR})
set(SPHINX_BUILD      ${CMAKE_CURRENT_BINARY_DIR}/sphinx)
set(SPHINX_INDEX_FILE ${SPHINX_BUILD}/index.html)

# Only regenerate Sphinx when:
# - Our doc files have been updated
# - The Sphinx config has been updated

file(GLOB_RECURSE SPHINX_RST_FILES ${CMAKE_CURRENT_LIST_DIR}/**.rst)
file(GLOB_RECURSE SPHINX_CSV_FILES ${CMAKE_CURRENT_LIST_DIR}/**.csv)
list(APPEND SPHINX_DOC_FILES ${SPHINX_RST_FILES} ${SPHINX_CSV_FILES})

add_custom_command(
        OUTPUT  ${SPHINX_INDEX_FILE}
        COMMAND
            ${SPHINX_EXECUTABLE} -b html
            -c ${SPHINX_CONFIG_DIR}
            ${SPHINX_SOURCE} ${SPHINX_BUILD}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS
        # Other docs files you want to track should go here (or in some variable)
        ${SPHINX_DOC_FILES}
        MAIN_DEPENDENCY ${SPHINX_CONF_OUT}
        COMMENT "Generating documentation with Sphinx"
        COMMAND_EXPAND_LISTS)

# Nice named target so we can run the job easily
add_custom_target(Sphinx ALL DEPENDS ${SPHINX_INDEX_FILE})

# Add an install target to install the docs
if (TORRENTTOOLS_INSTALL)
    install(DIRECTORY ${SPHINX_BUILD}
            DESTINATION ${CMAKE_INSTALL_DOCDIR}
            COMPONENT documentation)

endif()

