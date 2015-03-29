File structure
==============

To allow for semi-automatic integration between the artificial intelligences,
besides implementing [the common interface](player.h),
all AIs must follow have some common file structure
in their repository.

This file documents this structure.

Essentially, each artificial intelligence
must contains two files, named `ai.h` and `ai.conf`
(they must have exactly these name).
These two files inform the game's core
about a PlayerFactory function.
PlayerFactory is defined in [`player.h`](player.h).

`ai.h`
------

Declares the factory function.
For instance, it might be

    namespace my_intelligence {
        Player * generate( int, int );
    }

(We strongly reccomend each AI to have its own namespace
to avoid name conflicts and linking errors.)

There should be only the facory declaration,
not its implementation.

`ai.conf`
---------

Contains the player's name in quotes
(the name that will identify that player in the command-line options)
and a reference to it's factory function,
surrounded by braces:

    {"myIntelligence", my_intelligence::generate},

Note the trailing comma.

That's actually a valid constructor call for
`std::pair< const char *, PlayerFactory >`.

Code generation
---------------

The makefile generates C++ code based on the assumption
that the artificial intelligences follows this format.
--- the `ai.conf` format is especially important.
Since the code is actually an inclusion of all these `ai.h`,
followed by some code, followed by the inclusion of all these `ai.conf`,
a single pair `ai.h`-`ai.conf`
might declare several artificial intelligences,
simply by declaring several factories in `ai.h`
and having several bracketed lines in `ai.conf`.
Remember, however, to include the trailing comma.

(Each of these braced pairs are actually used to construct a `std::vector`
using an `initializer_list`;
that's why the comma is important.)

Compiler path
-------------

To avoid code duplication,
the compiler compiles the sources from the root directory
with the flag `-I./`;
this allows source files
to `#include` to any file in the source tree
by its name relative to the root directory
--- regardless of where that source file is located.

Each directory contains several `.cpp` files
and might have a custom makefile.mk.
See the [makefile conventions](makefile_conventions.md) for more information
about custom makefiles.
