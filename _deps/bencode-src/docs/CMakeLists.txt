find_package(Doxygen REQUIRED)

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/FindSphinx.cmake)
find_package(Sphinx REQUIRED)

set(BENCODE_INCLUDE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../include)
file(GLOB_RECURSE BENCODE_PUBLIC_HEADERS ${BENCODE_INCLUDE_DIRECTORY}/*.hpp)

#This will be the main output of our command
set(DOXYGEN_INDEX_FILE   ${CMAKE_CURRENT_SOURCE_DIR}/html/index.html)
set(DOXYGEN_INPUT_DIR    ${CMAKE_SOURCE_DIR}/include)
set(DOXYGEN_OUTPUT_DIR   ${CMAKE_CURRENT_BINARY_DIR}/doxygen)
set(DOXYGEN_INDEX_FILE   ${DOXYGEN_OUTPUT_DIR}/html/index.html)
set(DOXYFILE_IN          ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYFILE_OUT         ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
set(SPHINX_CONF_IN       ${CMAKE_CURRENT_SOURCE_DIR}/conf.py.in)
set(SPHINX_CONF_OUT      ${CMAKE_CURRENT_BINARY_DIR}/conf.py)
set(SPHINX_CONFIG_DIR    ${CMAKE_CURRENT_BINARY_DIR})



#Replace variables inside @@ with the current values
configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

#Replace variables inside @@ with the current values
configure_file(${SPHINX_CONF_IN} ${SPHINX_CONF_OUT} @ONLY)
# Doxygen won't create this for us
file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})

# Only regenerate Doxygen when the Doxyfile or public headers change
add_custom_command(
        OUTPUT          ${DOXYGEN_INDEX_FILE}
        DEPENDS         ${BENCODE_PUBLIC_HEADERS}
        COMMAND         ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
        MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
        COMMENT         "Generating docs"
        VERBATIM)

# Nice named target so we can run the job easily
add_custom_target(Doxygen ALL DEPENDS ${DOXYGEN_INDEX_FILE})

set(SPHINX_SOURCE     ${CMAKE_CURRENT_SOURCE_DIR})
set(SPHINX_BUILD      ${CMAKE_CURRENT_BINARY_DIR}/sphinx)
set(SPHINX_INDEX_FILE ${SPHINX_BUILD}/index.html)

# Only regenerate Sphinx when:
# - Doxygen has rerun
# - Our doc files have been updated
# - The Sphinx config has been updated


file(GLOB_RECURSE SPHINX_DOC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.rst)

add_custom_command(
        OUTPUT  ${SPHINX_INDEX_FILE}
        COMMAND
            ${SPHINX_EXECUTABLE} -b html
            -c ${SPHINX_CONFIG_DIR}
            -Dbreathe_projects.bencode=${DOXYGEN_OUTPUT_DIR}/xml
            ${SPHINX_SOURCE} ${SPHINX_BUILD}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS
        # Other docs files you want to track should go here (or in some variable)
        ${SPHINX_DOC_FILES}
        ${DOXYGEN_INDEX_FILE}
        MAIN_DEPENDENCY ${SPHINX_CONF_OUT}
        COMMENT "Generating documentation with Sphinx"
        COMMAND_EXPAND_LISTS)

# Nice named target so we can run the job easily
add_custom_target(Sphinx ALL DEPENDS ${SPHINX_INDEX_FILE})

# Add an install target to install the docs
include(GNUInstallDirs)
install(DIRECTORY ${SPHINX_BUILD}
        DESTINATION ${CMAKE_INSTALL_DOCDIR})

