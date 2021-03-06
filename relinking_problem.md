Relinking problem
=================

This is a more detailed discussion on the problem
that causes our makefile to have suboptimal dependency management
for object files.

This text is related to an issue in the makefile implementation;
it has nothing to do with the game in this repository.

In the main makefile, we have a variable `prog`
that contains every program generated by the makefile.
By default, this variable contains every `.cpp`
that defines a `main` function,
without the `.cpp` part.
For instance, `prog` contains `main`,
because `main.cpp` defines a `main` function.

The `obj` is the opposite:
it contains all object files
whose corresponding `.cpp` file
_does not_ define a `main` function.
For instance, `generate_player_list.cpp`
does not define a `main` function,
so `generate_player_list.o` is in `obj`.

(Appending a `.o` to every variable in `prog`,
we have the exact opposite of `obj`.
Note that, for each program,
the corresponding `.o` file has the symbol `main` defined.)

We have automatic dependency generation for source files.
Given a `.cpp`, we can discover
(through the flags `-MM`, `-MF`, etc)
what are all the header files that are included by that `.cpp`.
This header files are all the dependencies
of the `.o`: whenever any header change,
the `.o` might be outdated
and needs to be recompilated.

However, this helps nothing with the dependencies
between _object_ files.
We still have no idea on what object files
the `main` program depends, for instance.

My solution
-----------

In the makefile, we simply make every program
depend on every object file defined in `obj`.
Whenever a program needs relinking,
it _will_ be rebuild by the makefile.

This solution is _far_ from optimal
because even a `touch` in an "innocent" `.cpp` file
will trigger full relinking of the entire repository.

At the time of this writing, I have no idea how to solve this,
without resorting to this kludge
or to some monumental work.

The remainder of this text shows where alternative solutions
either fail to _guarantee_ rebuilding when necessary,
or require the programmer to do work than should be `make`'s responsibility.

Using the dependencies of the corresponding `.o` file
-----------------------------------------------------

Even tough we know which `.h` files the program depends on
--- via the dependencies of the `.o` file ---,
we cannot simply assume that every corresponding `.cpp` file
contains all the missing symbols.

Consider this situation:
`p.cpp` defines a `main` function and includes `file.h`.
`file.cpp` includes `util.h`, but `file.h` does not.
So, the automatic dependency generation
will link `p.o` to `file.h` only, not to `util.h`.
That's correct: if `util.h` is rewritten,
it is `file.o` that needs recompilation,
not `p.o`.

But `p` needs relinking. If we trust only the dependencies of `p.o`,
the makefile will never know `p` needs relinking,
and we will only update `p` if we,
knowing it needs relinking,
delete `p` and asks `make` to build it
--- but knowing this should be `make`'s job, not ours.


Inject dependencies between the `.o` files
------------------------------------------

We need first to adopt a strict coding guideline:
any symbol defined in `file.h` _must_ be implemened in `file.cpp`.
Thus, any symbol needed by `file.o`
will be in one of the object files
whose corresponding header files are included by `file.cpp`.

If `file.o` depends on `file.h` and `util.h`,
we can make `file.o` depends on `util.o`
(taking care to not create the circular dependency
"`file.o` depends on `file.o`").
That is, inject the dependency between `file.o` and `util.o`.

This works partially.
If some object file is changed,
every object file that "depends" on it will be treated as outdated
and will be recompiled.
This process is cascaded until `main`,
which will be both recompiled and relinked.

Altough this does work,
it defeats the purpose of makefiles of doing the least work possible.
We would have done several recompilations
in place of a single relinking.
And we still need to link `p` with the whole `obj`.

(An even more kludgey approach is to create .objdep.mk files
only to handle these dependencies between objects.
This approach seems to work, but it is very ugly
and may have hidden problems in it.)

Rewrite `ld` to output the used object files
--------------------------------------------

We could start by giving `p` the benefit of doubt
and linking it, for the first time,
to every file in `obj`;
and have the program `ld`
somehow write to a `.dep.mk` file
the list of object files that were effectively used
in the `p` construction.

This fails if we make `file.cpp` include a new file `other.h`
and use a symbol defined in `other.cpp`:
we have no way of knowing that the symbol
is, in fact, defined in `other.cpp`;
in principle, it could be defined anywhere in the repository.

But let's assume we have strict coding guidelines
and that symbol is actualy defined is `other.cpp`.
`make` would note that `p` is outdated
becakse `file.o` is outdated
becakse `file.cpp` is outdated.
It will then proceed to recompile `file.o`,
and then relink `p`.
However, `make` did not changed its dependency graph
between the recompilation and the relinking;
the makefile still does not know `p` depends on `other.o`.
`make` will then proceed to relink `p`.
If the relinking is done using only the object files
we discovered were necessary in the last linking,
we _will_ miss `other.o`.
So, we must link with every file in `obj`.

Apart from the herculean job of diving in the `ld`'s codebase,
this is the cleanest solution I can think.
