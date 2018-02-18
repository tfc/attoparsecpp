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

## Optimizations

As this code partly relies on C++17 already (actually only because of the fold-expression implementation of `oneOf` and `noneOf`), it could/should also use `std::string_view` instead of the handcrafted `str_pos` type.

The output type of the `many` parser combinator could also use `std::string_view`.

