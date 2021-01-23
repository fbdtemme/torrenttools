Integration
===========

CMake
-----

External
++++++++

You can use the bencode::bencode interface target in CMake.
The library can be located with find_package() and use the namespaced imported target.

.. code-block:: cmake

    # CMakeLists.txt
    find_package(bencode 0.1.0 REQUIRED)
    ...
    add_library(foo ...)
    ...
    target_link_libraries(foo INTERFACE bencode::bencode)

Embedded
++++++++

The source tree can be included in your project and included with :code:`add_subdirectory`

.. code-block::

    # CMakeLists.txt
    # Disable building tests and benchmarks.
    set(BENCODE_BUILD_TESTS OFF)
    set(BENCODE_BUILD_BENCHMARKS OFF)

    add_subdirectory(bencode)
    ...
    add_library(foo ...)
    ...
    target_link_libraries(foo INTERFACE bencode::bencode)

Embedded (FetchContent)
+++++++++++++++++++++++

.. code-block::

    # CMakeLists.txt
    include(FetchContent)

    FetchContent_Declare(bencode
      GIT_REPOSITORY https://github.com/fbdtemme/bencode.git
      GIT_TAG "master")

    FetchContent_MakeAvailable(bencode)
    ...
    add_library(foo ...)
    ...
    target_link_libraries(foo INTERFACE bencode::bencode)
