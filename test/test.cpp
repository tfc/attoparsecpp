#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include <attoparsecpp/parser.hpp>

#include <catch2/catch_test_macros.hpp>

using namespace apl;
using namespace std::string_literals;

SCENARIO("Fundamental parsers", "[parser]") {
  GIVEN("anyChar") {
    WHEN("given an empty string") {
      const auto r{run_parser(anyChar, "")};
      REQUIRE(!r.first);
    }
    WHEN("given an alphabetic string \"a\"") {
      const auto r{run_parser(anyChar, "a")};
      REQUIRE(!!r.first);
      REQUIRE(r.first == 'a');
      REQUIRE(r.second.at_end() == true);
    }
    WHEN("given an alphabetic string \"ab\"") {
      const std::string s{"ab"};
      const auto r{run_parser(anyChar, s)};
      REQUIRE(!!r.first);
      REQUIRE(r.first == 'a');
      REQUIRE(r.second.size() == 1);
      REQUIRE(r.second.peek() == 'b');
    }
  }
  GIVEN("number") {
    WHEN("given an empty string") {
      const auto r{run_parser(number, "")};
      REQUIRE_FALSE(!!r.first);
    }
    WHEN("given an alphabetic string") {
      const auto r{run_parser(number, "a")};
      REQUIRE_FALSE(!!r.first);
    }
    WHEN("given a number string") {
      const auto r{run_parser(number, "5")};
      REQUIRE(!!r.first);
      REQUIRE(r.first == '5');
      REQUIRE(r.second.at_end() == true);
    }
  }
  GIVEN("const_string") {
    const auto p{const_string("abcdef")};
    WHEN("given an empty string") {
      const auto r{run_parser(p, "")};
      REQUIRE(!r.first);
    }
    WHEN("given bad string") {
      const auto r{run_parser(p, "xzy")};
      REQUIRE(!r.first);
    }
    WHEN("given partly correct string") {
      const auto r{run_parser(p, "abc")};
      REQUIRE(!r.first);
    }
    WHEN("given correct string") {
      const auto r{run_parser(p, "abcdef")};
      REQUIRE(r.first);
      REQUIRE(r.first == "abcdef"s);
    }
    WHEN("given correct string plus suffix, rest string not consumed") {
      const std::string s{"abcdefg"};
      const auto r{run_parser(p, s)};
      REQUIRE(r.first);
      REQUIRE(r.first == "abcdef"s);
      REQUIRE(r.second.peek() == 'g');
    }
  }
}

SCENARIO("many parser combinations", "[parser]") {
  GIVEN("many noneOf 'a'") {
    const auto p{many(noneOf('a'))};
    WHEN("given list of 'a's") {
      const auto r{run_parser(p, "aaa")};
      REQUIRE(!!r.first);
      REQUIRE(r.first == ""s);
      REQUIRE(r.second.size() == 3);
    }
    WHEN("given bba") {
      const std::string s{"bba"};
      const auto r{run_parser(p, s)};
      REQUIRE(!!r.first);
      REQUIRE(r.first == "bb"s);
      REQUIRE(r.second.size() == 1);
      REQUIRE(r.second.peek() == 'a');
    }
  }
  GIVEN("many oneOf 'b'") {
    const auto p{many(oneOf('b'))};
    WHEN("given list of 'a's") {
      const auto r{run_parser(p, "aaa")};
      REQUIRE(!!r.first);
      REQUIRE(r.first == ""s);
      REQUIRE(r.second.size() == 3);
    }
    WHEN("given bba") {
      const std::string s{"bba"};
      const auto r{run_parser(p, s)};
      REQUIRE(!!r.first);
      REQUIRE(r.first == "bb"s);
      REQUIRE(r.second.size() == 1);
      REQUIRE(r.second.peek() == 'a');
    }
  }
}

SCENARIO("manyV parser combinations") {
  GIVEN("manyV string parser") {
    const auto p{manyV(anyChar)};
    using vect_t = std::vector<char>;
    WHEN("given empty string") {
      const auto r{run_parser(p, "")};
      REQUIRE(!!r.first);
      REQUIRE(r.first == vect_t{});
    }
    WHEN("given string \"abc\"") {
      const auto r{run_parser(p, "abc")};
      REQUIRE(!!r.first);
      REQUIRE(r.first == vect_t{'a', 'b', 'c'});
      REQUIRE(r.second.at_end());
    }
  }
  GIVEN("manyV integer parser") {
    const auto p{manyV(integer)};
    using vect_t = std::vector<int>;
    WHEN("given empty string") {
      const auto r{run_parser(p, "")};
      REQUIRE(!!r.first);
      REQUIRE(r.first == vect_t{});
    }

    WHEN("given string \"123\"") {
      const auto r{run_parser(p, "123")};
      REQUIRE(!!r.first);
      REQUIRE(r.first == vect_t{123});
      REQUIRE(r.second.at_end());
    }
  }
  GIVEN("manyV token integer parser") {
    const auto p{manyV(token(integer))};
    using vect_t = std::vector<int>;
    WHEN("given empty string") {
      const auto r{run_parser(p, "")};
      REQUIRE(!!r.first);
      REQUIRE(r.first == vect_t{});
    }
    WHEN("given string \"123\"") {
      const auto r{run_parser(p, "123")};
      REQUIRE(!!r.first);
      REQUIRE(r.first == vect_t{123});
      REQUIRE(r.second.at_end());
    }
    WHEN("given string \"1 2 3 \"") {
      const auto r{run_parser(p, "1 2 3 ")};
      REQUIRE(!!r.first);
      REQUIRE(r.first == vect_t{1, 2, 3});
      REQUIRE(r.second.at_end());
    }
    WHEN("given string \"1 2 3\"") {
      const auto r{run_parser(p, "1 2 3")};
      REQUIRE(!!r.first);
      REQUIRE(r.first == vect_t{1, 2, 3});
      REQUIRE(r.second.at_end());
    }
    WHEN("given string \"1 2 3ABC\"") {
      const std::string s{"1 2 3ABC"};
      const auto r{run_parser(p, s)};
      REQUIRE(!!r.first);
      REQUIRE(r.first == vect_t{1, 2, 3});
      REQUIRE_FALSE(r.second.at_end());
      REQUIRE(r.second.peek() == 'A');
    }
  }
}

SCENARIO("int parser", "[parser]") {
  GIVEN("empty string") {
    const auto r{run_parser(base_integer(10), "")};
    REQUIRE_FALSE(!!r.first);
  }
  GIVEN("string '1'") {
    const auto r{run_parser(base_integer(10), "1")};
    REQUIRE(!!r.first);
    REQUIRE(r.first == 1);
    REQUIRE(r.second.size() == 0);
  }
  GIVEN("string '1 '") {
    const auto r{run_parser(base_integer(10), "1 ")};
    REQUIRE(!!r.first);
    REQUIRE(r.first == 1);
    REQUIRE(r.second.size() == 1);
  }
  GIVEN("string '123'") {
    const auto r{run_parser(base_integer(10), "123")};
    REQUIRE(!!r.first);
    REQUIRE(r.first == 123);
    REQUIRE(r.second.size() == 0);
  }
  GIVEN("string '123' but only parsing first 2 digits") {
    const auto r{run_parser(base_integer(10, 2), "123")};
    REQUIRE(!!r.first);
    REQUIRE(r.first == 12);
    REQUIRE(r.second.size() == 1);
  }
}

SCENARIO("auto int parser", "[parser]") {
  GIVEN("empty string") {
    const auto r{run_parser(integer, "")};
    REQUIRE(!r.first);
  }
  GIVEN("string '0'") {
    const auto r{run_parser(integer, "0")};
    REQUIRE(r.first == 0);
  }
  GIVEN("string '0 '") {
    const std::string s{"0 "};
    const auto r{run_parser(integer, s)};
    REQUIRE(r.first == 0);
    REQUIRE(r.second.size() == 1);
    REQUIRE(r.second.peek() == ' ');
  }
  GIVEN("string '1'") {
    const auto r{run_parser(integer, "1")};
    REQUIRE(r.first == 1);
  }
  GIVEN("string '01'") {
    const auto r{run_parser(integer, "01")};
    REQUIRE(r.first == 1);
  }
  GIVEN("string '0x1'") {
    const auto r{run_parser(integer, "0x1")};
    REQUIRE(r.first == 1);
  }
  GIVEN("string '12345'") {
    const auto r{run_parser(integer, "12345")};
    REQUIRE(r.first == 12345);
  }
  GIVEN("string '012345'") {
    const auto r{run_parser(integer, "012345")};
    REQUIRE(r.first == 012345);
  }
  GIVEN("string '0x123abc'") {
    const auto r{run_parser(integer, "0x123abc")};
    REQUIRE(r.first == 0x123abc);
  }
}

SCENARIO("prefix/postfix parser", "[parser]") {
  const auto spaces{many(oneOf(' '))};
  GIVEN("prefix parser") {
    const auto p{prefixed(spaces, anyChar)};
    WHEN("parsing empty string") {
      const auto r{run_parser(p, "")};
      REQUIRE_FALSE(!!r.first);
    }
    WHEN("parsing prefix-only string") {
      const auto r{run_parser(p, "   ")};
      REQUIRE_FALSE(!!r.first);
    }
    WHEN("parsing space-prefixed string \"   ab\"") {
      const std::string s{"   ab"};
      const auto r{run_parser(p, s)};
      REQUIRE(!!r.first);
      REQUIRE(r.first == 'a');
      REQUIRE(r.second.peek() == 'b');
    }
  }
  GIVEN("postfix parser") {
    const auto p{postfixed(oneOf(']'), anyChar)};
    WHEN("parsing empty string") {
      const auto r{run_parser(p, "")};
      REQUIRE_FALSE(!!r.first);
    }
    WHEN("parsing postfix-only string") {
      const auto r{run_parser(p, "]")};
      REQUIRE_FALSE(!!r.first);
    }
    WHEN("parsing string \"a]b\"") {
      const std::string s{"a]b"};
      const auto r{run_parser(p, s)};
      REQUIRE(!!r.first);
      REQUIRE(r.first == 'a');
      REQUIRE(r.second.peek() == 'b');
    }
  }
  GIVEN("clasped parser") {
    const auto p{clasped(oneOf('('), oneOf(')'), integer)};
    WHEN("parsing empty string") {
      const auto r{run_parser(p, "")};
      REQUIRE_FALSE(!!r.first);
    }
    WHEN("parsing clasp-onlue string") {
      const auto r{run_parser(p, "()")};
      REQUIRE_FALSE(!!r.first);
    }
    WHEN("parsing string \"(123)\"") {
      const std::string s{"(123)x"};
      const auto r{run_parser(p, s)};
      REQUIRE(!!r.first);
      REQUIRE(r.first == 123);
      REQUIRE(r.second.peek() == 'x');
    }
  }
}

SCENARIO("sep_by parsers", "[parser]") {
  const auto whitespace{many(oneOf(' ', '\t'))};
  const auto comma_whitespace{prefixed(oneOf(','), whitespace)};
  const auto p{sep_by(integer, comma_whitespace, true)};
  GIVEN("sep_by int comma") {
    WHEN("given an empty string") {
      const auto r{run_parser(p, "")};
      REQUIRE(!r.first);
    }
    WHEN("given single item") {
      const auto r{run_parser(p, "1")};
      REQUIRE(!!r.first);
      REQUIRE(r.first == std::vector<int>{1});
    }
    WHEN("given multiple items without spaces") {
      const auto r{run_parser(p, "1,2,3,4")};
      REQUIRE(!!r.first);
      REQUIRE(r.first == std::vector<int>{1, 2, 3, 4});
    }
    WHEN("given multiple items with spaces") {
      const auto r{run_parser(p, "1, 2,  3,   4,\t  5")};
      REQUIRE(!!r.first);
      REQUIRE(r.first == std::vector<int>{1, 2, 3, 4, 5});
    }
  }
  GIVEN("sep_by newline sep_by comma integers (CSV)") {
    const auto csv_p{sep_by(p, many(oneOf('\n'), true), true)};
    using csv_vect = std::vector<std::vector<int>>;
    WHEN("given empty string") {
      const auto r{run_parser(csv_p, "")};
      REQUIRE(!r.first);
    }
    WHEN("given 1 csv line without following newline") {
      const auto r{run_parser(csv_p, "1,2,3")};
      REQUIRE(!!r.first);
      REQUIRE(r.first == csv_vect{{1, 2, 3}});
    }
    WHEN("given 1 csv line with following newline") {
      const auto r{run_parser(csv_p, "1,2,3\n")};
      REQUIRE(!!r.first);
      REQUIRE(r.first == csv_vect{{1, 2, 3}});
    }
    WHEN("given 3 csv lines") {
      const auto r{run_parser(csv_p, "1,2,3\n4, 5, 6\n7, 8, 9")};
      REQUIRE(!!r.first);
      REQUIRE(r.first == csv_vect{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}});
    }
    WHEN("given 3 csv lines with lots of newlines inbetween") {
      const auto r{
          run_parser(csv_p, "1,2,3\n\n\n\n4, 5, 6\n\n\n\n7, 8, 9\n\n\n")};
      REQUIRE(!!r.first);
      REQUIRE(r.first == csv_vect{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}});
    }
  }
}

SCENARIO("tuple_of parsers", "[parser]") {
  GIVEN("tuple of int, int") {
    const auto whitespace{many(oneOf(' ', '\t'))};
    const auto comma_whitespace{prefixed(oneOf(','), whitespace)};
    const auto p{tuple_of(integer, prefixed(comma_whitespace, integer))};
    WHEN("given an empty string") {
      const auto r{run_parser(p, "")};
      REQUIRE(!r.first);
    }
    WHEN("given valid string") {
      const auto r{run_parser(p, "123, 456")};
      REQUIRE(!!r.first);
      REQUIRE(r.first == std::make_tuple(123, 456));
    }
  }
  GIVEN("tuple of int, alphaword, vector int") {
    const auto whitespace{many(oneOf(' ', '\t'))};
    const auto comma_whitespace{prefixed(oneOf(','), whitespace)};
    const auto alphaword{
        many(sat([](char c) { return 'a' <= c && c <= 'z'; }), true)};
    const auto p{tuple_of(
        integer, prefixed(comma_whitespace, alphaword),
        prefixed(comma_whitespace, sep_by(integer, comma_whitespace)))};
    WHEN("given an empty string") {
      const auto r{run_parser(p, "")};
      REQUIRE(!r.first);
    }
    WHEN("given valid string") {
      const auto r{run_parser(p, "123, abc, 100, 200, 300")};
      REQUIRE(!!r.first);
      REQUIRE(r.first == std::make_tuple(123, std::string{"abc"},
                                         std::vector<int>{100, 200, 300}));
    }
  }
}

SCENARIO("choice parsers", "[parser]") {
  GIVEN("choice of A B number") {
    const auto p{choice(oneOf('A'), oneOf('B'), number)};
    WHEN("given an empty string") {
      const auto r{run_parser(p, "")};
      REQUIRE(!r.first);
    }
    WHEN("given A") {
      const std::string s{"A "};
      const auto r{run_parser(p, s)};
      REQUIRE(!!r.first);
      REQUIRE(r.first == 'A');
      REQUIRE(r.second.peek() == ' ');
    }
    WHEN("given B") {
      const auto r{run_parser(p, "B")};
      REQUIRE(!!r.first);
      REQUIRE(r.first == 'B');
    }
    WHEN("given C") {
      const auto r{run_parser(p, "C")};
      REQUIRE(!r.first);
    }
    WHEN("given 5") {
      const std::string s{"5 "};
      const auto r{run_parser(p, s)};
      REQUIRE(!!r.first);
      REQUIRE(r.first == '5');
      REQUIRE(r.second.peek() == ' ');
    }
  }
}
