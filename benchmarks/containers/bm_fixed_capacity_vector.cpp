#include <benchmark/benchmark.h>
#include <cstdint>
#include <cpp_playground/containers/fixed_capacity_vector.hpp>

static void BM_std_vector_creation(benchmark::State& state) {
    for(auto _ : state) {
        std::vector<uint64_t> vector;
        vector.reserve(state.range(0));
        benchmark::DoNotOptimize(vector.data());
        benchmark::ClobberMemory();
    }
}

template<std::size_t Capacity>
static void BM_fixed_capacity_vector_creation(benchmark::State& state) {
    for(auto _ : state) {
        CppPlayground::FixedCapacityVector<uint64_t, Capacity> vector;
        benchmark::DoNotOptimize(vector.data());
        benchmark::ClobberMemory();
        // std::vector<uint64_t> vector;
        // vector.reserve(state.range(0));
        // benchmark::DoNotOptimize(vector.data());
        // benchmark::ClobberMemory();
    }
}

// BENCHMARK(BM_std_vector_creation)->RangeMultiplier(2)->Range(1UL<<3UL, 1UL<<20UL);
BENCHMARK(BM_std_vector_creation)->RangeMultiplier(2)->Range(1UL<<3UL, 1<<4UL);
BENCHMARK(BM_fixed_capacity_vector_creation<8>);
BENCHMARK(BM_fixed_capacity_vector_creation<16>);
BENCHMARK(BM_fixed_capacity_vector_creation<32>);
BENCHMARK(BM_fixed_capacity_vector_creation<64>);
BENCHMARK(BM_fixed_capacity_vector_creation<128>);
BENCHMARK(BM_fixed_capacity_vector_creation<256>);
