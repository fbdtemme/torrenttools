![](docs/images/torrenttools.svg)

![build](https://github.com/fbdtemme/torrenttools/workflows/build/badge.svg)
[![Copr build status](https://copr.fedorainfracloud.org/coprs/fbdtemme/torrenttools/package/torrenttools/status_image/last_build.png)](https://copr.fedorainfracloud.org/coprs/fbdtemme/torrenttools/package/torrenttools/)
[![GitHub release (latest SemVer)](https://img.shields.io/github/v/release/fbdtemme/torrenttools)](https://github.com/fbdtemme/torrenttools/releases)
[![C++ standard](https://img.shields.io/badge/C%2B%2B-20-blue)](https://isocpp.org/)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/5cc3eec94d8a486dab62afeab5130def)](https://app.codacy.com/manual/floriandetemmerman/torrenttools?utm_source=github.com&utm_medium=referral&utm_content=fbdtemme/bencode&utm_campaign=Badge_Grade_Dashboard)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

A commandline tool for creating, inspecting and modifying bittorrent metafiles.

[**Features**](#Status) |
[**Examples**](#Examples) |
[**Building**](#Building) |
[**License**](#License)

## Features

* Creating bittorrent metafiles. 
* Inspecting bittorrent metafiles.
* Verifying bittorrent metafiles against local data.
* Support for the new [v2 and hybrid protocols](https://blog.libtorrent.org/2020/09/bittorrent-v2/) .
* Support for tracker abbreviations.
* Support for announce parameters. 

## Status

This project is under development. 
The commandline interface can change at any release prior to 1.0.0.

## Examples

### Info command
```shell
torrenttools info bittorrent-v2-hybrid-test.torrent
```
```
Metafile:         bittorrent-v2-hybrid-test.torrent
Protocol version: v1 + v2 (hybrid)
Infohash:         v1: 6ee14afe327f36a0e920b6e5393db3d959ed2f75
                  v2: de7aacff381db1b48c3a4de4fb012e6a31a7716b1d5b64ebb83ebc0d8a04787e
Piece size:       512 KiB (524288 bytes)
Created by:       libtorrent
Created on:       2020-06-03 08:45:06 UTC
Private:          false
Name:             bittorrent-v1-v2-hybrid-test
Source:           
Comment:          

Announces:

Files:
  bittorrent-v1-v2-hybrid-test
  ├── [6.23 MiB] Darkroom (Stellar, 1994, Amiga ECS) HQ.mp4
  ├── [19.6 MiB] Spaceballs-StateOfTheArt.avi
  ├── [ 326 MiB] cncd_fairlight-ceasefire_(all_falls_down)-1080p.mp4
  ├── [58.8 MiB] eld-dust.mkv
  ├── [ 265 MiB] fairlight_cncd-agenda_circling_forth-1080p30lq.mp4
  ├── [42.5 MiB] meet the deadline - Still _ Evoke 2014.mp4
  ├── [61.0   B] readme.txt
  ├── [25.1 MiB] tbl-goa.avi
  └── [ 111 MiB] tbl-tint.mpg

  854.06 MiB in 1 directories, 17 files
```

### Create command
```shell
torrenttools create -h
```
```
Create bittorrent metafiles.
Usage: torrenttools create [OPTIONS] target

Positionals:
  target <path>               Target filename or directory

Options:
  -h,--help                   Print this help message and exit
  -v,--protocol <protocol>    Set the bittorrent protocol to use. Options are 1, 2 or hybrid. [default: 1]
  -o,--output <path>          Set the filename and/or output directory of the created file. [default: <name>.torrent]
                              Use a path with trailing slash to only set the output directory.
  -a,--announce <url> ...     Add one or multiple announces urls.
                              Multiple trackers will be added in seperate tiers by default. 
                              Use square brackets to groups urls in a single tier:
                               eg. "--announce url1 [url1 url2]"
  -w,--web-seed <url> ...     Add one or multiple HTTP/FTP urls as seeds.
  -d,--dht-node <host:port> ...
                              Add one or multiple DHT nodes.
  -c,--comment <comment>      Add a comment.
  -p,--private                Set the private flag to disable DHT and PEX.
  -l,--piece-length <size[K|M]>
                              Set the piece size.
                              When no unit is specified block size will be 2^<n> bytes.
                              Piece size must be a power of two in range [16K, 64M].
                              Leave empty to determine by total file size. [default: auto]
  -s,--source <source>        Add a source tag to facilitate cross-seeding.
  -n,--name <name>            Set the name of the torrent. This changes the filename for single file torrents 
                              or the root directory name for multi-file torrents.
                              [default: <basename of target>]
  -t,--threads <n>            Set the number of threads to use for hashing pieces. [default: 2]
  --checksum <algorithm> ...  Include a per file checksum of given algorithm.
  --no-creation-date          Do not include the creation date.
  --no-created-by             Do not include the name and version of this program.
  --include <regex> ...       Only add files matching given regex to the metafile.
  --exclude <regex> ...       Do not add files matching given regex to the metafile.
  --include-hidden            Do not skip hidden files.


````


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
*  [oneTBB](https://github.com/oneapi-src/oneTBB)
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

### Installation

```{bash}
sudo cmake --install . --component torrentttools
```

## Installing from binary packages

There are binary packages for Fedora 32, Fedora 33, and Fedora rawhide available in the COPR repo.

```shell
sudo dnf copr enable fbdtemme/torrenttools
sudo dnf install torrenttools
```

Binary packages for ubuntu 18.04 and ubuntu 20.04 are available on the release page.

## Configuration files

`torrenttools` will first try to load user-specific configuration files and fall back
to system global configuration files if no user specific config is found.

There are two configuration files (located under resources in the source tree):
* Tracker database: `trackers.json`
* User settings: `config.yaml`

`torrenttools` will look for these files in the following locations:
* System location: `/etc/torrenttools`
* User location:   `$HOME/.config/torrentoools`


## License

Distributed under the MIT license. See `LICENSE` for more information.
