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
* Fast multi-buffer hashing with Intel ISA-L.

## Status

This project is under development. 
The commandline interface can change at any release prior to 1.0.0.

## Performance

Following test were performed on a RAM Disk with 1 MiB piece size 
and as target a 9.0 GiB [Centos8-stream image](http://isoredirect.centos.org/centos/8/isos/x86_64/).
The CPU is an Intel i7-7700HQ in a Dell XPS 15-9560 machine.

![Benchmark](benchmark/benchmark.svg)

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

A windows installer is available as an asset on the [release](https://github.com/fbdtemme/torrenttools/releases) page.


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
OpenSSL or Intel ISA-L crypto have to be installed on the system in advance.

### Installing build dependencies

Ubuntu 20.04

```shell
sudo apt install build-essential git cmake g++-10 libtbb2 libtbb-dev libssl-dev 
```

Fedora 33
```shell
sudo dnf install cmake make g++ git openssl-devel libtbb-devel
```

### Building Intel ISA-L crypto from source

```shell
wget https://github.com/intel/isa-l_crypto
cd isa-l_crypto
./autogen.sh
./configure
make
sudo make install
````

### Configuration

| Option                         |  Type    |  Description                 |
|--------------------------------|----------|------------------------------| 
| TORRENTTOOLS_BUILD_TESTS       | Bool     | Build tests.                 |
| TORRENTTOOLS_BUILD_DOCS        | Bool     | Build documentation.         |
| TORRENTTOOLS_INSTALL           | Bool     | Generate an install target.  |
| DOTTORRENT_CRYPTO_MULTIBUFFER  | Bool     | Enable fast multi buffer hashing. Requires Intel ISA-L Crypto library. |

### Building

This project requires C++20.
Currently only GCC 10 is supported.

This project can be build as every other project which makes use of the CMake build system.

```{bash}
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --target torrenttools
```

### Installation

Installing the project:

```{bash}
sudo cmake --install . --component torrentttools
```

## License

Distributed under the MIT license. See `LICENSE` for more information.
