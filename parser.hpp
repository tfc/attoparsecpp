#include <optional>
#include <string>
#include <utility>
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

std::ostream& operator<<(std::ostream& os, const str_pos &p)
{
    os << "(";
    std::copy(p.first, p.second, std::ostream_iterator<char>{os});
    return os << ")";
}

template <typename T>
using parser = std::optional<std::pair<T, str_pos>>;

template <typename P>
using parser_ret = decltype(std::declval<P>()(str_pos::from_str("")));

template <typename P>
using parser_payload_type = decltype(std::declval<P>()(str_pos::from_str(""))->first);

template <typename F>
static auto not_at_end(F f)
{
    return [f] (str_pos p) -> parser<parser_payload_type<F>> {
        if (p.at_end()) {
            return {};
        }
        return f(p);
    };
}

static parser<char> anyChar(str_pos pos) {
    return not_at_end([] (str_pos p) -> parser<char> {
        return {{static_cast<char>(*p), p.advance()}};
    })(pos);
}

static parser<char> number(str_pos pos) {
    return not_at_end([] (str_pos p) -> parser<char> {
        if (!('0' <= *p && *p <= '9')) {
            return {};
        }
        return {{static_cast<char>(*p), p.advance()}};
    })(pos);
}

template <typename ... Cs>
static auto noneOf(Cs ... cs) {
    return not_at_end([=] (str_pos p) -> parser<char> {
        if (((*p != cs) && ...)) {
            return {{*p, p.advance()}};
        }
        return {};
    });
}

template <typename ... Cs>
static auto oneOf(Cs ... cs) {
    return not_at_end([=] (str_pos p) -> parser<char> {
        if (((*p == cs) || ...)) {
            return {{*p, p.advance()}};
        }
        return {};
    });
}

template <typename P>
static auto many(P p, bool minimum_one = false) {
    return [p, minimum_one] (str_pos pos) -> parser<std::string> {
        std::string ss;
        while (auto ret {p(pos)}) {
            ss.push_back(ret->first);
            pos = ret->second;
        }
        if (minimum_one && ss.empty()) { return {}; }
        return {{std::string{std::cbegin(ss), std::cend(ss)}, pos}};
    };
}

template <typename P>
static auto many1(P p) {
    return many(p, true);
}

template <typename P>
static auto manyV(P p) {
    using T = parser_payload_type<P>;
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

static parser<int> integer(str_pos p) {
    if (auto ret = many1(number)(p)) {
        std::istringstream ss {ret->first};
        int i;
        ss >> i;
        return {{i, ret->second}};
    }
    return {};
}

template <typename P>
static auto token(P parser) {
    return not_at_end([parser] (str_pos p) -> decltype(parser(str_pos::from_str(""))) {
        if (auto ret {parser(p)}) {
            auto [c, newpos] = *ret;
            if (auto ret2 {many(oneOf(' ', '\t'))(newpos)}) {
                return {{c, ret2->second}};
            }
        }
        return {};
    });
}

template <typename P1, typename P2>
static auto chainl1(P1 item_parser, P2 op_parser)
{
    using T = parser_payload_type<P1>;
    return [item_parser, op_parser] (str_pos p) -> parser<T> {
        auto i {item_parser(p)};
        if (!i) { return {}; }
        auto accum {i->first};

        auto pos {i->second};

        auto op {op_parser(i->second)};

        while (op) {
            auto b {item_parser(op->second)};
            if (!b) { return {{accum, op->second}}; }

            accum = op->first(accum, b->first);

            pos = b->second;
            op = op_parser(b->second);
        }
        return {{accum, pos}};
    };
}

template <typename P1, typename P2>
static auto prefixed(P1 prefix_parser, P2 parser)
{
    return [prefix_parser, parser] (str_pos pos) -> parser_ret<P2> {
        if (auto ret1 {prefix_parser(pos)}) {
            return parser(ret1->second);
        }
        return {};
    };
}

template <typename P1, typename P2>
static auto postfixed(P1 suffix_parser, P2 parser)
{
    return [suffix_parser, parser] (str_pos pos) -> parser_ret<P2> {
        if (auto ret1 {parser(pos)}) {
            if (auto ret2 {suffix_parser(ret1->second)}) {
                return {{ret1->first, ret2->second}};
            }
        }
        return {};
    };
}

template <typename P1, typename P2, typename P3>
static auto clasped(P1 open_parser, P2 close_parser, P3 parser)
{
    return postfixed(close_parser, prefixed(open_parser, parser));
}

template <typename P1, typename P2>
static auto choice(P1 p1, P2 p2)
{
    return [p1, p2] (str_pos pos) {
        if (auto ret1 {p1(pos)}) {
            return ret1;
        }
        return p2(pos);
    };
}

template <typename P>
static auto parse(P &&p, const std::string &s)
{
    return p(str_pos::from_str(s))->first;
}
