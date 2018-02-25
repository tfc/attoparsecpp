#pragma once

#include <iterator>
#include <experimental/optional>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace std {
    using namespace std::experimental;
}

namespace apl {

using str_it = std::string::const_iterator;

struct str_pos : public std::pair<str_it, str_it> {
    str_pos(const std::pair<str_it, str_it> &p) : std::pair<str_it, str_it>{p} {}

    static str_pos from_str(const std::string &s) {
        return {std::pair<str_it, str_it>{std::cbegin(s), std::cend(s)}};
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

static parser<char> hexnumber(str_pos pos) {
    return sat([](char c) { return ('0' <= c && c <= '9') || ('a' <= c && c <= 'f'); })(pos);
}

namespace detail {

template <typename L, typename T>
static bool equalTo(const L& lhs, const T &rhs) {
    return lhs == rhs;
}

template <typename L, typename T, typename ... Ts>
static bool equalTo(const L& lhs, const T &rhs, const Ts& ... ts) {
    return lhs == rhs || equalTo(lhs, ts...);
}

template <typename L, typename T>
static bool unequalTo(const L& lhs, const T &rhs) {
    return lhs != rhs;
}
template <typename L, typename T, typename ... Ts>
static bool unequalTo(const L& lhs, const T &rhs, const Ts& ... ts) {
    return lhs != rhs && unequalTo(lhs, ts...);
}

}

template <typename ... Cs>
static auto noneOf(Cs ... cs) {
    return sat([cs...] (char c) { return detail::unequalTo(c, cs...); });
}

template <typename ... Cs>
static auto oneOf(Cs ... cs) {
    return sat([cs...] (char c) { return detail::equalTo(c, cs...); });
}

static auto const_string(std::string s) {
    return [s] (str_pos pos) -> parser<std::string> {
        for (const char c : s) {
            if (auto ret {oneOf(c)(pos)}) {
                pos = ret->second;
            } else {
                return {};
            }
        }
        return {{s, pos}};
    };
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
static auto manyV(Parser p, bool minimum_one = false, size_t reserve_items = 0) {
    return [p, minimum_one, reserve_items] (str_pos pos) -> parser<std::vector<T>> {
        std::vector<T> v;
        v.reserve(reserve_items);
        while (auto ret {p(pos)}) {
            v.push_back(ret->first);
            pos = ret->second;
        }
        if (minimum_one && v.empty()) { return {}; }
        return {{std::move(v), pos}};
    };
}

template <typename Parser>
static auto manyV1(Parser p, size_t reserve_items = 0) {
    return manyV(p, true, reserve_items);
}

template <typename IntType = int>
static auto base_integer(size_t base) {
    return [base] (str_pos p) -> parser<IntType> {
        IntType accum {0};
        str_pos cursor {p};
        const auto num_f = base == 16 ? hexnumber : number;
        while (auto ret = num_f(cursor)) {
            const char c {ret->first};
            accum = base * accum + c;
            if (base == 16 && ('a' <= c && c <= 'z')) { accum = accum + 10 - 'a'; }
            else { accum -= '0'; }
            cursor = ret->second;
        }
        if (p.first == cursor.first) { return {}; }
        return {{accum, cursor}};
    };
}

static parser<int> integer(str_pos p) {
    return not_at_end([] (str_pos pos) -> parser<int> {
        size_t base {10};
        if (*pos == '0') {
            base = 8;
            pos = pos.next();
            if (pos.at_end()) {
                return {{0, pos}};
            } else if (*pos == 'x') {
                base = 16;
                pos = pos.next();
            } else if (*pos < '0' || '9' < *pos) {
                return {{0, pos}};
            }
        }
        return base_integer(base)(pos);
    })(p);
}

template <typename Parser>
static auto token(Parser parser) {
    return not_at_end([parser] (str_pos p) -> decltype(parser(str_pos::from_str(""))) {
        if (auto ret {parser(p)}) {
            if (auto ret2 {many(oneOf(' ', '\t'))(ret->second)}) {
                return {{ret->first, ret2->second}};
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
            v.emplace_back(std::move(ret->first));
            auto sep_ret {sep_parser(ret->second)};
            if (!sep_ret) {
                pos = ret->second;
                break;
            }
            pos = sep_ret->second;
        }
        if (minimum_one && v.empty()) { return {}; }
        return {{std::move(v), pos}};
    };
}

template <typename TParser, typename SepParser,
          typename T = parser_payload_type<TParser>>
static auto sep_by1(TParser item_parser, SepParser sep_parser, size_t reserve_items = 0) {
    return sep_by(item_parser, sep_parser, true, reserve_items);
}

namespace detail {

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

}

template <typename ... Parsers>
static auto tuple_of(Parsers ... parsers) {
    return [parsers...] (str_pos pos) { return detail::apply_parsers(pos, parsers...); };
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

namespace detail {
template <typename Parser>
static parser_ret<Parser> apply_parser_choice(str_pos pos, Parser p) {
    return p(pos);
}

template <typename Parser, typename ...Parsers>
static parser_ret<Parser> apply_parser_choice(str_pos pos, Parser p, Parsers ... ps) {
    if (auto ret {p(pos)}) { return ret; }
    return apply_parser_choice(pos, ps...);
}
}

template <typename ...Parsers>
static auto choice(Parsers ... ps)
{
    return [ps...] (str_pos pos) {
        return detail::apply_parser_choice(pos, ps...);
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

}
