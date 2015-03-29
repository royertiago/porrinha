Porrinha
========

Assignment of the discipline INE5430 - Artificial Intelligence.

This is a game typically played on bars,
where the loser pays the beer (or some other penalty like that).
See the [game rules](game_rules.md) for more information.

The code is licensed under GPL version 3 or any later version.


Compiling
=========

To compile the game sources,
you need a `gcc`-compatible compiler
that supports C++14.
(The makefile uses the `CXX`, that can be set by the user
to an appropriate compiler.)

The game was tested with `gcc 4.9.1` and `clang 3.5.0`.


Implementing additional artificial intelligences
=================================================

The code of this repository is organized so that it is possible to have
several artificial intelligences battling simultaneously
in the same game.

There is a [common interface](player.h) that must be implemented
by all artificial intelligences.
How this interface is used by the game is documented in the
[modus operandi][]

Each artificial intelligence is in a subdirectory
(normally a git submodule)
that follows some specific file structure.
The file structure is documented [here][file structure]

If your artificial intelligence does not have complicated build rules
and can be described as a set of `.cpp` and `.h`/`.hpp` files,
then reading the [modus operandi][] and the [file structure][]
should be enough.

If your artificial intelligence needs special build rules
with a custom makefile, you must also read the
[makefile conventions](makefile_conventions.md) of this repository.

[modus operandi]: modus_operandi.md
[file structure]: file_structure.md
