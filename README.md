![](docs/images/torrenttools.svg)

A commandline tool for creating, inspecting and modifying bittorrent metafiles.

[**Features**](#Status) |
[**Status**](#Status) |
[**Examples**](#Examples) |
[**License**](#License)

## Features

* Creating bittorrent metafiles. 
* Inspecting bittorrent metafiles.
* Experimental support for the new [v2 and hybrid protocols](https://blog.libtorrent.org/2020/09/bittorrent-v2/) .

## Roadmap

* Editing torrent metadata.
* Verifying torrents against local files.

## Status

This project is under development.

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

All dependencies can be fetched from github during configure time or can be installed manually.


This project requires C++20.
Currently only GCC 10 is supported.

This project can be build as every other project which makes use of the CMake build system.

```{bash}
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make 
```

## Installation

```{bash}
sudo make install
```

## Configuration files

`torrenttools` will first try to load user-specific configuration files and fall back
to system global configuration files if no user specific config is found.

* System location: `$INSTALL_PREFIX/share/torrenttools`
* User location:   `$HOME/.local/share/torrentoools`


## License

Distributed under the MIT license. See `LICENSE` for more information.
