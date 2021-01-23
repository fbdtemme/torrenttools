#include <chrono>

#include <ranges>
#include <string_view>
#include <fstream>
#include <filesystem>

#include <benchmark/benchmark.h>
#include <span>
#include <libtorrent/bdecode.hpp>

#include "resources.hpp"

static void BM_decode_view(benchmark::State& state, const std::filesystem::path& path) {
    auto ifs = std::ifstream(path, std::ifstream::binary);
    std::string torrent(
            std::istreambuf_iterator<char>{ifs},
            std::istreambuf_iterator<char>{});

    auto s = std::span(torrent);
    for (auto _ : state) {
        benchmark::DoNotOptimize(libtorrent::bdecode(s));
        benchmark::ClobberMemory();
    }
    state.SetBytesProcessed(state.iterations() * torrent.size());
}

BENCHMARK_CAPTURE(BM_decode_view, "ubuntu",      resource::ubuntu);
BENCHMARK_CAPTURE(BM_decode_view, "covid",       resource::covid);
BENCHMARK_CAPTURE(BM_decode_view, "camelyon17",  resource::camelyon17);
BENCHMARK_CAPTURE(BM_decode_view, "pneumonia",   resource::pneumonia);
BENCHMARK_CAPTURE(BM_decode_view, "integers",    resource::integers);

BENCHMARK_MAIN();