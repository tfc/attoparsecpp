#include <attoparsecpp/math_expression.hpp>

#include <catch/catch.hpp>

using namespace apl;

SCENARIO( "op parser", "[math_expression_parser]" ) {
    GIVEN( "add_op parser" ) {
        WHEN( "given empty string" ) {
            const auto r {run_parser(add_op, "")};
            REQUIRE_FALSE( !!r.first );
        }
        WHEN( "given non-op string" ) {
            const auto r {run_parser(add_op, "a")};
            REQUIRE_FALSE( !!r.first );
        }
        WHEN( "given plus string" ) {
            const auto r {run_parser(add_op, "+")};
            REQUIRE( !!r.first );
            REQUIRE( (*r.first)(100, 10) == 110 );
        }
        WHEN( "given minus string" ) {
            const auto r {run_parser(add_op, "-")};
            REQUIRE( !!r.first );
            REQUIRE( (*r.first)(100, 10) == 90 );
        }
    }
    GIVEN( "mul_op parser" ) {
        WHEN( "given empty string" ) {
            const auto r {run_parser(mul_op, "")};
            REQUIRE_FALSE( !!r.first );
        }
        WHEN( "given non-op string" ) {
            const auto r {run_parser(mul_op, "a")};
            REQUIRE_FALSE( !!r.first );
        }
        WHEN( "given multiplication string" ) {
            const auto r {run_parser(mul_op, "*")};
            REQUIRE( !!r.first );
            REQUIRE( (*r.first)(10, 2) == 20 );
        }
        WHEN( "given division string" ) {
            const auto r {run_parser(mul_op, "/")};
            REQUIRE( !!r.first );
            REQUIRE( (*r.first)(10, 2) == 5 );
        }
    }
}

SCENARIO( "math expression parser", "[math_expression_parser]" ) {
    GIVEN( "Sum parsing" ) {
        WHEN( "empty string" ) {
            const auto r {run_parser(expr, "")};
            REQUIRE_FALSE( !!r.first );
        }
        WHEN( "string with single integer" ) {
            const auto r {run_parser(expr, "123")};
            REQUIRE( !!r.first );
            REQUIRE( r.first == 123 );
        }
        WHEN( "string with 2 ints" ) {
            const auto r {run_parser(expr, "123 + 456")};
            REQUIRE( !!r.first );
            REQUIRE( r.first == 579 );
        }
        WHEN( "string with 5 ints" ) {
            const auto r {run_parser(expr, "1 + 2 + 3 - 4 - 5")};
            REQUIRE( !!r.first );
            REQUIRE( r.first == -3 );
        }
    }
    GIVEN( "Product parsing" ) {
        WHEN( "string with 2 ints" ) {
            const auto r {run_parser(expr, "10 * 5")};
            REQUIRE( !!r.first );
            REQUIRE( r.first == 50 );
        }
        WHEN( "string with 5 ints" ) {
            const auto r {run_parser(expr, "1 * 2 * 3 * 4 / 2")};
            REQUIRE( !!r.first );
            REQUIRE( r.first == 12 );
        }
    }
    GIVEN( "Mixed product/sum parsing" ) {
        WHEN( "multiplication then addition" ) {
            const auto r {run_parser(expr, "2 * 3 + 5")};
            REQUIRE( !!r.first );
            REQUIRE( r.first == 11 );
        }
        WHEN( "addition then multiplication" ) {
            const auto r {run_parser(expr, "2 + 3 * 5")};
            REQUIRE( !!r.first );
            REQUIRE( r.first == 17 );
        }
    }
    GIVEN( "Expressions in parentheses" ) {
        WHEN( "number surrounded by a lot of parentheses" ) {
            const auto r {run_parser(expr, "((((((2))))))")};
            REQUIRE( !!r.first );
            REQUIRE( r.first == 2 );
        }
    }
    GIVEN( "Mixed expressions with sub-expressions in parentheses" ) {
        WHEN( "multiplication then addition" ) {
            const auto r {run_parser(expr, "(2 * 3) + 5")};
            REQUIRE( !!r.first );
            REQUIRE( r.first == 11 );
        }
        WHEN( "addition then multiplication" ) {
            const auto r {run_parser(expr, "2 * (3 + 5)")};
            REQUIRE( !!r.first );
            REQUIRE( r.first == 16 );
        }
        WHEN( "expression is a bit more complex" ) {
            const auto r {run_parser(expr, "1 + (2 * (4 + 3) + 12 * 12 - (6 / 3))")};
            REQUIRE( !!r.first );
            REQUIRE( r.first == 157 );
        }
    }
    // TODO: Following tests are wrong. Parser should actually fail.
    GIVEN( "Trailing operations" ) {
        WHEN( "string with single int and trailing +" ) {
            const std::string str {"123 +"};
            const auto r {run_parser(expr, str)};
            REQUIRE( !!r.first );
            REQUIRE( r.first == 123 );
            REQUIRE( r.second->at_end() );
        }
        WHEN( "string with 2 ints and trailing +" ) {
            const std::string str {"123 - 0 +"};
            const auto r {run_parser(expr, str)};
            REQUIRE( !!r.first );
            REQUIRE( r.first == 123 );
            REQUIRE( r.second->at_end() );
        }
        WHEN( "mixed expression with trailing +" ) {
            const std::string str {"1 + (2 * (4 + 3) + 12 * 12 - (6 / 3)) +"};
            const auto r {run_parser(expr, str)};
            REQUIRE( !!r.first );
            REQUIRE( r.first == 157 );
            REQUIRE( r.second->at_end() );
        }
    }
}
