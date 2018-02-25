#include <cassert>

#include <parser.hpp>
#include <math_expression.hpp>

#define BENCHPRESS_CONFIG_MAIN
#include "benchpress/benchpress.hpp"

using namespace apl;

std::string self_concat(const char *s, size_t times) {
    std::ostringstream ss;
    while (times--) { ss << s; }
    return ss.str();
}

static void measure_word_parsing(size_t size, benchpress::context* ctx) {
    const auto p {many(noneOf(' '))};
    const std::string s {self_concat("a", size)};

    for (size_t i = 0; i < ctx->num_iterations(); ++i) {
        auto r {parse_result(p, s)};
        benchpress::escape(const_cast<char*>(r->data()));
        // can get rid of this ugly const_cast in c++17.
    }
}

BENCHMARK("parse word of    10 chars", [](benchpress::context* ctx) { measure_word_parsing(10, ctx); })
BENCHMARK("parse word of   100 chars", [](benchpress::context* ctx) { measure_word_parsing(100, ctx); })
BENCHMARK("parse word of  1000 chars", [](benchpress::context* ctx) { measure_word_parsing(1000, ctx); })
BENCHMARK("parse word of 10000 chars", [](benchpress::context* ctx) { measure_word_parsing(10000, ctx); })


static void measure_vector_filling(size_t size, benchpress::context* ctx) {
    const auto p {manyV(token(integer), false, size)};
    const std::string s {self_concat("1 ", size - 1)};

    for (size_t i = 0; i < ctx->num_iterations(); ++i) {
        auto r {parse_result(p, s)};
        benchpress::escape(r->data());
    }
}


BENCHMARK("vector<int> of    10 items", [](benchpress::context* ctx) { measure_vector_filling(10, ctx); })
BENCHMARK("vector<int> of   100 items", [](benchpress::context* ctx) { measure_vector_filling(100, ctx); })
BENCHMARK("vector<int> of  1000 items", [](benchpress::context* ctx) { measure_vector_filling(1000, ctx); })
BENCHMARK("vector<int> of 10000 items", [](benchpress::context* ctx) { measure_vector_filling(10000, ctx); })

static auto csv_line(size_t reserve_items = 0) {
    return [reserve_items] (str_pos pos) {
        const auto comma_whitespace {prefixed(oneOf(','), many(oneOf(' ')))};
        return sep_by1(integer, comma_whitespace, reserve_items)(pos);
    };
}

static void csv_vector_of_ints(size_t size, benchpress::context* ctx) {
    assert(size > 0);
    const std::string s {std::string{"1"} + self_concat(", 1", size - 1)};
    const auto p {csv_line(size)};

    for (size_t i = 0; i < ctx->num_iterations(); ++i) {
        const auto r {parse_result(p, s)};
        assert(r->size() == size);
    }
}

BENCHMARK("csv vector of    10 ints", [](benchpress::context* ctx) { csv_vector_of_ints(10, ctx); })
BENCHMARK("csv vector of   100 ints", [](benchpress::context* ctx) { csv_vector_of_ints(100, ctx); })
BENCHMARK("csv vector of  1000 ints", [](benchpress::context* ctx) { csv_vector_of_ints(1000, ctx); })
BENCHMARK("csv vector of 10000 ints", [](benchpress::context* ctx) { csv_vector_of_ints(10000, ctx); })

static void sum_of_ints(size_t size, benchpress::context* ctx) {
    assert(size > 0);
    const std::string s {std::string{"1 "} + self_concat("+ 1", size - 1)};

    for (size_t i = 0; i < ctx->num_iterations(); ++i) {
        const auto r {parse_result(expr, s)};
        assert(*r == size);
    }
}

BENCHMARK("sum of    10 ints", [](benchpress::context* ctx) { sum_of_ints(10, ctx); })
BENCHMARK("sum of   100 ints", [](benchpress::context* ctx) { sum_of_ints(100, ctx); })
BENCHMARK("sum of  1000 ints", [](benchpress::context* ctx) { sum_of_ints(1000, ctx); })
BENCHMARK("sum of 10000 ints", [](benchpress::context* ctx) { sum_of_ints(10000, ctx); })

static void product_of_ints(size_t size, benchpress::context* ctx) {
    assert(size > 0);
    const std::string s {std::string{"1 "} + self_concat("* 1", size - 1)};

    for (size_t i = 0; i < ctx->num_iterations(); ++i) {
        const auto r {parse_result(expr, s)};
        assert(r == 1);
    }
}

BENCHMARK("product of    10 ints", [](benchpress::context* ctx) { product_of_ints(10, ctx); })
BENCHMARK("product of   100 ints", [](benchpress::context* ctx) { product_of_ints(100, ctx); })
BENCHMARK("product of  1000 ints", [](benchpress::context* ctx) { product_of_ints(1000, ctx); })
BENCHMARK("product of 10000 ints", [](benchpress::context* ctx) { product_of_ints(10000, ctx); })

