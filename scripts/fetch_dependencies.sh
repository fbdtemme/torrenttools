#!/bin/env bash

function nest {
  repo_url="$1"
  branch="$2"
  destination_name="$3"

  repo_name_plus_git=$(basename $repo_url)
  repo_name="${repo_name_plus_git%.git}"

  cd "external"
  git clone --branch "$branch" --single-branch "$repo_url"
  if [[ $repo_name != $destination_name ]]; then
    mv "$repo_name" "$destination_name"
  fi
  cd "../"
}


#lp:~fbdtemme/torrenttools/+git/torrenttools main
#merge packaging lp:~fbdtemme/torrenttools/+git/torrenttools deb-packaging
nest https://github.com/CLIUtils/CLI11.git master CLI11
nest https://github.com/fmtlib/fmt.git master fmt
nest https://github.com/gsl-lite/gsl-lite.git master gsl-lite
nest https://github.com/martinmoene/expected-lite.git master expected-lite
nest https://github.com/catchorg/Catch2.git v2.x Catch2
nest https://github.com/google/re2.git main re2
nest https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent.git master nlohmann_json
nest https://github.com/jbeder/yaml-cpp.git master yaml-cpp
nest https://github.com/fbdtemme/termcontrol.git main termcontrol
nest https://github.com/hanickadot/compile-time-regular-expressions.git master ctre
nest https://github.com/fbdtemme/cliprogress.git main cliprogress
nest https://github.com/palacaze/sigslot.git master sigslot
nest https://github.com/fbdtemme/bencode.git master bencode
nest https://github.com/fbdtemme/dottorrent.git v0.5.0 dottorrent
nest https://github.com/HowardHinnant/date.git master date
nest https://github.com/intel/isa-l_crypto.git master isa-l_crypto