#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include "parser.hpp"

static auto add_op()
{
    return [](str_pos p) -> parser<int(*)(int, int)> {
        if (p.at_end()) {
            return {};
        }

        switch (*p) {
        case '+': return {{[](int a, int b) { return a + b; }, p.advance()}};
        case '-': return {{[](int a, int b) { return a - b; }, p.advance()}};
        default: return {};
        }
    };
}

static auto mul_op()
{
    return [](str_pos p) -> parser<int(*)(int, int)> {
        if (p.at_end()) {
            return {};
        }

        switch (*p) {
        case '*': return {{[](int a, int b) { return a * b; }, p.advance()}};
        case '/': return {{[](int a, int b) { return a / b; }, p.advance()}};
        default: return {};
        }
    };
}

static auto op() { return choice(add_op(), mul_op()); }

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

/*
static parse_int_parser factor() {
    return choice(integer(), clasped(oneOf('('), oneOf(')'), expr()));
}

static parse_int_parser term() {
    return chainl1(factor, mul_op);
};

static parse_int_parser expr() {
    return chainl1(term, add_op);
};
*/

int main(int argc, char **argv)
{
    parse_success(parse(many(noneOf(' ')), "abc def"), std::string{"abc"}, "many noneOf works");
    parse_success(parse(manyV(token(number())), "1 2 3"),
                  std::vector<char>{'1', '2', '3'},
                  "manyV with number tokens works");
    parse_success(parse(manyV(token(integer())), "1 2 3"),
                  std::vector<int>{1, 2, 3},
                  "manyV with integer tokens works");

    parse_success((op()(str_pos::from_str("+"))->first)(1, 2), 3, "op parser works");

    static auto parse_sum {chainl1(token(integer()), token(op()))};
    parse_success(parse(parse_sum, "10"), 10, "sum parser works for single numbers");
    parse_success(parse(parse_sum, "10 + 100 + 1000"), 1110, "sum parser works");

    auto spaces {many(oneOf(' '))};
    parse_success(parse(prefixed(spaces, integer()), "    123"), 123, "prefix parser works");

    parse_success(parse(clasped(oneOf('('), oneOf(')'), integer()), "(123)"), 123, "clasped parser works");

    parse_success(parse(choice(integer(), prefixed(spaces, integer())), "   123"), 123, "choice parser works");

    {
    }
}
