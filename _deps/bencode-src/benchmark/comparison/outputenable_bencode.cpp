#include <chrono>

#include <ranges>
#include <string_view>
#include <fstream>
#include <filesystem>

#include <benchmark/benchmark.h>
#include <span>

#include <contagiousbt/bencode/outline/descriptor.hpp>
#include <contagiousbt/bencode/outline/object_def.hpp>
#include <contagiousbt/bencode/status.hpp>
#include "gsl/span"

#include "resources.hpp"

namespace bencode = contagiousbt::bencode;
namespace outline = bencode::outline;


//static void BM_decode_value(benchmark::State& state, const std::filesystem::path& path) {
//    auto ifs = std::ifstream(path, std::ifstream::binary);
//    std::string torrent(
//            std::istreambuf_iterator<char>{ifs},
//            std::istreambuf_iterator<char>{});
//
//    static std::size_t max_depth = 100;
//    for (auto _ : state) {
//        benchmark::DoNotOptimize();
//        benchmark::ClobberMemory();
//    }
//    state.SetBytesProcessed(state.iterations() * torrent.size());
//}


static void BM_decode_view(benchmark::State& state, const std::filesystem::path& path) {
    auto ifs = std::ifstream(path, std::ifstream::binary);
    std::string torrent(
            std::istreambuf_iterator<char>{ifs},
            std::istreambuf_iterator<char>{});

    int ec{};

    for (auto _ : state) {
        contagiousbt::bencode::outline::object<> obj;
        benchmark::DoNotOptimize(obj.outline(gsl::span(torrent.data(), torrent.size()), 1, ec));
        benchmark::ClobberMemory();
    }
    state.SetBytesProcessed(state.iterations() * torrent.size());
}

//BENCHMARK_CAPTURE(BM_decode_value, "ubuntu",      resource::ubuntu);
//BENCHMARK_CAPTURE(BM_decode_value, "covid",       resource::covid);
//BENCHMARK_CAPTURE(BM_decode_value, "camelyon17",  resource::camelyon17);
//BENCHMARK_CAPTURE(BM_decode_value, "pneumonia",   resource::pneumonia);
//BENCHMARK_CAPTURE(BM_decode_value, "integers",    resource::integers);

BENCHMARK_CAPTURE(BM_decode_view, "ubuntu",      resource::ubuntu);
BENCHMARK_CAPTURE(BM_decode_view, "covid",       resource::covid);
BENCHMARK_CAPTURE(BM_decode_view, "camelyon17",  resource::camelyon17);
BENCHMARK_CAPTURE(BM_decode_view, "pneumonia",   resource::pneumonia);
BENCHMARK_CAPTURE(BM_decode_view, "integers",   resource::integers);

BENCHMARK_MAIN();