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

TEST_CASE( "Building cmdline::args via push_back", "[cmdline][push_back]" ) {
    cmdline::args args;
    CHECK( args.size() == 0 );
    CHECK( args.program_name() == "" );
    args.program_name( "custom" );
    CHECK( args.program_name() == "custom" );

    args.push_back( "--value" );
    CHECK( args.size() == 1 );
    args.push_back( "78" );
    CHECK( args.size() == 2 );
    args.push_back( "file.cpp" );
    CHECK( args.size() == 3 );

    CHECK( args.peek() == "--value" );
    CHECK( args.next() == "--value" );
    CHECK( args.size() == 2 );
    CHECK( args.peek() == "78" );
    CHECK( args.next() == "78" );
    CHECK( args.size() == 1 );
    CHECK( args.peek() == "file.cpp" );
    CHECK( args.next() == "file.cpp" );
    CHECK( args.size() == 0 );
    CHECK( args.program_name() == "custom" );

    args.push_back( "another.cpp" );
    CHECK( args.size() == 1 );
    CHECK( args.peek() == "another.cpp" );
    CHECK( args.next() == "another.cpp" );
    CHECK( args.size() == 0 );
}

TEST_CASE( "Subarguments", "[cmdline]" ) {
    const char * argv[] = {
        "test",
        "cmd",
        "one",
        "two",
        "another_cmd",
        "three",
        "four",
        "six",
        "extra_arg",
    };
    cmdline::args args( 9, argv );
    CHECK( args.size() == 8 );

    CHECK( args.peek() == "cmd" );
    cmdline::args cmd1 = args.subcmd( 2 );
    CHECK( args.peek() == "another_cmd" );
    CHECK( cmd1.size() == 2 );
    CHECK( args.size() == 5 );
    CHECK( cmd1.program_name() == "cmd" );
    CHECK( cmd1.next() == "one" );
    CHECK( cmd1.next() == "two" );
    CHECK( cmd1.size() == 0 );

    CHECK( args.next() == "another_cmd" );
    cmdline::args cmd2 = args.subarg( 3 );
    CHECK( cmd2.size() == 3 );
    CHECK( cmd2.next() == "three" );
    CHECK( cmd2.next() == "four" );
    CHECK( cmd2.next() == "six" );

    CHECK( args.next() == "extra_arg" );
    CHECK( args.size() == 0 );
}

TEST_CASE( "Subarguments with predicates", "[cmdline][predicate]" ) {
    const char * argv[] = {
        "test",
        "[cmd]",
        "one",
        "two",
        "[cmd2]",
        "three",
        "four",
        "[cmd3]",
    };

    bool (* pred )(const std::string&) = [](const std::string & str) {
        if( str == "" ) return false;
        return *str.begin() == '[' && *str.rbegin() == ']';
    };

    cmdline::args main_args( 8, argv );
    cmdline::args args;
    args = main_args.subarg_until( pred );

    CHECK( args.size() == 0 );
    CHECK( args.program_name() == "" );
    CHECK( main_args.size() == 7 );
    CHECK( main_args.next() == "[cmd]" );

    args = main_args.subarg_until( pred );
    CHECK( args.size() == 2 );
    CHECK( args.program_name() == "" );
    CHECK( args.next() == "one" );
    CHECK( args.next() == "two" );
    CHECK( main_args.size() == 4 );
    CHECK( main_args.peek() == "[cmd2]" );

    args = main_args.subcmd_until( pred );
    CHECK( args.size() == 2 );
    CHECK( args.program_name() == "[cmd2]" );
    CHECK( args.next() == "three" );
    CHECK( args.next() == "four" );
    CHECK( main_args.size() == 1 );
    CHECK( main_args.peek() == "[cmd3]" );

    args.push_back( "A" );
    CHECK( args.size() != 0 );
    // Just to make sure we will erase 'args'.

    SECTION( "subarg_until near end" ) {
        main_args.shift();
        REQUIRE_NOTHROW( args = main_args.subarg_until(pred) );
        CHECK( args.size() == 0 );
    }

    SECTION( "subcmd_until near end" ) {
        REQUIRE_NOTHROW( args = main_args.subcmd_until(pred) );
        CHECK( args.size() == 0 );
        CHECK( args.program_name() == "[cmd3]" );
        CHECK( main_args.size() == 0 );
    }
}
