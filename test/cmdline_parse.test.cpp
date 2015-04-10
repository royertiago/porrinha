/* Unit test for cmdline/parse.h.
 *
 * Altough cmdline/parse.h and cmdline/args.h must be used together,
 * seems to be better to separate their test cases.
 */
#include "cmdline/args.h"
#include <catch.hpp>

TEST_CASE( "operator>> test for cmdline::args", "[cmdline][operator][trivial" ) {
    const char * argv[] = {
        "program",
        "42",
        "43a",
        "numstr",
        "str",
        "3.1415",
        "8.8a",
        "numstr",
    };
    int i;
    double d;
    std::string str;
    std::stringstream log;

    cmdline::args args( 8, argv );
    args.log(log);

    CHECK( args.size() == 7 );

    args >> i;
    CHECK( i == 42 );
    CHECK( log.str() == "" );
    CHECK( args.size() == 6 );

    args >> i;
    CHECK( i == 43 );
    CHECK( log.str() == 
            "Warning: partially parsed string\n"
            "Unparsed bit: 'a'\n"
         );
    CHECK( args.size() == 5 );
    log.str("");

    args >> i;
    CHECK( i == 0 ); // Failed conversion "returns" zero
    CHECK( log.str() == "Error: could not parse numstr.\n" );
    CHECK( args.size() == 4 );
    log.str("");

    args >> str;
    CHECK( str == "str" );
    CHECK( args.size() == 3 );

    args >> d;
    CHECK( d == Approx(3.1415) );
    CHECK( args.size() == 2 );

    args >> d;
    CHECK( d == Approx(8.8) );
    CHECK( log.str() == 
            "Warning: partially parsed string\n"
            "Unparsed bit: 'a'\n"
         );
    CHECK( args.size() == 1 );
    log.str("");

    args >> d;
    CHECK( log.str() == "Error: could not parse numstr.\n" );
    CHECK( args.size() == 0 );
}
