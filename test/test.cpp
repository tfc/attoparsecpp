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
            const auto r {run_parser(p, "1 2 3ABC")};
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
