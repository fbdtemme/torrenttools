#include <chrono>

#include <ranges>
#include <string_view>
#include <fstream>
#include <filesystem>

#include <stdexcept>
#include <benchmark/benchmark.h>
#include "Bencode.h"
#include "resources.hpp"


static void BM_decode_value(benchmark::State& state, const std::filesystem::path& path) {
    auto ifs = std::ifstream(path, std::ifstream::binary);
    std::string torrent(
            std::istreambuf_iterator<char>{ifs},
            std::istreambuf_iterator<char>{});

    for (auto _ : state) {
        benchmark::DoNotOptimize(bencode::DecodeBencodeElement(torrent));
        benchmark::ClobberMemory();
    }
    state.SetBytesProcessed(state.iterations() * torrent.size());
}


BENCHMARK_CAPTURE(BM_decode_value, "ubuntu",     resource::ubuntu);
BENCHMARK_CAPTURE(BM_decode_value, "covid",      resource::covid);
BENCHMARK_CAPTURE(BM_decode_value, "camelyon17", resource::camelyon17);
BENCHMARK_CAPTURE(BM_decode_value, "pneumonia",  resource::pneumonia);
BENCHMARK_CAPTURE(BM_decode_value, "integers",   resource::integers);

BENCHMARK_MAIN();