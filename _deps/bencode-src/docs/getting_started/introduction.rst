.. cpp:namespace:: bencode

Introduction
============
Design goals
------------

There are a few C++ bencode projects available online but many are limited in scope.
The goal of this library is to provide first class support for bencode and to provide all
features expected of a modern C++ serialization/deserialization library.

Features
--------
*  Convenient owning representation of bencoded data with `bvalue`.
*  Fast and memory efficient read-only, non-owning representation into stable buffers
   of bencoded data with `bview`.
*  Build-in serialization/deserializaton for most standard containers.
*  Support for serializing/deserializing to/from user-defined types.
*  Parse directly to custom types by satisfying the `EventConsumer` concept.
*  Throwing and non throwing variants of common functions.
*   Bencode pointer similar to json pointer.


Requirements
------------

Compiler support
++++++++++++++++

This library requires C++20.
Compiler support is limited to GCC 10 and MinGW-w64 10 at the moment.

Dependencies
++++++++++++

This library depends on following projects:

* `fmt <https://github.com/fmtlib/fmt>`_
* `gsl-lite <https://github.com/gsl-lite/gsl-lite>`_
* `expected-lite <https://github.com/martinmoene/expected-lite>`_

When building tests:

* `Catch2 <https://github.com/catchorg/Catch2>`_
* `google-benchmark <https://github.com/google/benchmark>`_, when building benchmarks

When building benchmarks:

* `google-benchmark <https://github.com/google/benchmark>`_
* `libtorrent <https://github.com/arvidn/libtorrent>`_
* `jimporter/bencode <https://github.com/jimporter/bencode>`_
* `s3rvac/cpp-bencoding <https://github.com/s3rvac/cpp-bencoding>`_

When building documentation:

* `doxygen <https://github.com/doxygen/doxygen>`_
* `sphinx <https://github.com/sphinx-doc/sphinx>`_
* `breathe <https://github.com/michaeljones/breathe>`_

All dependencies for building tests and benchmarks can be fetched from github using
cmake FetchContent during configuration if no local installation is found.
