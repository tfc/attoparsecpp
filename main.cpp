#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include "parser.hpp"

static auto op()
{
    return [](str_pos p) -> parser<int(*)(int, int)> {
        if (p.at_end()) {
            return {};
        }

        switch (*p) {
        case '+': return {{[](int a, int b) { return a + b; }, p.advance()}};
        case '-': return {{[](int a, int b) { return a - b; }, p.advance()}};
        case '*': return {{[](int a, int b) { return a * b; }, p.advance()}};
        case '/': return {{[](int a, int b) { return a / b; }, p.advance()}};
        default: return {};
        }
    };
}

template <typename T>
static std::ostream& operator<<(std::ostream &os, const std::vector<T> &v)
{
    os << '[';
    std::copy(std::begin(v), std::end(v), std::ostream_iterator<T>{os, ", "});
    return os << ']';
}


template <typename T>
static void test(const T &a, const T &b, const std::string &s)
{
    std::cout << "================\nTest: " << s <<'\n';
    if (a != b) {
        std::cout << "Fail!\n a = " << a << "\n b = " << b << '\n';;
    }
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cout << "plz provide a string as parameter\n";
        return 1;
    }

    const std::string input {argv[1]};

    test(parse(many(noneOf(' ')), "abc def"), std::string{"abc"}, "many noneOf works");
    test(parse(manyV(token(number())), "1 2 3"),
         std::vector<char>{'1', '2', '3'},
         "manyV with number tokens works");
    test(parse(manyV(token(integer())), "1 2 3"),
         std::vector<int>{1, 2, 3},
         "manyV with integer tokens works");
    test((op()(str_pos::from_str("+"))->first)(1, 2), 3, "op parser works");

    static auto parse_sum {chainl1(token(integer()), token(op()))};
    test(parse(parse_sum, "10 + 100 + 1000"), 1110, "sum parser works");
}
