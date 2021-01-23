include(FindPackageHandleStandardArgs)

#Look for an executable called sphinx-build
find_program(SPHINX_EXECUTABLE
        NAMES sphinx-build
        DOC "Path to sphinx-build executable")

if (NOT SPHINX_EXECUTABLE OR SPHINX_EXECUTABLE STREQUAL SPHINX_EXECUTABLE-NOTFOUND)
    include(FindPython3)
    set(FPHSA_NAME_MISMATCHED TRUE)
    find_package(Python3 QUIET COMPONENTS Interpreter)
    execute_process(
            COMMAND python -c "import site; print(site.getusersitepackages())"
            OUTPUT_VARIABLE Python3_USERLIB
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    find_file(
            SPHINX_LIB_MAIN_PATH  "__main__.py"
            PATHS ${Python3_SITELIB}/sphinx
                  ${Python3_ARCHLIB}/sphinx
                  ${Python3_USERLIB}/sphinx
    )
    if (SPHINX_LIB_MAIN_PATH)
        set(SPHINX_EXECUTABLE "${Python3_EXECUTABLE};-m;sphinx")
    endif()
endif()


##Handle standard arguments to find_package like REQUIRED and QUIET
find_package_handle_standard_args(Sphinx
        REQUIRED_VARS SPHINX_EXECUTABLE
        FAIL_MESSAGE "Failed to find sphinx-build executable or sphinx library"
)