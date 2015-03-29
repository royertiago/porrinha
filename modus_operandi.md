Modus operandi
==============

(This section documents the sequence of method calls
made by the game's core. You should know the
[game rules](game_rules.md) before reading this.
To develop a new artificial intelligence for this game,
be sure to know the [file structure](file_structure.md)
and [makefile conventions](makefile_conventions.md).)

(There is some overlapping with [`game_rules.md`](game_rules.md).)

Using factory functions
(explained better in [`file_structure.md`](file_structure.md)),
several players are generated
according to the command-line arguments.

The command-line arguments also specify the order of the players.
To each player is assigned an index that stays the same the entire game.
The first player is at index 0, the second is at index 1 and so on.

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

Guessing
--------

Calls `Player::guess()` on all players that are still playing the game.

The `other_guesses` vector contains a list of all the guesses
that the other players already made.
The indexes in the passed vector is the same index
described here (and used by the game core).

If the guess of some player is negative,
then it codifies some information about the player guess
instead of the guess itself.
There are three such values:
-   `Player::PENDING`:
    Used when that player still hadn't made its guess,
    due to the order of the guesses.
-   `Player::NOT_PLAYING`:
    Means that the player is not playing anymore.
    This happens when the player empties its hand.
-   `Player::INVALID`:
    The player had made something invalid.
    Currently, "invalid" means either
    a value outside the range [0, `total_of_chopsticks`]
    or a repeated value.

If the guess of some player is a nonnegative value,
then it contains the actual guess of the player,
which is guaranteed to always be smaller than
the current total chopstick count
(since a value greater than that would be `player::INVALID`).

Each call is used to construct the `other_guesses` vector,
that is then passed to the next method calls;
thus, to the first guesser is given a vector with all
`Player::PENDING` or `Player::NOT_PLAYING`.

The first player to ever make a guess is the player number 0.

Settling the round
------------------

Calls `Players::settle_round()` for each player,
even if it's not playing anymore.

The `hands` vector contains all the results of calling
`Player::hand()` on each player.
It always have a valid value.

The `guesses` vector follows the same conventions
as the `other_guesses` vector above.

Next round
==========

After all three phases of some round ends,
it is checked if some player guessed right;
in this case, that player's chopstick number is decremented,
and, if this number become zero,
that player is taken out of the game.

Then, we calculate who is the next "first guesser",
and start the next round.

Message printing
================

The progress of the game is reported to `stdout`.
Note that progress reports might interleave
with the game's phases.
Take this into consideration
if your artificial intelligence uses std::cout.

There is a guarantee: all output concerning the current round
is displayed before starting the next round.

`core/util.h`
=============

The functions defined in this file allows programmers to query
the current game state about several aspects,
regarding both overall game state
(like the total number of chopsticks)
and specific information about players
(like the number of chopsticks a player have).

Use the functions of this file to avoid being
"outside the rules".

Error reporting
===============

Dumb AI programmers might let its creation do things
that are outside the game's rules
(like guessing a value that had already been guessed),
or are easily provable stupid
(like guessing a negative value).
The game's core know how to handle some of this situations
in order to keep running,
even in the presence of bad programmers
(or buggy code).

You may use the functions in `core/util.h`
to avoid being outside the game rules.

There is some error reporting built in core's algorithms;
note that these reports might interleave with the program's normal output.
