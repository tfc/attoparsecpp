#include <cassert>
#include <sstream>

#include <attoparsecpp/math_expression.hpp>
#include <attoparsecpp/parser.hpp>

#include <benchmark/benchmark.h>

static constexpr size_t max_range{10000000};
#define BENCH_COMPLX(x)                                                        \
  BENCHMARK(x)                                                                 \
      ->RangeMultiplier(10)                                                    \
      ->Range(10, max_range)                                                   \
      ->Complexity(benchmark::oN)

using namespace apl;

std::string self_concat(const char *s, size_t times) {
  std::ostringstream ss;
  while (times--) {
    ss << s;
  }
  return ss.str();
}

static void measure_word_parsing(benchmark::State &state) {
  const size_t size{static_cast<size_t>(state.range(0))};
  const auto p{many(noneOf(' '))};
  const std::string s{self_concat("a", size)};

  for (auto _ : state) {
    auto r{parse_result(p, s)->data()};
    benchmark::DoNotOptimize(r);
  }
  state.SetComplexityN(state.range(0));
}

BENCH_COMPLX(measure_word_parsing);

static void measure_vector_filling(benchmark::State &state) {
  const size_t size{static_cast<size_t>(state.range(0))};
  const auto p{manyV(token(integer), false, size)};
  const std::string s{self_concat("1 ", size)};

  for (auto _ : state) {
    auto r{parse_result(p, s)};
    auto res{r->data()};
    benchmark::DoNotOptimize(res);
    assert(r->size() == size);
  }
  state.SetComplexityN(state.range(0));
}

BENCH_COMPLX(measure_vector_filling);

static auto csv_line(size_t reserve_items = 0) {
  return [reserve_items](str_pos pos) {
    const auto comma_whitespace{prefixed(oneOf(','), many(oneOf(' ')))};
    return sep_by1(integer, comma_whitespace, reserve_items)(pos);
  };
}

static void csv_vector_of_ints(benchmark::State &state) {
  const size_t size{static_cast<size_t>(state.range(0))};
  assert(size > 0);
  const std::string s{std::string{"1"} + self_concat(", 1", size - 1)};
  const auto p{csv_line(size)};

  for (auto _ : state) {
    const auto r{parse_result(p, s)};
    auto res{r->data()};
    benchmark::DoNotOptimize(res);
    assert(r->size() == size);
  }
  state.SetComplexityN(state.range(0));
}

BENCH_COMPLX(csv_vector_of_ints);

static void sum_of_ints(benchmark::State &state) {
  const auto size{static_cast<int>(state.range(0))};
  assert(size > 0);
  const std::string s{std::string{"1 "} + self_concat("+ 1", size - 1)};

  for (auto _ : state) {
    const auto r{parse_result(expr, s)};
    bool res{*r == size};
    benchmark::DoNotOptimize(res);
    assert(res);
  }
  state.SetComplexityN(state.range(0));
}

BENCH_COMPLX(sum_of_ints);

static void product_of_ints(benchmark::State &state) {
  const size_t size{static_cast<size_t>(state.range(0))};
  assert(size > 0);
  const std::string s{std::string{"1 "} + self_concat("* 1", size - 1)};

  for (auto _ : state) {
    const auto r{parse_result(expr, s)};
    bool res{r == 1};
    benchmark::DoNotOptimize(res);
    assert(res);
  }
  state.SetComplexityN(state.range(0));
}

BENCH_COMPLX(product_of_ints);

BENCHMARK_MAIN();
