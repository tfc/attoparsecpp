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

template <typename T>
using parser_payload_type = decltype(std::declval<T>()(str_pos::from_str(""))->first);

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


static auto anyChar() {
    return not_at_end([] (str_pos p) -> parser<char> {
        return {{static_cast<char>(*p), p.advance()}};
    });
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

static auto integer() {
    return [] (str_pos p) -> parser<int> {
        if (auto ret = many(number())(p)) {
            std::istringstream ss {ret->first};
            int i;
            ss >> i;
            return {{i, ret->second}};
        }
        return {};
    };
}

template <typename P>
static auto token(P parser) {
    return [parser] (str_pos p) -> decltype(parser(str_pos::from_str(""))) {
        if (p.at_end()) { return {}; }
        if (auto ret {parser(p)}) {
            auto [c, newpos] = *ret;
            if (auto ret2 {many(oneOf(' ', '\t'))(newpos)}) {
                return {{c, ret2->second}};
            }
        }
        return {};
    };
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

template <typename P>
static auto parse(P &&p, const std::string &s)
{
    return p(str_pos::from_str(s))->first;
}
