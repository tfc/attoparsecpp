# AttoParsec++

This repository contains a little experimental C++ library for parsing strings.

## Context

I am still totally amazed by the simplicity and power of [Haskell](https://www.haskell.org/) parser libraries like [`parsec`](https://hackage.haskell.org/package/parsec).
There are also libraries like [`attoparsec`](https://hackage.haskell.org/package/attoparsec) and [`megaparsec`](https://hackage.haskell.org/package/megaparsec).
These are all pretty similar in how to use them, but tuned for different use cases.

The general idea for such parsers originates from [this paper](http://www.cs.nott.ac.uk/~pszgmh/pearl.pdf).

There have been [attempts](http://yapb-soc.blogspot.de/2012/11/monadic-parsing-in-c.html) to implement this in C++.
This repository is my attempt to provide a useful and simple library for composing string based parsers that does not scare non-Haskell programmers away immediately.

## Usage

Just include `parser.hpp` from the `include/` folder in your C++ file.
This is a header-only library (but does not inherently have to be).

You can find a lot of examples in the `test/` folder.

There is also the mathematical expression parser example in `include/math_expression.hpp` which implements the very short and elegant `expr` parser from the [original Haskell monadic parsing paper](http://www.cs.nott.ac.uk/~pszgmh/pearl.pdf).
Unit tests for this parser are in `test/math_expression.cpp`.

## Example

The *Mathematical Expression* grammar from the pearl paper that can parse and evaluate the result of expressions like `1 + (3 + 4 * 3) / 2` looks like the following:

``` haskell
expr = term   `chainl1` add_op
term = factor `chainl1` mul_op
factor = integer <|> (char '(' *> expr <* char ')')

add_op = (char '+' *> pure (+)) <|> (char '-' *> pure (-))
mul_op = (char '*' *> pure (*)) <|> (char '/' *> pure (/))
```

With this library, it is possible to get close to it like this:

``` c++
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
```

In order to write a little calculator tool from it, the only code left to be implemented is:

``` c++
std::optional<int> evalutate_math_expression(std::string input) {
    if (const auto result {run_parser(expr, input)}) {
        return {result->first};
    }
    return {};
}

// evaluate_math_expression("1 + 2 * (3 + 2)")
// ==> returns    std::optional<int>{ 11 }
```

`run_parser` returns a tuple that contains the result of successful parsing in the first field and the rest of the not consumed part of the string in the second field.
The whole tuple is wrapped into an `std::optional` which enables the parser lib to indicate failure.

## Optimizations

As this code partly relies on C++17 already (actually only because of the fold-expression implementation of `oneOf` and `noneOf`), it could/should also use `std::string_view` instead of the handcrafted `str_pos` type.

The output type of the `many` parser combinator could also use `std::string_view`.

