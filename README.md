![](resources/images/torrenttools.svg)

![build](https://github.com/fbdtemme/torrenttools/workflows/build/badge.svg)
[![Copr build status](https://copr.fedorainfracloud.org/coprs/fbdtemme/torrenttools/package/torrenttools/status_image/last_build.png)](https://copr.fedorainfracloud.org/coprs/fbdtemme/torrenttools/package/torrenttools/)
[![GitHub release (latest SemVer)](https://img.shields.io/github/v/release/fbdtemme/torrenttools)](https://github.com/fbdtemme/torrenttools/releases)
[![C++ standard](https://img.shields.io/badge/C%2B%2B-20-blue)](https://isocpp.org/)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/5cc3eec94d8a486dab62afeab5130def)](https://app.codacy.com/manual/floriandetemmerman/torrenttools?utm_source=github.com&utm_medium=referral&utm_content=fbdtemme/bencode&utm_campaign=Badge_Grade_Dashboard)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

A commandline tool for creating, inspecting and modifying bittorrent metafiles.

[**Features**](#Status) |
[**Documentation**](#Documentation) |
[**Packages**](#Packages) |
[**Building**](#Building) |
[**License**](#License)

## Features

* Creating bittorrent metafiles. 
* Inspecting bittorrent metafiles.
* Verifying bittorrent metafiles against local data.
* Editing existing bittorrent metafiles.
* Support for the new [v2 and hybrid protocols](https://blog.libtorrent.org/2020/09/bittorrent-v2/) .
* Support for tracker abbreviations.
* Support for announce substitution parameters. 

## Status

This project is under development. 
The commandline interface can change at any release prior to 1.0.0.

## Documentation

Documentation is hosted on [Github Pages](https://fbdtemme.github.io/torrenttools/).


## Packages

Binary and source packages for Fedora 32, Fedora 33, and Fedora Rawhide are available in a COPR repo.

```shell
sudo dnf copr enable fbdtemme/torrenttools
sudo dnf install torrenttools
```

Binary and source packages for Ubuntu 20.04, Ubuntu 20.10, Ubuntu 21.04 are available in a launchpad repo.

```shell
sudo add-apt-repository ppa:fbdtemme/torrenttools
sudo apt-get update
sudo apt install torrenttools
```


## Building

This library depends on following projects:

*  [CLI11](https://github.com/CLIUtils/CLI11)
*  [Catch2](https://github.com/catchorg/Catch2)
*  [CTRE](https://github.com/hanickadot/compile-time-regular-expressions)
*  [gsl-lite](https://github.com/gsl-lite/gsl-lite)
*  [RE2](https://github.com/google/re2)
*  [expected-lite](https://github.com/martinmoene/expected-lite)
*  [fmt](https://github.com/fmtlib/fmt)
*  [nlohmann/json](https://github.com/nlohmann/json)
*  [yaml-cpp](https://github.com/jbeder/yaml-cpp)
*  [bencode](https://github/com/fbdtemme/bencode)
*  [date](https://github.com/HowardHinnant/date)
*  [OpenSSL](https://github.com/openssl/openssl) or [libgcrypt](https://github.com/gpg/libgcrypt)

Almost all dependencies can be fetched from github during configure time or can be installed manually.
oneTBB and OpenSSL (or libgcrypt if so configured) have to be installed on the system in advance.

### Installing build dependencies

Ubuntu 20.04

```shell
sudo apt install build-essential git cmake g++-10 libtbb2 libtbb-dev libssl-dev 
```

Fedora 33
```shell
sudo dnf install cmake make g++ git openssl-devel libtbb-devel 
```



This project requires C++20.
Currently only GCC 10 is supported.

This project can be build as every other project which makes use of the CMake build system.

```{bash}
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --target torrenttools
```

Installing the project:

```{bash}
sudo cmake --install . --component torrentttools
```

## License

Distributed under the MIT license. See `LICENSE` for more information.
