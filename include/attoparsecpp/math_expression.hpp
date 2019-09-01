#pragma once

#include "parser.hpp"

/*
 * Following BNF Grammar from "functional pearls - monadic parsing in haskell"
 * paper: http://www.cs.nott.ac.uk/~pszgmh/pearl.pdf
 *
 * expr = term   `chainl1` add_op
 * term = factor `chainl1` mul_op
 * factor = integer <|> (char '(' *> expr <* char ')')
 *
 * add_op = (char '+' *> pure (+)) <|> (char '-' *> pure (-))
 * mul_op = (char '*' *> pure (*)) <|> (char '/' *> pure (/))
 */

namespace apl {

template <typename T>
static auto pure(T ret) {
  return [ret] (str_pos &) -> parser<T> {
    return {ret};
  };
}


inline int plus_op(    int a, int b) { return a + b; }
inline int minus_op(   int a, int b) { return a - b; }
inline int multiply_op(int a, int b) { return a * b; }
inline int divide_op(  int a, int b) { return a / b; }

static auto add_op(str_pos &p) {
  using namespace operators;
  return (
      ('+'_charP >> pure(plus_op))
    | ('-'_charP >> pure(minus_op))
  )(p);
}

static auto mul_op(str_pos &p)
{
  using namespace operators;
  return (
      ('*'_charP >> pure(multiply_op))
    | ('/'_charP >> pure(divide_op))
  )(p);
}

static parser<int> expr(  str_pos &p);
static parser<int> term(  str_pos &p);
static parser<int> factor(str_pos &p);

static parser<int> expr(str_pos &p) {
    return chainl1(token(term), token(add_op))(p);
};

static parser<int> term(str_pos &p) {
    return chainl1(token(factor), token(mul_op))(p);
};

static parser<int> factor(str_pos &p) {
    return choice(base_integer(10), clasped(oneOf('('), oneOf(')'), expr))(p);
}

} // namespace apl
