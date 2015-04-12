Modus operandi
==============

This section documents the sequence of method calls
made by the game's core. You should know the
[game rules](game_rules.md) before reading this.
To develop a new artificial intelligence for this game,
be sure to know the [file structure](file_structure.md).
If your artificial intelligence has complex build rules,
read the [makefile conventions](makefile_conventions.md) as well.

There is some overlapping with this text and the documentation
in the files [`player.h`](player.h) and [`core/util.h`](core/util.h).
These two files
(together with the game rules)
should be enough for you to develop your artificial intelligence.
This text documents the exact method call sequence,
which is only needed if you plan to to strange function calls at unusal moments
or wants to substitute the game's core with another implementation.
(This is not an absurd possibility:
the core is merely a submodule of the main repository,
so you can reimplement it (say, in a GUI)
and merely change the submodule pointer to your core implementation.)


Factory call
============

Every player is built using the factories
in the very beginning of execution,
before any game has taken place.
A single factory function might be called several times.
Each call corresponds to a _different_ player
that will be chosen to playing games later on.

Some of the attributes of the game will already be set
when the first factory function is called;
see [`core/util.h`][] for details.

After every `PlayerFactory` call is completed,
the game begins.
There is no further invocations of factories
for the remainder of this program execution.


Begin and end of the game
=========================

A single artificial intelligence may participate of several games
(a kind of tournament)
while mantaining its knowledge from previous games.
To ease the statistics collection,
there are two methods,
`Player::begin_game` and `Player::end_game`,
whose calls surround each game.

(This "tournament" feature is not yet implemented...)

The players are arranged in a vector,
each with a different index.
The first player to choses his hand is at index 0,
the second player is at index 1,
and so on.
This vector is traversed circularly through the game.
Most of the round-specific functions of [`core/util.h`][]
is based on this index.
When the first call to `begin_game` is done,
this ordering shall already be set,
and remain unchanged for the duration of the game.
(There is an utility function that retrieves the index
based on a pointer to the player.)

First, every player has `begin_game` called,
then they play the game,
then every player has `end_game` called.
The call order for `begin_game` and `end_game`
is implementation-defined.


Game phases
===========

There are three phases of each round:
choosing hands,
guessing,
and settling the round,
executed in this order.

Each phase is associated with a method call.
The order of the calls follows the game rules:
the first guesser always have its method called first,
followed the next player
according to the index.

The player is advised to use the methods in [`core/util.h`][]
to retrieve information about the current game status
and the other players' moves.
The section "overall game queries" is only updated between rounds,
while the "individual round queries" are updated as the round progresses,
so there may be missing information when a player is asked to make a decision.

Choosing hands
--------------

Simply calls `Player::hand()` on all players that are still playing the game.

If the value returned by the call is invalid
--- negative or greater than the number of chopsticks of that player ---
then it is reseted to some valid value.
Do not rely on this behavior, tough;
on later versions, a player that returns invalid values
or throws exceptions
(in any method call)
might be removed from the game.

(There is no excuse for doing this;
use the utility functions in `core/util.h`
if you are lazy.)

Note that there is no extra useful information
that the player can retrieve from [`core/util.h`][]
in this phase of the game.

Guessing
--------

Calls `Player::guess()` on all players that are still playing the game.

The player may use the function `core::guess`,
in [`core/util.h`][],
to walk through the other players' guesses
in order to extend the avaliable information prior to making its own guess.

If the guess of some player is negative,
then it codifies some information about the player guess
instead of the guess itself.
There are three such values:
-   `core::PENDING_GUESS`:
    Used when that player still hadn't made its guess,
    due to the order of the guesses.
-   `core::NOT_PLAYING`:
    Means that the player is not playing anymore.
    This happens when the player empties its hand.
-   `core::INVALID_GUESS`:
    The player had made something invalid.
    Currently, "invalid" means either
    a value outside the range [0, `total_of_chopsticks`]
    or a repeated value.

If the guess of some player is a nonnegative value,
then it contains the actual guess of the player,
which is guaranteed to always be smaller than
the current total chopstick count
(since a value greater than that would be `core::INVALID_GUESS`).

Note that the first player to make a guess
will have only `core::PENDING_GUESS` and `core::NOT_PLAYING`
as return values for `core::guess`.

Settling the round
------------------

Calls `Player::end_round()` for each player,
even if it's not playing anymore.

At this time, the data in the "end round information"
section of [`core/util.h`][]
shall be valid.

Note that there is no corresponding `Player::begin_round()` method;
since there is no useful information collectable
between the end of the current round and the call of `Player::hand()`
in the next round,
this method is of no necissity in the class `Player`.


Next round
==========

After all three phases of some round ends,
it is checked if some player guessed right;
in this case, that player's chopstick number is decremented,
and, if this number become zero,
that player is taken out of the game.

Then, we calculate who is the next "first guesser",
and start the next round.

Note that all these information is calculated
_before_ the call for `Player::end_round()`,
so each player may record these statistics.


Message printing
================

The progress of the game is reported to `stdout`.
Note that progress reports might interleave
with the game's phases.
Take this into consideration
if your artificial intelligence uses std::cout.

There is a basic guarantee: all output concerning the current round
is displayed before starting the next round.


Error reporting
===============

Dumb AI programmers might let its creation do things outside the game's rules
(like guessing a value that had already been guessed),
or are easily provable stupid
(like guessing a negative value).
The game's core know how to handle some of this situations
in order to keep running,
even in the presence of bad programmers
(or buggy code).

You are advised to use the functions in [`core/util.h`][]
to avoid being outside the game rules.

There is some error reporting built in core's algorithms;
note that these reports might interleave with the program's normal output.

[`core/util.h`]: core/util.h
