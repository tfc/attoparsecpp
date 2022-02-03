#include <numeric>
#include <attoparsecpp/parser.hpp>

#include <catch2/catch.hpp>

using namespace apl;
using namespace std::string_literals;

static uint8_t gdb_checksum(const std::string &s) {
    return std::accumulate(std::begin(s), std::end(s), static_cast<uint8_t>(0));
}

static parser<std::string> checksum_parser(str_pos &pos) {
    if (const auto payload_str {clasped(oneOf('$'), oneOf('#'), many(noneOf('#')))(pos)}) {
        const auto ref_chksum {base_integer<uint8_t>(16, 2)(pos)};
        if (ref_chksum == gdb_checksum(*payload_str)) { return payload_str; }
    }
    return {};
}

SCENARIO( "gdb parser", "[gdb_parser]" ) {
    GIVEN( "add_op parser" ) {
        WHEN( "given empty string" ) {
            const auto r {run_parser(checksum_parser, "")};
            REQUIRE( !r.first );
        }
        WHEN( "Given valid empty string" ) {
            const auto r {run_parser(checksum_parser, "$#00")};
            REQUIRE( !!r.first );
            REQUIRE( r.first == ""s );
        }
        WHEN( "Given valid string" ) {
            const auto r {run_parser(checksum_parser, "$vMustReplyEmpty#3a")};
            REQUIRE( !!r.first );
            REQUIRE( r.first == "vMustReplyEmpty"s );
        }
        WHEN( "Given invalid string" ) {
            const auto r {run_parser(checksum_parser, "$vMustReplyEmpty#3b")};
            REQUIRE( !r.first );
        }
    }
}
