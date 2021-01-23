#include <chrono>

#include <ranges>
#include <string_view>
#include <fstream>
#include <filesystem>

#include <benchmark/benchmark.h>
#include "jimporter/bencode.hpp"

#include "resources.hpp"

static void BM_decode_value(benchmark::State& state, const std::filesystem::path& path) {
    auto ifs = std::ifstream(path, std::ifstream::binary);
    std::string torrent(
            std::istreambuf_iterator<char>{ifs},
            std::istreambuf_iterator<char>{});

    for (auto _ : state) {
        benchmark::DoNotOptimize(bencode::decode(torrent));
        benchmark::ClobberMemory();
    }
    state.SetBytesProcessed(state.iterations() * torrent.size());
}

static void BM_decode_view(benchmark::State& state, const std::filesystem::path& path) {
    auto ifs = std::ifstream(path, std::ifstream::binary);
    std::string torrent(
            std::istreambuf_iterator<char>{ifs},
            std::istreambuf_iterator<char>{});

    for (auto _ : state) {
        benchmark::DoNotOptimize(bencode::decode_view(torrent));
        benchmark::ClobberMemory();
    }
    state.SetBytesProcessed(state.iterations() * torrent.size());
}


BENCHMARK_CAPTURE(BM_decode_value, "ubuntu",     resource::ubuntu);
BENCHMARK_CAPTURE(BM_decode_value, "covid",      resource::covid);
BENCHMARK_CAPTURE(BM_decode_value, "camelyon17", resource::camelyon17);
BENCHMARK_CAPTURE(BM_decode_value, "pneumonia",  resource::pneumonia);
BENCHMARK_CAPTURE(BM_decode_value, "integers",   resource::integers);

BENCHMARK_CAPTURE(BM_decode_view, "ubuntu",     resource::ubuntu);
BENCHMARK_CAPTURE(BM_decode_view, "covid",      resource::covid);
BENCHMARK_CAPTURE(BM_decode_view, "camelyon17", resource::camelyon17);
BENCHMARK_CAPTURE(BM_decode_view, "pneumonia",  resource::pneumonia);
BENCHMARK_CAPTURE(BM_decode_view, "integers",   resource::integers);

BENCHMARK_MAIN();