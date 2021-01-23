add_test(NAME cmake_import_configure
        COMMAND ${CMAKE_COMMAND}
        -G "${CMAKE_GENERATOR}"
        -A "${CMAKE_GENERATOR_PLATFORM}"
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        -Dbencode_DIR=${PROJECT_BINARY_DIR}
        -Dgsl-lite_DIR=${gsl-lite_BINARY_DIR}
        -Dexpected-lite_DIR=${expected-lite_BINARY_DIR}
        -Dfmt_DIR=${fmt_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/project
        )
add_test(NAME cmake_import_build
        COMMAND ${CMAKE_COMMAND} --build .
        )
set_tests_properties(cmake_import_configure PROPERTIES
        FIXTURES_SETUP cmake_import
        )
set_tests_properties(cmake_import_build PROPERTIES
        FIXTURES_REQUIRED cmake_import
        )