# dottorrent

A library for working with BitTorrent metafiles.

## Status

This library is in development and not suitable for general use.

## Building

This project requires C++20.
Currently only GCC 10 is supported.

This project can be build with CMake.

```{bash}
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make 
```

The library can be installed as a CMake package.

```
make install
```