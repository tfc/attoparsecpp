#include <cassert>
#include <sstream>

#include <attoparsecpp/parser.hpp>
#include <attoparsecpp/math_expression.hpp>

#include <benchmark/benchmark.h>

BENCHMARK_MAIN()

using namespace apl;

constexpr size_t max_range {10000000};

std::string self_concat(const char *s, size_t times) {
    std::ostringstream ss;
    while (times--) { ss << s; }
    return ss.str();
}

static void measure_word_parsing(benchmark::State &state) {
    const size_t size {static_cast<size_t>(state.range(0))};
    const auto p {many(noneOf(' '))};
    const std::string s {self_concat("a", size)};

    for (auto _ : state) {
        auto r {parse_result(p, s)};
        benchmark::DoNotOptimize(r->data());
    }
    state.SetComplexityN(state.range(0));
}

BENCHMARK(measure_word_parsing)->RangeMultiplier(10)->Range(10, max_range)->Complexity(benchmark::oN);;

static void measure_vector_filling(benchmark::State &state) {
    const size_t size {static_cast<size_t>(state.range(0))};
    const auto p {manyV(token(integer), false, size)};
    const std::string s {self_concat("1 ", size)};

    for (auto _ : state) {
        auto r {parse_result(p, s)};
        benchmark::DoNotOptimize(r->data());
        assert(r->size() == size);
    }
    state.SetComplexityN(state.range(0));
}

BENCHMARK(measure_vector_filling)->RangeMultiplier(10)->Range(10, max_range)->Complexity(benchmark::oN);;

static auto csv_line(size_t reserve_items = 0) {
    return [reserve_items] (str_pos pos) {
        const auto comma_whitespace {prefixed(oneOf(','), many(oneOf(' ')))};
        return sep_by1(integer, comma_whitespace, reserve_items)(pos);
    };
}

static void csv_vector_of_ints(benchmark::State &state) {
    const size_t size {static_cast<size_t>(state.range(0))};
    assert(size > 0);
    const std::string s {std::string{"1"} + self_concat(", 1", size - 1)};
    const auto p {csv_line(size)};

    for (auto _ : state) {
        const auto r {parse_result(p, s)};
        benchmark::DoNotOptimize(r->data());
        assert(r->size() == size);
    }
    state.SetComplexityN(state.range(0));
}

BENCHMARK(csv_vector_of_ints)->RangeMultiplier(10)->Range(10, max_range)->Complexity(benchmark::oN);;

static void sum_of_ints(benchmark::State &state) {
    const size_t size {static_cast<size_t>(state.range(0))};
    assert(size > 0);
    const std::string s {std::string{"1 "} + self_concat("+ 1", size - 1)};

    for (auto _ : state) {
        const auto r {parse_result(expr, s)};
        benchmark::DoNotOptimize(*r == size);
        assert(*r == size);
    }
    state.SetComplexityN(state.range(0));
}

BENCHMARK(sum_of_ints)->RangeMultiplier(10)->Range(10, max_range);

static void product_of_ints(benchmark::State &state) {
    const size_t size {static_cast<size_t>(state.range(0))};
    assert(size > 0);
    const std::string s {std::string{"1 "} + self_concat("* 1", size - 1)};

    for (auto _ : state) {
        const auto r {parse_result(expr, s)};
        benchmark::DoNotOptimize(r == 1);
        assert(r == 1);
    }
    state.SetComplexityN(state.range(0));
}

BENCHMARK(product_of_ints)->RangeMultiplier(10)->Range(10, max_range)->Complexity(benchmark::oN);;
