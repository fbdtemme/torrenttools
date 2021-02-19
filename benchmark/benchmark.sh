#!/usr/bin/env bash

program_name=$1
working_dir=$2
target=$3
threads=$4

function benchmark_mktorrent()
{
    target="$1"
    threads="$2"
    rm *.torrent
    out="$(time mktorrent -t$threads -l20 "$target" 1> /dev/null 2> /dev/null)";
    echo "$out"
    return;
}

function benchmark_torrenttools_openssl()
{
    target="$1"
    threads="$2"
    rm *.torrent
    # gdbserver :1234 torrenttools-release create -t$threads -l20 "$target"
    out="$(time torrenttools-release create -t$threads -l20 "$target" 1> /dev/null 2> /dev/null)";
    echo "$out"
    return;
}

function benchmark_torrenttools_isal()
{
    target="$1"
    threads="$2"
    rm *.torrent
    # torrenttools-isal-release create -t$threads -l20 "$target" 2>&1
    out="$(time torrenttools-isal-release create -t$threads -l20 "$target" 1> /dev/null 2> /dev/null)";
    echo "$out"
    return;
}


function benchmark_imdl()
{
    target="$1"
    threads="$2"
    rm *.torrent
    out="$(time imdl torrent create --piece-length 1MiB "$target" 1> /dev/null 2> /dev/null)";
    echo "$out"
    return;
}

function benchmark_dottorrent_cli()
{
    target="$1"
    threads="$2"
    rm *.torrent
    out="$(time dottorrent --piece_size 1M  "$target" "output.torrent" 1> /dev/null 2> /dev/null)";
    echo "$out"
    return;
}

function benchmark_pyrocore()
{
    target="$1"
    threads="$2"
    rm *.torrent
    out="$(time mktor --chunk-min=1M --chunk-max=1M "$target" https://testurl.com/announce 1> /dev/null 2> /dev/null)";
    echo "$out"
    return;
}


function benchmark_transmission()
{
    target="$1"
    threads="$2"
    rm *.torrent
    out="$(time transmission-create --piece-size 1024 "$target" 1> /dev/null 2> /dev/null)";
    echo "$out"
    return;
}


cd "$working_dir"

if [ $program_name == "mktorrent" ]; then
    # echo "mktorrent"
    benchmark_mktorrent "$target" "$threads"
elif [ $program_name == "torrenttools_openssl" ]; then
    benchmark_torrenttools_openssl "$target" "$threads"
elif [ $program_name == "torrenttools_isal" ]; then
    benchmark_torrenttools_isal "$target" "$threads"
elif [ $program_name == "imdl" ]; then
    benchmark_imdl "$target" "$threads"
elif [ $program_name == "pyrocore" ]; then
    benchmark_pyrocore "$target" "$threads"
elif [ $program_name == "dottorrent-cli" ]; then
    benchmark_dottorrent_cli "$target" "$threads"   
elif [ $program_name == "transmission-create" ]; then
    benchmark_transmission "$target" "$threads"
fi