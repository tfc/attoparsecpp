#pragma once

#include <iterator>
#include <optional>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>

#include <vector>

using str_it = std::string::const_iterator;

struct str_pos : public std::pair<str_it, str_it> {
    static str_pos from_str(const std::string &s) {
        return {std::pair{std::cbegin(s), std::cend(s)}};
    }

    std::optional<char> peek() const {
        if (!at_end()) { return {*(this->first)}; }
        return {};
    }

    char operator*() const { return *(this->first); }

    str_pos next() const {
        auto r {*this};
        ++(r.first);
        return r;
    }

    size_t size() const { return this->second - this->first; }

    bool at_end() const { return size() == 0; }
};

static std::ostream& operator<<(std::ostream& os, const str_pos &p)
{
    os << "(";
    std::copy(p.first, p.second, std::ostream_iterator<char>{os});
    return os << ")";
}

template <typename T>
using parser = std::optional<std::pair<T, str_pos>>;

template <typename Parser>
using parser_ret = typename std::result_of<Parser(str_pos)>::type;

template <typename Parser>
using parser_payload_type = typename parser_ret<Parser>::value_type::first_type;

template <typename F>
static auto not_at_end(F f)
{
    return [f] (str_pos p) -> parser<parser_payload_type<F>> {
        if (p.at_end()) { return {}; }
        return f(p);
    };
}

static parser<char> anyChar(str_pos pos) {
    return not_at_end([] (str_pos p) -> parser<char> {
        return {{*p, p.next()}};
    })(pos);
}

template <typename F>
static auto sat(F predicate) {
    return not_at_end([predicate] (str_pos p) -> parser<char> {
        if (predicate(*p)) { return {{*p, p.next() }}; }
        return {};
    });
}

static parser<char> number(str_pos pos) {
    return sat([](char c) { return '0' <= c && c <= '9'; })(pos);
}

template <typename ... Cs>
static auto noneOf(Cs ... cs) {
    return sat([cs...] (char c) { return ((c != cs) && ...); });
}

template <typename ... Cs>
static auto oneOf(Cs ... cs) {
    return sat([cs...] (char c) { return ((c == cs) || ...); });
}

template <typename Parser>
static auto many(Parser p, bool minimum_one = false) {
    return [p, minimum_one] (str_pos pos) -> parser<std::string> {
        std::string s;
        while (auto ret {p(pos)}) {
            s.push_back(ret->first);
            pos = ret->second;
        }
        if (minimum_one && s.empty()) { return {}; }
        return {{std::move(s), pos}};
    };
}

template <typename Parser>
static auto many1(Parser p) {
    return many(p, true);
}

template <typename Parser, typename T = parser_payload_type<Parser>>
static auto manyV(Parser p, size_t reserve_items = 0) {
    return [p, reserve_items] (str_pos pos) -> parser<std::vector<T>> {
        std::vector<T> v;
        v.reserve(reserve_items);
        while (auto ret {p(pos)}) {
            auto [c, newpos] = *ret;
            v.push_back(c);
            pos = newpos;
        }
        return {{std::move(v), pos}};
    };
}

static parser<int> integer(str_pos p) {
    int accum = 0;
    str_pos cursor {p};
    while (auto ret = number(cursor)) {
        accum = 10 * accum + ret->first - '0';
        cursor = ret->second;
    }
    if (p.first == cursor.first) {
        return {};
    }
    return {{accum, cursor}};
}

template <typename Parser>
static auto token(Parser parser) {
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

template <typename TParser, typename SepParser,
          typename T = parser_payload_type<TParser>>
static auto sep_by(TParser item_parser, SepParser sep_parser, bool minimum_one = false, size_t reserve_items = 0) {
    return [item_parser, sep_parser, minimum_one, reserve_items]
        (str_pos pos) -> parser<std::vector<T>> {
        std::vector<T> v;
        v.reserve(reserve_items);
        while (auto ret {item_parser(pos)}) {
            auto [c, newpos] = *ret;
            v.emplace_back(std::move(c));
            auto sep_ret {sep_parser(newpos)};
            if (!sep_ret) {
                pos = newpos;
                break;
            }
            pos = sep_ret->second;
        }
        if (minimum_one && v.empty()) { return {}; }
        return {{std::move(v), pos}};
    };
}

template <typename Parser>
static parser<std::tuple<parser_payload_type<Parser>>>
apply_parsers(str_pos pos, const Parser &parser) {
    if (auto ret {parser(pos)}) {
        return {{std::make_tuple(std::move(ret->first)), ret->second}};
    }
    return {};
}

template <typename Parser, typename ... Parsers>
static parser<std::tuple<parser_payload_type<Parser>, parser_payload_type<Parsers> ...>>
apply_parsers(str_pos pos, const Parser &parser, const Parsers& ... rest_parsers) {
    if (auto ret {parser(pos)}) {
        if (auto ret_rest {apply_parsers(ret->second, rest_parsers...)}) {
            return {{std::tuple_cat(std::make_tuple(std::move(ret->first)),
                                    std::move(ret_rest->first)),
                     ret_rest->second}};
        }
    }
    return {};
}

template <typename ... Parsers>
static auto tuple_of(Parsers ... parsers) {
    return [parsers...] (str_pos pos) { return apply_parsers(pos, parsers...); };
}

template <typename Parser1, typename Parser2>
static auto chainl1(Parser1 item_parser, Parser2 op_parser)
{
    using T = parser_payload_type<Parser1>;
    return [item_parser, op_parser] (str_pos p) -> parser<T> {
        auto i {item_parser(p)};
        if (!i) { return {}; }
        auto accum {i->first};

        auto pos {i->second};

        auto op {op_parser(i->second)};

        while (op) {
            auto b {item_parser(op->second)};
            if (!b) { return {{accum, pos}}; }

            accum = op->first(accum, b->first);

            pos = b->second;
            op = op_parser(b->second);
        }
        return {{accum, pos}};
    };
}

template <typename Parser1, typename Parser2>
static auto prefixed(Parser1 prefix_parser, Parser2 parser)
{
    return [prefix_parser, parser] (str_pos pos) -> parser_ret<Parser2> {
        if (auto ret1 {prefix_parser(pos)}) {
            return parser(ret1->second);
        }
        return {};
    };
}

template <typename Parser1, typename Parser2>
static auto postfixed(Parser1 suffix_parser, Parser2 parser)
{
    return [suffix_parser, parser] (str_pos pos) -> parser_ret<Parser2> {
        if (auto ret1 {parser(pos)}) {
            if (auto ret2 {suffix_parser(ret1->second)}) {
                return {{ret1->first, ret2->second}};
            }
        }
        return {};
    };
}

template <typename Parser1, typename Parser2, typename Parser3>
static auto clasped(Parser1 open_parser, Parser2 close_parser, Parser3 parser)
{
    return postfixed(close_parser, prefixed(open_parser, parser));
}

template <typename Parser1, typename Parser2>
static auto choice(Parser1 p1, Parser2 p2)
{
    return [p1, p2] (str_pos pos) {
        if (auto ret1 {p1(pos)}) {
            return ret1;
        }
        return p2(pos);
    };
}

template <typename Parser>
static auto run_parser(Parser &&p, const std::string &s)
{
    return p(str_pos::from_str(s));
}

template <typename Parser>
static std::optional<parser_payload_type<Parser>> parse_result(Parser &&p, const std::string &s)
{
    if (auto ret {run_parser(p, s)}) {
        return {ret->first};
    }
    return {};
}
