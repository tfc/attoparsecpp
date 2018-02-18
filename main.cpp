#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include "parser.hpp"

static parser<int(*)(int, int)> add_op(str_pos p)
{
    if (p.at_end()) {
        return {};
    }

    switch (*p) {
    case '+': return {{[](int a, int b) { return a + b; }, p.advance()}};
    case '-': return {{[](int a, int b) { return a - b; }, p.advance()}};
    default: return {};
    }
}

static parser<int(*)(int, int)> mul_op(str_pos p)
{
    if (p.at_end()) {
        return {};
    }

    switch (*p) {
    case '*': return {{[](int a, int b) { return a * b; }, p.advance()}};
    case '/': return {{[](int a, int b) { return a / b; }, p.advance()}};
    default: return {};
    }
}

static auto op() { return choice(add_op, mul_op); }

template <typename T>
static std::ostream& operator<<(std::ostream &os, const std::vector<T> &v)
{
    os << '[';
    std::copy(std::begin(v), std::end(v), std::ostream_iterator<T>{os, ", "});
    return os << ']';
}


template <typename T>
static void parse_success(const T &a, const T &b, const std::string &s)
{
    std::cout << "================\nTest: " << s <<'\n';
    if (a != b) {
        std::cout << "Fail!\n a = " << a << "\n b = " << b << '\n';;
    }
}

static parser<int> expr(str_pos p);
static parser<int> term(str_pos p);
static parser<int> factor(str_pos p);

static parser<int> expr(str_pos p) {
    return [] (str_pos pos) {
        return chainl1(token(term), token(add_op))(pos);
    }(p);
};

static parser<int> term(str_pos p) {
    return [] (str_pos pos) {
        return chainl1(token(factor), token(mul_op))(pos);
    }(p);
};
static parser<int> factor(str_pos p) {
    return [] (str_pos pos) {
        return choice(integer, clasped(oneOf('('), oneOf(')'), expr))(pos);
    }(p);
}

int main(int argc, char **argv)
{
    parse_success(parse(many(noneOf(' ')), "abc def"), std::string{"abc"}, "many noneOf works");
    parse_success(parse(manyV(token(number)), "1 2 3"),
                  std::vector<char>{'1', '2', '3'},
                  "manyV with number tokens works");
    parse_success(parse(manyV(token(integer)), "1 2 3"),
                  std::vector<int>{1, 2, 3},
                  "manyV with integer tokens works");

    parse_success((op()(str_pos::from_str("+"))->first)(1, 2), 3, "op parser works");

    static auto parse_sum {chainl1(token(integer), token(op()))};
    parse_success(parse(parse_sum, "10"), 10, "sum parser works for single numbers");
    parse_success(parse(parse_sum, "10 + 100 + 1000"), 1110, "sum parser works");

    auto spaces {many(oneOf(' '))};
    parse_success(parse(prefixed(spaces, integer), "    123"), 123, "prefix parser works");

    parse_success(parse(clasped(oneOf('('), oneOf(')'), integer), "(123)"), 123, "clasped parser works");

    parse_success(parse(choice(integer, prefixed(spaces, integer)), "   123"), 123, "choice parser works");

    parse_success(parse(expr, "1 + 2"), 3, "expr parses 1 + 2");
    parse_success(parse(expr, "1 + 2 + 3"), 6, "expr parses normal sums");
    parse_success(parse(term, "1 * 2 * 5"), 10, "term parses normal products");
    parse_success(parse(expr, "1 * 2 * 5"), 10, "expr parses normal products");
    parse_success(parse(choice(integer, clasped(oneOf('('), oneOf(')'), expr)), "(1 + 2 + 3)"),
                  6, "expr parses sums in parentheses");
    parse_success(parse(expr, "1 + 2 * 3 + 3"), 10, "expr parses sums with products");
    parse_success(parse(expr, "1000 + (200 + 30) + 4"), 1234, "expr parses sums with products");
    parse_success(parse(expr, "1000 + (200 * 3) + 4"), 1604, "expr parses sums with products");
    parse_success(parse(expr, "(((((5)))))"), 5, "expr extracts number from many nested parentheses");
    parse_success(parse(expr, "1 + (2 * (4 + 3) + 12 * 12 - (6 / 3))"), 157,
                  "expr parses a more complicated expression correctly");
}
