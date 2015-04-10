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

Convention Synopsis
-------------------

The targets `clean` and `mostlyclean` can be extended
by providing extra dependencies.
The depended targets can, then, execute the cleaning code.
In fact, since the top makefile does not clean object files
in subdirectories that contains makefiles,
at lest the `moslyclean` target must be extended to do this.

To avoid name clashes, any extra target or variable
must be prefixed by the name of the makefile's directory.
For instance, `player_list/makefile.mk` defines the variable
`playerlist` and the target `playerlist-clean`.

The top makefile will, by default,
create build rules for all object files and programs in the directory.
The presence of a submakefile supresses this rule generation.
The submakefile may use the top makefile's hooks
to have that functionality back
and to interact with the rest of the repository.

There are four variables,
in the main makefile,
that may be appended to to request such services.

-   `prog` lists all the programs of this repository.
    This variable will hold a list of targets.
    The object files in these targets' dependencies
    will be linked together to form the specified program.

-   `src` lists all the `.cpp` files that should be built
    into corresponding `.o` files.

-   `dep` lists all the `.dep.mk` files that will be automatically generated
    from the corresponding source files.
    (Note that the extension is `.dep.mk`, not the customary `.d`

-   `obj` lists all the object files that provide symbol definitions
    that might be useful for other targets.
    (This variable is mandatory; it is used to control dependencies.)

There is a special target, `main`,
that will build the main program.
This target may be extended with arbitrary prerequisites.
You can, for instance, define a `.PHONY` target
that runs another makefile
--- thus having a recursive makefile.
(This defeats the purpose of this somewhat complex integration protocol,
but allows for fast integration with arbitrarily complex build systems.)

The remainder of this section is an extended explanation of this summary,
with some examples of use.


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
prefixes all its variables with `playerlist`
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

    playerlist_dir := $(dir $(lastword $(MAKEFILE_LIST)))

Then, all other variable and targets
refer to files relative to `$(playerlist_dir)`.


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
so extending either `clean` or `mostlyclean`
is mandatory.

Target `main`
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

There are two variables that can be used
to request these services from the main makefile.

`src` is a list of `.cpp` files
whose compilation are to be handled by the top makefile.
Any submakefile may append (not overwrite)
to this variable with arbitrary `.cpp` files,
that must be in the same directory as the current makefile
(or in subdirectories).

`dep` is a list of `.dep.mk` files
that are to be generated by the top makefile.
These files contains all the dependencies of the associated `.cpp` file,
in the form of makefile rules.
We use a `.mk` suffix to emphasize this property;
and a `.dep` "presuffix" to say that this is a dependency file.

There must be an acompanying `.cpp` file
for each `.dep.mk` file in the `dep` variable;
or, at least, some makefile rule to generate the `.cpp`.

For instance, [`player_list/makefile.mk`][]
has the code

    playerlist := $(playerlist_dir)generate_player_list.cpp
    src += $(playerlist)
    dep += $(playerlist:.cpp=.dep.mk)

    $(playerlist) : $(playerlist_dep)
        $(playerlist_dir)generate_player_list.sh > $(playerlist)

The first line points to the `.cpp` handled by the submakefile.
The second and third lines "subscribe" that file
to the hooks of the top makefile.
The last two lines define build rules for that `.cpp` file.

If the `.cpp` does not exist in the moment of compilation,
that rule will be invoked to generate the `.cpp` file,
and then the main makefile will compile it to an object file.

Note, however, that `clean` and a`mostlyclean` targets
sill needs to be extended to remove the object files generated.


The `prog` and `obj` variables
==============================

All the compilation steps described above stops at linking.
They merely compile the `.cpp` file to an `.o`
without linking them to executable files.
This allows the `main` function to be defined several times
in different translation units,
allowing multiple programs in the same repository.

The main makefile provide the variable `prog`
with the list of programs that are to be generated by that makefile.
By default, `prog` contains every `.cpp` file
that _does_ define a `main` function,
without the `.cpp` part.
For instance, `main` is in `prog`
because `main.cpp` defines the function `main`.

The other important variable is `obj`.
It contains a list of object files
that are to be shared with other programs.
By default, it contains every `.o` file
whose corresponding `.cpp` file
_does not_ define a `main` function.

For each string `p` in `prog`,
the main makefile defines `p` as a target.
This target depends, by default,
on `p.o` and on every file in `obj`;
the target `all` depends on this target.

The program `p` will be generated by linking together
every object file that it depends on.
This includes at least `p.o` and every file in `obj`.
This has two consequences.

First, since the target depends on every file in `obj`,
whenever any "useful" object file is updated
(most of times, by the build process itself),
`p` will be relinked.
This means a single change in some `.cpp` file
whose object file is included in `obj`
will trigger the relinking of _every_ program defined in the variable `prog`.

This may include several programs;
you can restrain the rebuild time
by specifying which program you want to relink,
using, for instance, `make p`.

(This is admittedly suboptimal; the automatic dependency management
merely reduces the recompilation time, but not the relinking time.
See [`relinking_problem.md`](relinking_problem.md) for details.)

The second consequence is that we can add arbitrary object files
as dependencies of `p`,
without exposing them to the external world through `obj`,
and have the top makefile correctly link them together.

[`player_list/makefile.mk`]: player_list/makefile.mk
