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

You can find a lot of examples in the `test/` and `benchmark/` folders.

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
$ make && ./main
clang++ -O2 -std=c++14 -lbenchmark -I../include main.cpp -o main
Run on (8 X 2300 MHz CPU s)
2018-02-27 13:53:18
-----------------------------------------------------------------------
Benchmark                                Time           CPU Iterations
-----------------------------------------------------------------------
measure_word_parsing/10                 60 ns         59 ns   11958044
measure_word_parsing/100              1122 ns       1117 ns     615309
measure_word_parsing/1000             5624 ns       5614 ns     122420
measure_word_parsing/10000           46167 ns      46050 ns      15128
measure_word_parsing/100000         447579 ns     446684 ns       1523
measure_word_parsing/1000000       4793773 ns    4770685 ns        149
measure_word_parsing/10000000     51993664 ns   51924583 ns         12
measure_word_parsing_BigO             5.20 N       5.19 N
measure_word_parsing_RMS                 2 %          2 %
measure_vector_filling/10              424 ns        422 ns    1642071
measure_vector_filling/100            2291 ns       2280 ns     301999
measure_vector_filling/1000          20898 ns      20852 ns      34442
measure_vector_filling/10000        208505 ns     207562 ns       3555
measure_vector_filling/100000      2054322 ns    2040226 ns        337
measure_vector_filling/1000000    21730601 ns   21642455 ns         33
measure_vector_filling/10000000  223637116 ns  223132000 ns          3
measure_vector_filling_BigO          22.36 N      22.31 N
measure_vector_filling_RMS               1 %          1 %
csv_vector_of_ints/10                  414 ns        412 ns    1693202
csv_vector_of_ints/100                2194 ns       2185 ns     320976
csv_vector_of_ints/1000              18223 ns      18196 ns      38116
csv_vector_of_ints/10000            181302 ns     181041 ns       3820
csv_vector_of_ints/100000          1867089 ns    1862028 ns        363
csv_vector_of_ints/1000000        18843539 ns   18808769 ns         39
csv_vector_of_ints/10000000      200541779 ns  200397750 ns          4
csv_vector_of_ints_BigO              20.04 N      20.03 N
csv_vector_of_ints_RMS                   1 %          1 %
sum_of_ints/10                         313 ns        312 ns    2183304
sum_of_ints/100                       3068 ns       3064 ns     220643
sum_of_ints/1000                     30839 ns      30772 ns      22453
sum_of_ints/10000                   310284 ns     310001 ns       2243
sum_of_ints/100000                 3130188 ns    3124631 ns        225
sum_of_ints/1000000               30558996 ns   30527304 ns         23
sum_of_ints/10000000             309699901 ns  309107000 ns          2
product_of_ints/10                     195 ns        195 ns    3498286
product_of_ints/100                   1814 ns       1812 ns     397680
product_of_ints/1000                 17817 ns      17805 ns      38756
product_of_ints/10000               176918 ns     176772 ns       3829
product_of_ints/100000             1762898 ns    1761650 ns        369
product_of_ints/1000000           17843418 ns   17833368 ns         38
product_of_ints/10000000         176399736 ns  176310500 ns          4
product_of_ints_BigO                 17.64 N      17.63 N
product_of_ints_RMS                      0 %          0 %
```

