Makefile conventions
====================

In the light of
[Recursive Make Considered Harmful](http://aegis.sourceforge.net/auug97.pdf),
the makefiles of this project are not recursive.
Instead, the top make file (the file ./makefile)
includes other makefiles.
Some conventions ease this integration
while allowing for different build rules for each makefile.

Every submakefile shall be named `makefile.mk`.

Submakefiles
============

Each submakefile will "dominate" its directory:
the main makefile will not interfere with the building process
that happens inside the directories that contains a `makefile.mk`.

The `makefile.mk` is allowed to define build rules and targets
for any file inside its directory and any subdirectories.

Mimicking C's convention,
any included makefile may define any additional variable
and any additional target it wants,
provided that they are prefixed with some string.
For instance, [`player_list/makefile.mk`][]
prefixes all its variables with `PLAYERLIST`
and all its targets with `playerlist-`.

Note that these targets and variables are not immediately linked
to the main makefile;
the remainder of this document estabilishes ways that we allow
these makefiles to interact with the main makefile.

Since the `make` program does not handle makefiles in subirectories
like the C preprocessor does,
to be able to refer to the files inside the directory of the makefile
it is convenient to define a `DIR` variable
to contain the path to the makefile's directory
and refer to other files using that variable instead.

Actually, `make` provides a way to obtain such path immediately;
for instance, [`player_list/makefile.mk`][]
contains the code

    PLAYERLIST_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

Canonical targets
=================

There are three makefile targets that are defined in the main makefile.
-   `all`:
    Builds all the programs of this repository.

-   `clean`:
    Deletes every non-source file
    (like binary and dependency files).

-   `mostlyclean`:
    Deletes almost every non-source file;
    like `clean` but might preserve some files
    that are either too costly to rebuild
    or have some special importance to the repository.
    By default, no executable program is deleted with `mostlyclean`.

`mostlyclean` is a dependency of `clean`.

The submakefiles are allowed to add prerequisites to these targets,
effectively extending them.
For instance, [`player_list/makefile.mk`][]
defines two targets `playerlist-mostlyclean` and `playerlist-clean`
and uses them to delete the object file on `mostlyclean`
and the generated `.cpp` on `clean`.

Both `clean` and `mostlyclean` recipes
take care to ignore any directory that have a submakefile,
so extending `clean` and `mostlyclean` might be a necessity.

target `main`
=============
The main makefile will always build, at least,
the program `main`.
Since this program links together all artificial intelligences,
you are allowed to add prerequisites to this target.

For instance, if you insist in using recursive makefiles,
you could create a `my_ai/makefile.mk` like this:

    main: my-ai-all

    my-ai-all:
        cd my_ai && make

This way, whenever main is considered to be rebuilt,
the file `my_ai/makefile` is used to make your AI in a recursive manner.

A less evil option is to add an object file as dependency
and add rules to building that object file.

Compilation and automatic dependency generation hooks
=====================================================

With the above rules, you should be able to adapt any existing makefile
to conform this repository's assumptions.
The main makefile will _not_ touch any file inside your makefile's directory.

Two tasks, however, should be common to the submakefiles:
compiling `.cpp` files to object files,
and automatic dependency generation.

The main makefile does these two tasks for any directory
not under the "control" of a `makefile.mk`.
The automatic dependency generation mechanism is inspired in
<http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/>.

Since these tasks are common, the main makefile provides hooks
so that you can enable this functionality in your makefile
without needing to rewrite that entire code.

There are two variables,
`MAIN` and `NOMAIN`,
that can be appended with `.cpp` files
that you wish the main makefile to compile and generate dependencies for you.

`MAIN` contains every `.cpp` file that can be compiled to an executable;
that is, the files that contains the `main` function.
Any file in this variable will be turned in a target
by stripping the `.cpp` part;
this target is the executable produced when compiling that `.cpp`.
By default, the main makefile includes every file
that matches `^int main`.

`NOMAIN` contains every `.cpp` file without a main function.
By default, the main makefile includes every file
that do _not_ matches "^int main".

You can append any `.cpp` file to these variables to tell the main makefile
to compile it and manage its automatic dependency generation for you.
For instance, [`player_list/makefile.mk`][]
adds its `generate_player_list.cpp` to `NOMAIN`.

`MAIN` and `NOMAIN` shall be mutually excludent.

The makefile genere a `.dep.mk` file for each file in each of these variables.
`make mostlyclean` will remove any `.dep.mk` in any directory,
regardless of the existence of a `makefile.mk`.
This is the only exception to the rule that the main makefile
ignores all subdirectories that contains submakefiles.

Note, however, that `clean` and a`mostlyclean` targets
sill needs to be extended to remove the object files generated.

[`player_list/makefile.mk`]: player_list/makefile.mk
