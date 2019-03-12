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

using namespace operators;
int foofoo(int, int) { return 123; }

/*
template <typename T> class debug_t;


static auto foo(str_pos &p) {

debug_t<decltype(
  (
      ('+'_charP >> pure(foofoo))
    | ('+'_charP >> pure(foofoo))

  )(p)
)> d;

}
*/

static parser<int(*)(int, int)> add_op(str_pos &p) {
  return (
      ('+'_charP >> pure([](int a, int b) -> int { return a + b; }))
    | ('-'_charP >> pure([](int a, int b) -> int { return a - b; }))
  )(p);
}

static parser<int(*)(int, int)> mul_op(str_pos &p)
{
  return (
      ('*'_charP >> pure(foofoo))
    | ('/'_charP >> pure(foofoo))
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

}
