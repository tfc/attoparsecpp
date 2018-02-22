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
    return parse_result(expr, input);
}

// evaluate_math_expression("1 + 2 * (3 + 2)")
// ==> returns    std::optional<int>{ 11 }
```

`run_parser` returns a tuple that contains the result of successful parsing in the first field and the rest of the not consumed part of the string in the second field.
The whole tuple is wrapped into an `std::optional` which enables the parser lib to indicate failure.

## Performance

Benchmark output on a Macbook Pro late 2013

- 22 nm "Haswell/Crystalwell" 2.6 GHz Intel "Core i7" processor (4960HQ)
- 256 KB L2, 6 MB L3 Cache
- 16 GB 1600 MHz DDR3L SDRAM

``` bash
parse word of    10 chars              20000000          55 ns/op
parse word of   100 chars               1000000        1084 ns/op
parse word of  1000 chars                200000        5541 ns/op
parse word of 10000 chars                 50000       48410 ns/op
vector<int> of    10 items              5000000         321 ns/op
vector<int> of   100 items              1000000        1238 ns/op
vector<int> of  1000 items               200000       10125 ns/op
vector<int> of 10000 items                20000       97389 ns/op
sum of    10 ints                       5000000         275 ns/op
sum of   100 ints                        500000        2680 ns/op
sum of  1000 ints                         50000       26113 ns/op
sum of 10000 ints                          5000      260483 ns/op
product of    10 ints                  10000000         181 ns/op
product of   100 ints                   1000000        1662 ns/op
product of  1000 ints                    100000       16364 ns/op
product of 10000 ints                     10000      163884 ns/op
```



