/* Unit test for the class cmdline::args.
 */
#include "cmdline/args.h"
#include <catch.hpp>

TEST_CASE( "Basic cmdline::args test", "[cmdline][trivial]" ) {
    const char * argv[] = {
        "test",
        "--home",
        "--val",
        "45",
        "--",
        "file.cpp",
        "another.cpp",
    };
    cmdline::args args( 7, argv );

    CHECK( args.size() == 6 );
    CHECK( args.program_name() == "test" );
    args.program_name( "main" );
    CHECK( args.program_name() == "main" );

    CHECK( args.peek() == "--home" );
    CHECK( args.next() == "--home" );
    CHECK( args.size() == 5 );

    CHECK( args.peek() == "--val" );
    CHECK( args.next() == "--val" );
    CHECK( args.size() == 4 );

    CHECK( args.peek() == "45" );
    CHECK( args.next() == "45" );
    CHECK( args.size() == 3 );

    CHECK( args.peek() == "--" );
    args.shift();
    CHECK( args.size() == 2 );

    CHECK( args.peek() == "file.cpp" );
    args.shift();
    CHECK( args.size() == 1 );

    CHECK( args.peek() == "another.cpp" );
    args.shift();
    CHECK( args.size() == 0 );
}
