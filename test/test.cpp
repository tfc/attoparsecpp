#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include <parser.hpp>

#include "catch.hpp"

SCENARIO( "Fundamental parsers", "[parser]" ) {
    GIVEN( "anyChar" ) {
        WHEN( "given an empty string" ) {
            const auto r {run_parser(anyChar, "")};
            REQUIRE( !r.has_value() );
        }
        WHEN( "given an alphabetic string \"a\"" ) {
            const auto r {run_parser(anyChar, "a")};
            REQUIRE( r.has_value() );
            REQUIRE( r->first == 'a' );
            REQUIRE( r->second.at_end() == true );
        }
        WHEN( "given an alphabetic string \"ab\"" ) {
            const std::string s {"ab"};
            const auto r {run_parser(anyChar, s)};
            REQUIRE( r.has_value() );
            REQUIRE( r->first == 'a' );
            REQUIRE( r->second.size() == 1);
            REQUIRE( r->second.peek() == 'b' );
        }
    }
    GIVEN( "number" ) {
        WHEN( "given an empty string" ) {
            const auto r {run_parser(number, "")};
            REQUIRE_FALSE( r.has_value() );
        }
        WHEN( "given an alphabetic string" ) {
            const auto r {run_parser(number, "a")};
            REQUIRE_FALSE( r.has_value() );
        }
        WHEN( "given a number string" ) {
            const auto r {run_parser(number, "5")};
            REQUIRE( r.has_value() );
            REQUIRE( r->first == '5' );
            REQUIRE( r->second.at_end() == true );
        }
    }
}

SCENARIO( "many parser combinations", "[parser]" ) {
    GIVEN( "many noneOf 'a'" ) {
        const auto p {many(noneOf('a'))};
        WHEN( "given list of 'a's" ) {
            const auto r {run_parser(p, "aaa")};
            REQUIRE( r.has_value() );
            REQUIRE( r->first == "" );
            REQUIRE( r->second.size() == 3 );
        }
        WHEN( "given bba" ) {
            const std::string s {"bba"};
            const auto r {run_parser(p, s)};
            REQUIRE( r.has_value() );
            REQUIRE( r->first == "bb" );
            REQUIRE( r->second.size() == 1 );
            REQUIRE( r->second.peek() == 'a' );
        }
    }
    GIVEN( "many oneOf 'b'" ) {
        const auto p {many(oneOf('b'))};
        WHEN( "given list of 'a's" ) {
            const auto r {run_parser(p, "aaa")};
            REQUIRE( r.has_value() );
            REQUIRE( r->first == "" );
            REQUIRE( r->second.size() == 3 );
        }
        WHEN( "given bba" ) {
            const std::string s {"bba"};
            const auto r {run_parser(p, s)};
            REQUIRE( r.has_value() );
            REQUIRE( r->first == "bb" );
            REQUIRE( r->second.size() == 1 );
            REQUIRE( r->second.peek() == 'a' );
        }
    }
}

SCENARIO( "manyV parser combinations" ) {
    GIVEN( "manyV string parser" ) {
        const auto p {manyV(anyChar)};
        using vect_t = std::vector<char>;
        WHEN( "given empty string" ) {
            const auto r {run_parser(p, "")};
            REQUIRE( r.has_value() );
            REQUIRE( r->first == vect_t{} );
        }
        WHEN( "given string \"abc\"" ) {
            const auto r {run_parser(p, "abc")};
            REQUIRE( r.has_value() );
            REQUIRE( r->first == vect_t{'a', 'b', 'c'});
            REQUIRE( r->second.at_end() );
        }
    }
    GIVEN( "manyV integer parser" ) {
        const auto p {manyV(integer)};
        using vect_t = std::vector<int>;
        WHEN( "given empty string" ) {
            const auto r {run_parser(p, "")};
            REQUIRE( r.has_value() );
            REQUIRE( r->first == vect_t{} );
        }

        WHEN( "given string \"123\"" ) {
            const auto r {run_parser(p, "123")};
            REQUIRE( r.has_value() );
            REQUIRE( r->first == vect_t{123} );
            REQUIRE( r->second.at_end() );
        }
    }
    GIVEN( "manyV token integer parser" ) {
        const auto p {manyV(token(integer))};
        using vect_t = std::vector<int>;
        WHEN( "given empty string" ) {
            const auto r {run_parser(p, "")};
            REQUIRE( r.has_value() );
            REQUIRE( r->first == vect_t{} );
        }
        WHEN( "given string \"123\"" ) {
            const auto r {run_parser(p, "123")};
            REQUIRE( r.has_value() );
            REQUIRE( r->first == vect_t{123} );
            REQUIRE( r->second.at_end() );
        }
        WHEN( "given string \"1 2 3 \"" ) {
            const auto r {run_parser(p, "1 2 3 ")};
            REQUIRE( r.has_value() );
            REQUIRE( r->first == vect_t{1, 2, 3} );
            REQUIRE( r->second.at_end() );
        }
        WHEN( "given string \"1 2 3\"" ) {
            const auto r {run_parser(p, "1 2 3")};
            REQUIRE( r.has_value() );
            REQUIRE( r->first == vect_t{1, 2, 3} );
            REQUIRE( r->second.at_end() );
        }
        WHEN( "given string \"1 2 3ABC\"" ) {
            const std::string s {"1 2 3ABC"};
            const auto r {run_parser(p, s)};
            REQUIRE( r.has_value() );
            REQUIRE( r->first == vect_t{1, 2, 3} );
            REQUIRE_FALSE( r->second.at_end() );
            REQUIRE( r->second.peek() == 'A' );
        }
    }
}

SCENARIO( "int parser", "[parser]" ) {
    GIVEN( "empty string" ) {
        const auto r {run_parser(integer, "")};
        REQUIRE_FALSE( r.has_value() );
    }
    GIVEN( "string '1'" ) {
        const auto r {run_parser(integer, "1")};
        REQUIRE( r.has_value() );
        REQUIRE( r->first == 1 );
        REQUIRE( r->second.size() == 0 );
    }
    GIVEN( "string '1 '" ) {
        const auto r {run_parser(integer, "1 ")};
        REQUIRE( r.has_value() );
        REQUIRE( r->first == 1 );
        REQUIRE( r->second.size() == 1 );
    }
    GIVEN( "string '123'" ) {
        const auto r {run_parser(integer, "123")};
        REQUIRE( r.has_value() );
        REQUIRE( r->first == 123 );
        REQUIRE( r->second.size() == 0 );
    }
}

SCENARIO( "prefix/postfix parser", "[parser]" ) {
    const auto spaces {many(oneOf(' '))};
    GIVEN( "prefix parser" ) {
        const auto p {prefixed(spaces, anyChar)};
        WHEN( "parsing empty string" ) {
            const auto r {run_parser(p, "")};
            REQUIRE_FALSE( r.has_value() );
        }
        WHEN( "parsing prefix-only string" ) {
            const auto r {run_parser(p, "   ")};
            REQUIRE_FALSE( r.has_value() );
        }
        WHEN( "parsing space-prefixed string \"   ab\"" ) {
            const std::string s {"   ab"};
            const auto r {run_parser(p, s)};
            REQUIRE( r.has_value() );
            REQUIRE( r->first == 'a' );
            REQUIRE( r->second.peek() == 'b' );
        }
    }
    GIVEN( "postfix parser" ) {
        const auto p {postfixed(oneOf(']'), anyChar)};
        WHEN( "parsing empty string" ) {
            const auto r {run_parser(p, "")};
            REQUIRE_FALSE( r.has_value() );
        }
        WHEN( "parsing postfix-only string" ) {
            const auto r {run_parser(p, "]")};
            REQUIRE_FALSE( r.has_value() );
        }
        WHEN( "parsing string \"a]b\"" ) {
            const std::string s {"a]b"};
            const auto r {run_parser(p, s)};
            REQUIRE( r.has_value() );
            REQUIRE( r->first == 'a' );
            REQUIRE( r->second.peek() == 'b' );
        }
    }
    GIVEN( "clasped parser" ) {
        const auto p {clasped(oneOf('('), oneOf(')'), integer)};
        WHEN( "parsing empty string" ) {
            const auto r {run_parser(p, "")};
            REQUIRE_FALSE( r.has_value() );
        }
        WHEN( "parsing clasp-onlue string" ) {
            const auto r {run_parser(p, "()")};
            REQUIRE_FALSE( r.has_value() );
        }
        WHEN( "parsing string \"(123)\"" ) {
            const std::string s {"(123)x"};
            const auto r {run_parser(p, s)};
            REQUIRE( r.has_value() );
            REQUIRE( r->first == 123 );
            REQUIRE( r->second.peek() == 'x' );
        }
    }
}

SCENARIO( "sep_by parsers", "[parser]" ) {
    const auto whitespace {many(oneOf(' ', '\t'))};
    const auto comma_whitespace {prefixed(oneOf(','), whitespace)};
    const auto p {sep_by(integer, comma_whitespace, true)};
    GIVEN( "sep_by int comma" ) {
        WHEN( "given an empty string" ) {
            const auto r {run_parser(p, "")};
            REQUIRE( !r.has_value() );
        }
        WHEN( "given single item" ) {
            const auto r {run_parser(p, "1")};
            REQUIRE( r.has_value() );
            REQUIRE( r->first == std::vector<int>{1} );
        }
        WHEN( "given multiple items without spaces" ) {
            const auto r {run_parser(p, "1,2,3,4")};
            REQUIRE( r.has_value() );
            REQUIRE( r->first == std::vector<int>{1, 2, 3, 4} );
        }
        WHEN( "given multiple items with spaces" ) {
            const auto r {run_parser(p, "1, 2,  3,   4,\t  5")};
            REQUIRE( r.has_value() );
            REQUIRE( r->first == std::vector<int>{1, 2, 3, 4, 5} );
        }
    }
    GIVEN( "sep_by newline sep_by comma integers (CSV)" ) {
        const auto csv_p {sep_by(p, many(oneOf('\n'), true), true)};
        using csv_vect = std::vector<std::vector<int>>;
        WHEN( "given empty string" ) {
            const auto r {run_parser(csv_p, "")};
            REQUIRE( !r.has_value() );
        }
        WHEN( "given 1 csv line without following newline" ) {
            const auto r {run_parser(csv_p, "1,2,3")};
            REQUIRE( r.has_value() );
            REQUIRE( r->first == csv_vect{ {1, 2, 3} } );
        }
        WHEN( "given 1 csv line with following newline" ) {
            const auto r {run_parser(csv_p, "1,2,3\n")};
            REQUIRE( r.has_value() );
            REQUIRE( r->first == csv_vect{ {1, 2, 3} } );
        }
        WHEN( "given 3 csv lines" ) {
            const auto r {run_parser(csv_p, "1,2,3\n4, 5, 6\n7, 8, 9")};
            REQUIRE( r.has_value() );
            REQUIRE( r->first == csv_vect{ {1, 2, 3}, {4, 5, 6}, {7, 8, 9} } );
        }
        WHEN( "given 3 csv lines with lots of newlines inbetween" ) {
            const auto r {run_parser(csv_p, "1,2,3\n\n\n\n4, 5, 6\n\n\n\n7, 8, 9\n\n\n")};
            REQUIRE( r.has_value() );
            REQUIRE( r->first == csv_vect{ {1, 2, 3}, {4, 5, 6}, {7, 8, 9} } );
        }
    }
}


SCENARIO( "tuple_of parsers", "[parser]" ) {
    GIVEN( "tuple of int, int" ) {
        const auto whitespace {many(oneOf(' ', '\t'))};
        const auto comma_whitespace {prefixed(oneOf(','), whitespace)};
        const auto p {tuple_of(integer, prefixed(comma_whitespace, integer))};
        WHEN( "given an empty string" ) {
            const auto r {run_parser(p, "")};
            REQUIRE( !r.has_value() );
        }
        WHEN( "given valid string" ) {
            const auto r {run_parser(p, "123, 456")};
            REQUIRE( r.has_value() );
            REQUIRE( r->first == std::make_tuple(123, 456) );
        }
    }
    GIVEN( "tuple of int, alphaword, vector int" ) {
        const auto whitespace {many(oneOf(' ', '\t'))};
        const auto comma_whitespace {prefixed(oneOf(','), whitespace)};
        const auto alphaword {many(sat([] (char c) { return 'a' <= c && c <= 'z'; }), true)};
        const auto p {tuple_of(integer,
                               prefixed(comma_whitespace, alphaword),
                               prefixed(comma_whitespace, sep_by(integer, comma_whitespace))
                               )};
        WHEN( "given an empty string" ) {
            const auto r {run_parser(p, "")};
            REQUIRE( !r.has_value() );
        }
        WHEN( "given valid string" ) {
            const auto r {run_parser(p, "123, abc, 100, 200, 300")};
            REQUIRE( r.has_value() );
            REQUIRE( r->first == std::make_tuple(123, std::string{"abc"}, std::vector<int>{100, 200, 300}) );
        }
    }
}


