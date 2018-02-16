#include <iostream>
#include <optional>
#include <string>
#include <vector>

using str_it = std::string::const_iterator;

struct str_pos : public std::pair<str_it, str_it> {
    static str_pos from_str(const std::string &s) {
        return {std::pair{std::cbegin(s), std::cend(s)}};
    }

    char operator*() const { return *(this->first); }

    str_pos& advance(size_t i = 1) {
        ++this->first;
        return *this;
    }

    bool at_end() const { return this->first == this->second; }
};

template <typename T>
using parser = std::optional<std::pair<T, str_pos>>;

static auto anyChar() {
    return [] (str_pos p) -> parser<char> {
        if (p.at_end()) {
            return {};
        }
        return {{static_cast<char>(*p), p.advance()}};
    };
}

static auto number() {
    return [] (str_pos p) -> parser<char> {
        if (p.at_end() || !('0' <= *p && *p <= '9')) {
            return {};
        }
        return {{static_cast<char>(*p), p.advance()}};
    };
}

template <typename ... Cs>
static auto noneOf(Cs ... cs) {
    return [=] (str_pos p) -> parser<char> {
        if (p.at_end() || ((*p == cs) || ...)) {
            return {};
        }
        return {{*p, p.advance()}};
    };
}

template <typename ... Cs>
static auto oneOf(Cs ... cs) {
    return [=] (str_pos p) -> parser<char> {
        if (p.at_end() || !((*p == cs) || ...)) {
            return {};
        }
        return {{*p, p.advance()}};
    };
}

template <typename P>
static auto many(P p) {
    return [p] (str_pos pos) -> parser<std::string> {
        std::string ss;
        while (auto ret {p(pos)}) {
            ss.push_back(ret->first);
            pos = ret->second;
        }
        return {{std::string{std::cbegin(ss), std::cend(ss)}, pos}};
    };
}

template <typename P>
static auto manyV(P p) {
    using T = decltype(p(str_pos::from_str(""))->first);
    return [p] (str_pos pos) -> parser<std::vector<T>> {
        std::vector<T> v;
        while (auto ret {p(pos)}) {
            auto [c, newpos] = *ret;
            v.push_back(c);
            pos = newpos;
        }
        return {{v, pos}};
    };
}

template <typename P>
static auto token(P parser) {
    return [parser] (str_pos p) -> decltype(parser(str_pos::from_str(""))) {
        if (auto ret {parser(p)}) {
            auto [c, newpos] = *ret;
            if (auto ret2 {many(oneOf(' ', '\t'))(newpos)}) {
                return {{c, ret2->second}};
            }
        }
        return {};
    };
}

template <typename P>
static auto parse(P &&p, const std::string &s)
{
    return p(str_pos::from_str(s))->first;
}

template <typename T>
static std::ostream& operator<<(std::ostream &os, const std::vector<T> &v)
{
    os << '[';
    for (const auto &i : v) { os << i << ", "; }
    return os << ']';
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cout << "plz provide a string as parameter\n";
        return 1;
    }

    const std::string input {argv[1]};

    std::cout << input << '\n';
    std::cout << parse(manyV(token(number())), input) << '\n';
    std::cout << parse(many(noneOf(' ')), input) << '\n';
}
