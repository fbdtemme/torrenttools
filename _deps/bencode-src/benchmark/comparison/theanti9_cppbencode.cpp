#include <chrono>

#include <ranges>
#include <string_view>
#include <fstream>
#include <filesystem>

#include <benchmark/benchmark.h>
#include <span>

#include "bencode.h"
#include "resources.hpp"

static void BM_decode_value(benchmark::State& state, const std::filesystem::path& path) {
    auto ifs = std::ifstream(path, std::ifstream::binary);
    std::string torrent(
            std::istreambuf_iterator<char>{ifs},
            std::istreambuf_iterator<char>{});

    static std::size_t max_depth = 100;
    for (auto _ : state) {
        ben::variant data;
        benchmark::DoNotOptimize(ben::decoder::decode_all(&data, torrent));
        benchmark::ClobberMemory();
    }
    state.SetBytesProcessed(state.iterations() * torrent.size());
}

BENCHMARK_CAPTURE(BM_decode_value, "ubuntu",      resource::ubuntu);
BENCHMARK_CAPTURE(BM_decode_value, "covid",       resource::covid);
BENCHMARK_CAPTURE(BM_decode_value, "camelyon17",  resource::camelyon17);
BENCHMARK_CAPTURE(BM_decode_value, "pneumonia",   resource::pneumonia);
BENCHMARK_CAPTURE(BM_decode_value, "integers",    resource::integers);

BENCHMARK_MAIN();