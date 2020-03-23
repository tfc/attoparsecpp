
#include <attoparsecpp/parser.hpp>

#include <catch2/catch.hpp>

#include <iterator>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace apl;
using namespace std::string_literals;

using section = std::tuple<std::string, std::vector<std::string>>;
using section_list = std::vector<section>;

static parser<section> sectionParser(str_pos &p) {
  return tuple_of(
    clasped(oneOf('['), const_string("]\n"), many(noneOf(']'))),
    manyV(postfixed(oneOf('\n'), many(noneOf('\n', '['))))
  )(p);
}

static parser<section_list> iniMap(str_pos &p) {
  return manyV(sectionParser)(p);
}

SCENARIO( "INI parser", "[ini]" ) {
    GIVEN( "example document" ) {
        WHEN( "reading example document" ) {
            const char* example{
              "[UEFI]\n"
              "foo\n"
              "bar\n"
              "[Something]\n"
              "zoo\n"
              "zop\n"
            };

            const auto r {run_parser(iniMap, example)};
            REQUIRE( r.first );

            const auto ini {*r.first};

            const section_list reference {
              {"UEFI", {"foo", "bar"}},
              {"Something", {"zoo", "zop"}}
            };

            REQUIRE( ini == reference );
        }
    }
}
