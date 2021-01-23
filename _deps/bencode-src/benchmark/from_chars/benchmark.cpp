#include <algorithm>
#include <iostream>
#include <vector>
#include <array>
#include <random>
#include <charconv>

#include <benchmark/benchmark.h>

#include "bencode/detail/parser/from_chars.hpp"

static std::random_device rnd_device {};
static std::mt19937 mersenne_engine {rnd_device()};


static auto uint32_uniform = std::uniform_int_distribution<std::size_t>{
        std::numeric_limits<std::uint32_t>::min(),
        std::numeric_limits<std::uint32_t>::max()
};

static constexpr auto uint32_normal = [] (auto rng) {
    static auto distribution = std::normal_distribution<double>{0, 1e6};

    constexpr auto max_value = double(std::numeric_limits<std::uint32_t>::max());
    constexpr auto min_value = double(std::numeric_limits<std::uint32_t>::min());

    double v = distribution(rng);
    v = std::clamp(v, min_value, max_value);
    return std::uint32_t(v);
};

static constexpr auto uint32_exponential = [] (auto rng) {
    static auto distribution = std::exponential_distribution<double>{1/100};

    constexpr auto max_value = double(std::numeric_limits<std::uint32_t>::max());
    constexpr auto min_value = double(std::numeric_limits<std::uint32_t>::min());

    double v = distribution(rng);
    v = std::clamp(v, min_value, max_value);
    return std::uint32_t(v);
};


static auto uint64_uniform = std::uniform_int_distribution<std::size_t>{
        std::numeric_limits<std::uint64_t>::min(),
        std::numeric_limits<std::uint64_t>::max()
};

static constexpr auto uint64_normal = [] (auto rng) {
    static auto distribution = std::normal_distribution<double>{0, 1e12};

    constexpr auto max_value = double(std::numeric_limits<std::uint64_t>::max());
    constexpr auto min_value = double(std::numeric_limits<std::uint64_t>::min());

    double v = distribution(rng);
    v = std::clamp(v, min_value, max_value);
    return std::uint64_t(v);
};

static constexpr auto uint64_exponential = [] (auto rng) {
    static auto distribution = std::exponential_distribution<double>{1/10000};

    constexpr auto max_value = double(std::numeric_limits<std::uint64_t>::max());
    constexpr auto min_value = double(std::numeric_limits<std::uint64_t>::min());

    double v = distribution(rng);
    v = std::clamp(v, min_value, max_value);
    return std::uint64_t(v);
};

template <typename Distribution>
auto generate_test_data(Distribution d, int size)
{
    std::vector<std::pair<std::string, std::string_view>> data(size);
    std::generate(data.begin(), data.end(),
            [&]() {
                auto r = std::to_string(std::size_t(d(mersenne_engine)));
                auto size = r.size();
                r.resize(32, '\0');
                return std::pair(std::move(r), std::string_view(r.data(), size));
            });
    return data;
}


template <typename T>
static void BM_std_from_chars_uint32_t(benchmark::State& state, const T& distribution) {
    for (auto _ : state) {
        state.PauseTiming();
        auto data = generate_test_data(distribution, 100);
        state.ResumeTiming();
        std::uint32_t v;
        for (auto [first, second] : data) {
            benchmark::DoNotOptimize(std::from_chars(first.data(), first.data()+first.size(), v));
        }
    }
    state.SetItemsProcessed(state.iterations() * 100);
}


template <typename T>
static void BM_std_from_chars_uint64_t(benchmark::State& state, const T& distribution) {
    for (auto _ : state) {
        state.PauseTiming();
        auto data = generate_test_data(distribution, 100);
        state.ResumeTiming();
        std::uint64_t v;
        for (auto [first, second] : data) {
            benchmark::DoNotOptimize(std::from_chars(first.data(), first.data()+first.size(), v));
        }
    }
    state.SetItemsProcessed(state.iterations() * 100);
}

template <typename T>
static void BM_serial_from_chars_uint32_t(benchmark::State& state, const T& distribution) {
    using namespace bencode::detail;
    for (auto _ : state) {
        state.PauseTiming();
        auto data = generate_test_data(distribution, 100);
        state.ResumeTiming();
        std::uint32_t v;
        for (auto [first, second] : data) {
            benchmark::DoNotOptimize(
                    from_chars(
                            first.data(), first.data()+first.size(), v, implementation::serial));
        }
    }
    state.SetItemsProcessed(state.iterations() * 100);
}

template <typename T>
static void BM_serial_from_chars_uint64_t(benchmark::State& state, const T& distribution) {
    using namespace bencode::detail;
    for (auto _ : state) {
        state.PauseTiming();
        auto data = generate_test_data(distribution, 100);
        state.ResumeTiming();
        std::uint64_t v;
        for (auto [first, second] : data) {
            benchmark::DoNotOptimize(
                    from_chars(
                            first.data(), first.data()+first.size(), v, implementation::serial));
        }
    }
    state.SetItemsProcessed(state.iterations() * 100);
}

template <typename T>
static void BM_swar_from_chars_uint32_t(benchmark::State& state, const T& distribution) {
    using namespace bencode::detail;

    for (auto _ : state) {
        state.PauseTiming();
        auto data = generate_test_data(distribution, 100);
        state.ResumeTiming();
        std::uint32_t v;
        for (auto [first, second] : data) {
            benchmark::DoNotOptimize(from_chars(first.data(), first.data()+first.size(), v, implementation::swar));
        }
    }
    state.SetItemsProcessed(state.iterations() * 100);
}

template <typename T>
static void BM_swar_from_chars_uint64_t(benchmark::State& state, const T& distribution) {
    using namespace bencode::detail;

    for (auto _ : state) {
        state.PauseTiming();
        auto data = generate_test_data(distribution, 100);
        state.ResumeTiming();
        std::uint64_t v;
        for (auto [first, second] : data) {
            benchmark::DoNotOptimize(from_chars(first.data(), first.data()+first.size(), v, implementation::swar));
        }
    }
    state.SetItemsProcessed(state.iterations() * 100);
}

template <typename T>
static void BM_sse41_from_chars_uint32_t(benchmark::State& state, const T& distribution) {
    using namespace bencode::detail;

    for (auto _ : state) {
        state.PauseTiming();
        auto data = generate_test_data(distribution, 100);
        state.ResumeTiming();
        std::uint64_t v;
        for (auto [first, second] : data) {
            benchmark::DoNotOptimize(from_chars(first.data(), first.data()+first.size(), v, implementation::sse41));
        }
    }
    state.SetItemsProcessed(state.iterations() * 100);
}

template <typename T>
static void BM_sse41_from_chars_uint64_t(benchmark::State& state, const T& distribution) {
    using namespace bencode::detail;

    for (auto _ : state) {
        state.PauseTiming();
        auto data = generate_test_data(distribution, 100);
        state.ResumeTiming();
        std::uint64_t v;
        for (auto [first, second] : data) {
            benchmark::DoNotOptimize(from_chars(first.data(), first.data()+first.size(), v, implementation::sse41));
        }
    }
    state.SetItemsProcessed(state.iterations() * 100);
}

template <typename T>
static void BM_avx2_from_chars_uint32_t(benchmark::State& state, const T& distribution) {
    using namespace bencode::detail;

    for (auto _ : state) {
        state.PauseTiming();
        auto data = generate_test_data(distribution, 100);
        state.ResumeTiming();
        std::uint64_t v;
        for (auto [first, second] : data) {
            benchmark::DoNotOptimize(from_chars(first.data(), first.data()+first.size(), v, implementation::avx2));
        }
    }
    state.SetItemsProcessed(state.iterations() * 100);
}

template <typename T>
static void BM_avx2_from_chars_uint64_t(benchmark::State& state, const T& distribution) {
    using namespace bencode::detail;

    for (auto _ : state) {
        state.PauseTiming();
        auto data = generate_test_data(distribution, 100);
        state.ResumeTiming();
        std::uint64_t v;
        for (auto [first, second] : data) {
            benchmark::DoNotOptimize(from_chars(first.data(), first.data()+first.size(), v, implementation::avx2));
        }
    }
    state.SetItemsProcessed(state.iterations() * 100);
}

BENCHMARK_CAPTURE(BM_std_from_chars_uint32_t, "uniform",          uint32_uniform);
BENCHMARK_CAPTURE(BM_std_from_chars_uint32_t, "normal",           uint32_normal);
BENCHMARK_CAPTURE(BM_std_from_chars_uint32_t, "exponential",      uint32_exponential);
BENCHMARK_CAPTURE(BM_std_from_chars_uint64_t, "uniform",          uint64_uniform);
BENCHMARK_CAPTURE(BM_std_from_chars_uint64_t, "normal",           uint64_normal);
BENCHMARK_CAPTURE(BM_std_from_chars_uint64_t, "exponential",      uint64_exponential);

BENCHMARK_CAPTURE(BM_serial_from_chars_uint32_t, "uniform",          uint32_uniform);
BENCHMARK_CAPTURE(BM_serial_from_chars_uint32_t, "normal",           uint32_normal);
BENCHMARK_CAPTURE(BM_serial_from_chars_uint32_t, "exponential",      uint32_exponential);
BENCHMARK_CAPTURE(BM_serial_from_chars_uint64_t, "uniform",          uint64_uniform);
BENCHMARK_CAPTURE(BM_serial_from_chars_uint64_t, "normal",           uint64_normal);
BENCHMARK_CAPTURE(BM_serial_from_chars_uint64_t, "exponential",      uint64_exponential);

BENCHMARK_CAPTURE(BM_swar_from_chars_uint32_t, "uniform",          uint32_uniform);
BENCHMARK_CAPTURE(BM_swar_from_chars_uint32_t, "normal",           uint32_normal);
BENCHMARK_CAPTURE(BM_swar_from_chars_uint32_t, "exponential",      uint32_exponential);
BENCHMARK_CAPTURE(BM_swar_from_chars_uint64_t, "uniform",          uint64_uniform);
BENCHMARK_CAPTURE(BM_swar_from_chars_uint64_t, "normal",           uint64_normal);
BENCHMARK_CAPTURE(BM_swar_from_chars_uint64_t, "exponential",      uint64_exponential);

BENCHMARK_CAPTURE(BM_sse41_from_chars_uint32_t, "uniform",          uint32_uniform);
BENCHMARK_CAPTURE(BM_sse41_from_chars_uint32_t, "normal",           uint32_normal);
BENCHMARK_CAPTURE(BM_sse41_from_chars_uint32_t, "exponential",      uint32_exponential);
BENCHMARK_CAPTURE(BM_sse41_from_chars_uint64_t, "uniform",          uint64_uniform);
BENCHMARK_CAPTURE(BM_sse41_from_chars_uint64_t, "normal",           uint64_normal);
BENCHMARK_CAPTURE(BM_sse41_from_chars_uint64_t, "exponential",      uint64_exponential);

BENCHMARK_CAPTURE(BM_avx2_from_chars_uint32_t, "uniform",          uint32_uniform);
BENCHMARK_CAPTURE(BM_avx2_from_chars_uint32_t, "normal",           uint32_normal);
BENCHMARK_CAPTURE(BM_avx2_from_chars_uint32_t, "exponential",      uint32_exponential);
BENCHMARK_CAPTURE(BM_avx2_from_chars_uint64_t, "uniform",          uint64_uniform);
BENCHMARK_CAPTURE(BM_avx2_from_chars_uint64_t, "normal",           uint64_normal);
BENCHMARK_CAPTURE(BM_avx2_from_chars_uint64_t, "exponential",      uint64_exponential);

BENCHMARK_MAIN();