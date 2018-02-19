#pragma once

#include <parser.hpp>

/*
 * Following BNF Grammar:
 *
 * expr = term   `chainl1` add_op
 * term = factor `chainl1` mul_op
 * factor = integer <|> (char '(' *> expr <* char ')')
 *
 * add_op = (char '+' *> pure (+)) <|> (char '-' *> pure (-))
 * mul_op = (char '*' *> pure (*)) <|> (char '/' *> pure (/))
 */

static parser<int(*)(int, int)> add_op(str_pos p)
{
    if (p.at_end()) { return {}; }

    switch (*p) {
    case '+': return {{[](int a, int b) { return a + b; }, p.next()}};
    case '-': return {{[](int a, int b) { return a - b; }, p.next()}};
    default: return {};
    }
}

static parser<int(*)(int, int)> mul_op(str_pos p)
{
    if (p.at_end()) { return {}; }

    switch (*p) {
    case '*': return {{[](int a, int b) { return a * b; }, p.next()}};
    case '/': return {{[](int a, int b) { return a / b; }, p.next()}};
    default: return {};
    }
}

static parser<int> expr(  str_pos p);
static parser<int> term(  str_pos p);
static parser<int> factor(str_pos p);

static parser<int> expr(str_pos p) {
    return chainl1(token(term), token(add_op))(p);
};

static parser<int> term(str_pos p) {
    return chainl1(token(factor), token(mul_op))(p);
};

static parser<int> factor(str_pos p) {
    return choice(integer, clasped(oneOf('('), oneOf(')'), expr))(p);
}

