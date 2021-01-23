![](docs/images/bencode.svg)

[![build](https://github.com/fbdtemme/bencode/workflows/build/badge.svg?branch=master)](https://github.com/fbdtemme/bencode/actions?query=workflow%3Abuild)
[![docs](https://github.com/fbdtemme/bencode/workflows/documentation/badge.svg?branch=master)](https://fbdtemme.github.io/bencode/)
[![santizers](https://github.com/fbdtemme/bencode/workflows/sanitizers/badge.svg?branch=master)](https://github.com/fbdtemme/bencode/actions?query=workflow%3Asanitizers)
[![GitHub release (latest SemVer)](https://img.shields.io/github/v/release/fbdtemme/bencode)](https://github.com/fbdtemme/bencode/releases)
[![C++ standard](https://img.shields.io/badge/C%2B%2B-20-blue)](https://isocpp.org/)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/5cc3eec94d8a486dab62afeab5130def)](https://app.codacy.com/manual/floriandetemmerman/bencode?utm_source=github.com&utm_medium=referral&utm_content=fbdtemme/bencode&utm_campaign=Badge_Grade_Dashboard)
[![codecov](https://codecov.io/gh/fbdtemme/bencode/branch/master/graph/badge.svg)](https://codecov.io/gh/fbdtemme/bencode)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

[**Features**](#Features) |
[**Status**](#Status) |
[**Documentation**](#Documentation) |
[**Performance**](#Performance) |
[**Examples**](#Examples) |
[**Building**](#Building) | 
[**Integration**](#Integration) |
[**License**](#License)

A header-only C++20 bencode serialization/deserialization library.

## Features

* **Feature-rich**. The main goal of this library is to provide a complete bencode library that 
  provides optimal solutions for all common use cases. `bvalue` is an owning representation of bencoded data
  and is usefull for creating and modifying bencoded documents. 
  `bview` is a fast and memory efficient, read-only, non-owning representation into a stable buffer of bencoded data.
  `bpointer` can be used to access both `bvalue` and `bview` types.
* **Extensibility**. This library provides built-in serialization and deserialization from/to most standard containers.
  Support for user-defined types can be added by implementing the necessary extension points.
  Users can parse directly to their data type of preference by implementing a class satisfying
  the EventConsumer concept.
* **Conformance**. This library is 100% conforming to the bencode specification.
  All parsers validate the input and provide exact error messages.
* **Security**. Parsing arbitrary user data can be dangerous and you do not want your bittorrent tracker 
  to crash when a user sends malformed data. All parsers are recursion-free to protect against
  stack-based buffer overflow attacks. Integer parsing throws when overflows are encountered.
* **Speed**. While not the primary goal of this project this library provides optimized integer parsing with
  SWAR techniques. Benchmarks show this library performs well in comparison with other libraries
* **Well-tested**. This project achieves 95% testing coverage. 
  We also run Sanitizers in a CI pipeline to check for leaks and undefined-behavior.

## Status
 
This library is under active development. The API may change at any release prior to 1.0.0.
Versioning follows the Semantic Versioning Specification.


## Documentation

Documentation is available on the [bencode GitHub pages](https://fbdtemme.github.io/bencode/)

## Performance

Decoding performance was benchmarked for both value and view types.
Value types own the data they refer to and thus need to copy data from the buffer with bencoded data.
View types try to minimize copies from the buffer with bencoded data and instead point
to data directly inside the buffer. 

Decoding speed was compared for these libraries in alphabetical order:

* [Aetf/QBencode](https://github.com/Aetf/QBencode)
* [arvidn/libtorrent](https://github.com/arvidn/libtorrent)
* [iRajul/bencode](https://github.com/iRajul/bencode)
* [jimporter/bencode.hpp](https://github.com/jimporter/bencode.hpp)
* [kriben/bencode](https://github.com/kriben/bencode)
* [outputenable/bencode](https://gitlab.com/outputenable/bencode)
* [rakshasa/libtorrent](https://github.com/rakshasa/libtorrent)
* [s3ponia/BencodeParser](https://github.com/s3ponia/BencodeParser)
* [s3rvac/cpp-bencoding](https://github.com/s3rvac/cpp-bencoding)
* [theanti9/cppbencode](https://github.com/theanti9/cppbencode)

#### Parsing to value types

![benchmark-decoding-value](docs/images/benchmark-decoding-value.svg)

#### Parsing to view types

![benchmark-decoding-view](docs/images/benchmark-decoding-view.svg)

All benchmarks were build with GCC 10.2.1 with -O3 and run on an intel i7-7700hq.

Notes:
* arvidn/libtorrent does not decode integers until they are actually accessed. 
  This gives a performance benefit when decoding but results in slower 
  access times when retrieving integral values.
* kriben/bencode support only 32-bit integers and fails on the
  camelyon17, integers and pneumomia benchmarks
* iRajul/bencode fails all benchmarks and was excluded from the results.

## Examples

```cpp
// All examples use namespace bc for brevity
namespace bc = bencode;
```

Decode bencoded data to `bvalue`.

```cpp
//#include <bencode/bencode.hpp> 

namespace bc = bencode;

bc::bvalue b = bc::decode_value("l3:fooi2ee");

// check if the first list element is a string
if (holds_list(b) && holds_string(b[0])) {
    std::cout << "success";
}

// type tag based type check, return false
bc::holds_alternative<bc::type::dict>(b); 

// access the first element of the list "foo" and move it 
// out of the bvalue into v1
std::string v1 = bc::get_string(std::move(b[0]));

// access the second element
std::size_t v2 = bc::get_integer(b[1]);
```

Serialize a `bvalue` to an output stream.

```cpp
bc::bvalue b{
  {"foo", 1},
  {"bar", 2},
  {"baz", bc::bvalue(bc::btype::list, {1, 2, 3})},
};

bc::encode_to(std::cout, b);
```

Retrieve data from a bvalue.

``` cpp
auto b = bc::bvalue(bc::type::list, {1, 2, 3, 4, 5, 6, 7, 8, 9});

// return a list of integers as a byte vector, throws on error
auto bytes = get_as<std::vector<std::byte>>(b);

// non throwing version with a std::expected type
auto res = try_get_as<std::vector<std::byte>>(b);

if (res.has_value()) {
    std::cout << res.value(); 
} else {
    std::cout << "error" << to_string(res.error());
}

```

Decode bencoded data to `bview`.

```cpp
//#include <bencode/bview.hpp> 

namespace bc = bencode;

// decode the data to a descriptor_table
bc::descriptor_table t = bc::decode_view("l3:fooi2ee");

// get the bview to the root element (ie the list) 
bc::bview b = t.get_root();

// access data and convert to std::size_t and std::string_view
std::string_view v1 = bc::get_string(b[0]);
std::size_t v2 = bc::get_integer(b[1]);
```



Serialize to bencode using `encoder`.

```cpp
#include <bencode/encode.hpp>
#include <bencode/traits/vector.hpp>    

bc::encoder es(std::cout);

es << bc::dict_begin
       << "foo" << 1UL
       << "bar" << bc::list_begin 
                    << bc::bvalue(1)
                    << "two" 
                    << 3
                << bc::list_end
       << "baz" << std::vector{1, 2, 3}
   << bc::dict_end;
```

Use bpointer to access values in a nested datastructure.

```cpp
bc::bvalue b {
    {"foo", 1},
    {"bar", 2},
    {"baz", bc::bvalue(bc::btype::list, {1, 2, 3})},
};

b.at("baz/2"_bpointer);
```

See the [documentation](https://fbdtemme.github.io/bencode/) for more examples. 

## Building

This project requires C++20.
Currently only GCC 10 or MinGW-w64 10 or later are supported.

This library uses following projects:
*   [fmt](https://github.com/fmtlib/fmt)
*   [gsl-lite](https://github.com/gsl-lite/gsl-lite)
*   [expected-lite](https://github.com/martinmoene/expected-lite)

When building tests:
*   [Catch2](https://github.com/catchorg/Catch2)

When building benchmarks:
*   [google/benchmark](https://github.com/google/benchmark)
*   [Boost](https://www.boost.org/)
*   [Qt5](https://www.qt.io/)


When building documentation:
* [doxygen](https://github.com/doxygen/doxygen>)
* [sphinx](https://github.com/sphinx-doc/sphinx>)
* [breathe](https://github.com/michaeljones/breathe>)
* [shphinx-rtd-theme](https://github.com/readthedocs/sphinx_rtd_theme)

All dependencies except for boost, Qt5 and documentation dependencies can be fetched from github using
cmake FetchContent during configuration if no local installation is found.

The tests can be built as every other project which makes use of the CMake build system.

```{bash}
mkdir build; cd build;
cmake \
  -DCMAKE_BUILD_TYPE=Debug \
  -DBENCODE_BUILD_TESTS=ON \
  -DBENCODE_BUILD_BENCHMARKS=OFF \
  -DBENCODE_BUILD_DOCS=OFF \
   --build . --target .. 
make bencode-tests
```

The library can be installed as a CMake package.
```bash
cmake -DBENCODE_BUILD_TESTS=OFF \
      -DBENCODE_BUILD_BENCHMARKS=OFF \
      -DBENCODE_BUILD_DOCS=OFF --build . --target ..
sudo make install
```

## Integration

You can use the `bencode::bencode` interface target in CMake.
The library can be located with `find_package`.

```cmake
# CMakeLists.txt
find_package(bencode REQUIRED)
...
add_library(foo ...)
...
target_link_libraries(foo INTERFACE bencode::bencode)
```

The source tree can be included in your project and added to your build with `add_subdirectory`.

```cmake
# CMakeLists.txt
add_subdirectory(bencode)
...
add_library(foo ...)
...
target_link_libraries(foo INTERFACE bencode::bencode)
```

You can also use `FetchContent` to download the source code from github.
    
```cmake
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
```

## License

Distributed under the MIT license. See `LICENSE` for more information.