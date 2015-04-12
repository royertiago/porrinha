Tutorial
========

(This tutorial mimicks the tone of the tutorials
for the `beamer` and TikZ LaTeX packages.)

The full code of this tutorial is avaliable
[here](https://github.com/royertiago/porrinha-random).

Ricardo likes to play with artificial intelligences.
He heard that the Porrinha game is organized
to permit modular integration between
several different artificial intelligences,
so he would like to develop its own AI for this game
and fight against other artificial intelligences.

Ricardo decides to first create an "artificial intelligence"
that does random moves,
simply to learn how the AIs are integrated by the game.

He begins by reading the [game rules](game_rules.md),
and taking a look in the file [`player.h`][]
and [`core/util.h`][].
The latter two files are well-documented,
so much of the information Ricardo needs to creating an AI
he finds there.

He proceeds by creating a git repository and adding a README file.
He knows that all artificial intelligences have its own repository.
This automates the process of adding a new artificial intelligence
to the base repository.
Ricardo does not understand, though,
why the game's core is also a separate repository,
but decides to ignore this for now.


Random number generation
------------------------

Ricardo knows he will need a random number generator
to its artificial intelligence.
He knows how to use the C++ `random` library,
but would like to "hide" all that ugliness
when implementing its AI,
so he proceeds by creating a header file
that concentrates all the functionality he will need.

    namespace random_player {
        /* Returns a random number in the range [0, max]. */
        int random( int max );
    }

(Ricardo had read in the [readme file](README.md)
that all artificial intelligences have its own namespace.)
He saves this code in the file `random.h`.

The implementation goes in `random.cpp`.
The code includes global variables inside the namespace,
but Ricardo does not care since he aims to understand the core of the program
instead of creating beautiful code.

    #include <chrono>
    #include <random>
    #include "random.h"

    namespace random_player {
        std::mt19937 random_engine;
        bool engine_initialized = false;

        int random( int max ) {
            if( !engine_initialized )
                random_engine.seed(
                    std::chrono::system_clock::now().time_since_epoch().count()
                );

            return std::uniform_int_distribution<int>(0, max)( random_engine );
        }
    } // namespace random_player


The artificial intelligence
---------------------------

Ricardo is happy with its random number generator
and proceeds to actually implementing its AI.
He knows that his artificial intelligence
must implements the `core` interface,
defined in the file [`player.h`][].

His artificial intelligence is very boring,
so he does not put any field in its class.
Since there is no private members,
he chooses to use a struct instead of a class,
just to get rid of the `public:` that he would need to put
in the beginning of the class.

    #include "player.h"

    namespace random_player {

        struct Randomcore : public Player {
            std::string name() const override;
            void begin_game() override;
            int hand() override;
            int guess() override;
            void end_round() override;
            void end_game() override;
        };

    } // namespace random_player

He saves this file as `random_player.h`.

The default compiler flags
specifically allows source files to refer to other source files
by its name relative to the root directory.
So, when Ricardo's AI is included as a git submodule in the main repository,
everything will work correctly.

Ricardo then implements its artificial intelligence
in `random_player.cpp`.

    #include "random_player.h"
    #include "random.h"

    namespace random_player {
        std::string Randomcore::name() const {
            return "Random";
        }
    }

The name is used to refer to the artificial intelligence in the game output.

Looking at [`player.h`][],
Ricardo learns he needs to implement the methods
`core::begin_game`, `Player::end_game`, and `Player::end_round`.
These methods are mainly used so that more elaborated artificial intelligences
can record statistics about past games
and ease the initialization of game-specific data structures.
Ricardo's AI does not record anything,
so he just implement then as no-ops.

    namespace random_player {

        void Randomcore::begin_game() {
            // no-op
        }

        void Randomcore::end_round() {
            // no-op
        }

        void Randomcore::end_game() {
            // no-op
        }

    } // namespace random_player

It only remains to implement the "core" of his artificial intelligence:
the methods `core::hand` and `Player::guess`.

    namespace random_player {

        int Randomcore::hand() {
            return random(3)));
        }

        int Randomcore::guess() {
            return random(9);
        }

    } // namespace random_player

Ricardo definitively does not like that hardcoded `3` and `9`,
but he decides to solve it later.

Ricardo has his artificial intelligence fully implemented.

Integration with other artificial intelligences
-----------------------------------------------

To allow interation with other artificial intelligences,
Ricardo must create two more files,
named `ai.h` and `ai.conf`,
in his repository.
The code generation of the makefile
depends on these files being named exactly `ai.h` and `ai.conf`.

`ai.h` contains a coreFactory function
--- a function that takes as an argument `cmdline::args&&`.
The class cmdline::args encapsulates an argument vector
(similar to the `String args[]` of Java)
but with some helper methods that allow command line argument parsing
in a way very similar to shell scripting.

    namespace random_player {
        core * generate( cmdline::args&& );
    } // namespace human_player

Ricardo gets confused by the obscure format of `ai.conf`.
He begins by peeking the `ai.conf` of the human player.

    {"human", human_player::generate},

Ricardo changes `"human"` to `"random"`
and `human_player` to `random_player`,
resulting in his `ai.conf` file:

    {"random", random_player::generate},

Ricardo wonders why there must be a pair of braces around its texts
and screams internally about the trailing comma,
but defer to understand why this file must be so arcane to later.
(On a later read, he discovers the reasons reading
[`file_structure.md`](file_structure.md).)

Ricardo implements the function `random_player::generate` in `ai.cpp`.
For now, he decides to ignore the command line arguments.

    #include "random_player.h"
    #include "ai.h"

    namespace random_player {

        core * generate( cmdline::args&& ) {
            return new Randomcore;
        }

    } // namespace random_player

His artificial intelligence is finished!
Now, Ricardo only needs to integrate it with the main repository.

### Git submodule

Ricardo finally adds its repository as a submodule of the main repository.
Ricardo then types

    make

on the command line, and all is built correctly.

Done!
Ricardo succesfully developed an artificial intelligence for Porrinha,
and can put two of them to battle using

    ./main random random

Ricardo notices that the text `random` is exactly the string
that he had put in its `ai.conf`.
That's not coincidence: that string is used to uniquely identify
the artificial intelligence on the command-line
and have nothing to do with the method core::name().

When executing `git status` on the submodule,
Ricardo notices that there are six new files;
three with the extension `.o` and three with `.dep.mk`.
The `.o` files are object files,
generated from the `.cpp` files during the compilation process.
The `.dep.mk` are dependency files.
These files are generated by the makefile
to keep track on which source file each object file depends on,
thus allowing the makefile to rebuild only the minimum necessary files
when recompiling the main file.

The main repository contains `.gitignore` rules to ignore these files,
but these rules do not propagate to git submodules;
so, Ricardo puts a `.gitignore` in his repository.

However, the makefile `clean` target _does_ propagate to submodules,
so, when doing a `make clean`,
these files are correctly erased.

The interface [`core/util.h`][]
===========================

Ricardo is upset that the game rants about its artificial intelligence
making "stupid" moves
(like trying to play 3 chopsticks when it have only two)
and decides to rewrite his AI to strictly conform the game rules.

However, he do not like the idea of having to reimplement the game rules;
so, he decides to use the header `core/util.h`,
that gives him the possibility to query the state of the game.

(Rationale: on previous versions of the `core` interface,
the methods `core::guess` and `Player::end_round`
had, as parameters, `std::vector`s that contained other player's guesses
and plays.
These vectors were near to useless,
because artificial intelligence implementors needed to reimplement the game rules.
Thus, some query functions were added to `core/util.h`
to allow AI implementors to query the game state.
To avoid having the same information come from different sources
--- thus violating the "Dont Repeat Yourself" principle ---
these arguments were removed from the interface.)

The function `core::chopsticks` can be used
to query the number of chopsticks any player has,
given its index.
The function `core::index` returns the index of a player
based on its pointer.
Ricardo can use `core::index(this)` to get the index of the current object
and then `core::chopsticks` to find the upper limit
for the `hand` method.
The modified code looks like

    int Randomcore::hand() {
        return random(core::chopsticks(core::index(this)));
    }

The function `core::chopstick_count`
returns the sum of chopsticks in the game.
Ricardo can use this to set an upper limit for the random number
in the `guess` method;
however, the generated value might still be invalid
because anoter player may already had guessed that value.
Luckily, there is a function `core::valid_guess`
that determines wether the passed value
is a valid guess for the current player.
He combines both functions this way:

    int Randomcore::guess() {
        int guess = random(core::chopstick_count());
        while( !core::valid_guess(guess) )
            guess = random(core::chopstick_count());
        return guess;
    }

Ricardo is pleased that he got rid of the hardcoded constants.

Ricardo knows that Randomcore::guess now might run forever;
however, the probability of this hapenning decreases exponentially
at each loop iteration,
so he does not modify its code.

Since both [`player.h`][] and [`core/util.h`][]
are reasonably well documented,
Ricardo can read that header files directly
as references on how the game works.

[`player.h`]: player.h
[`core/util.h`]: core/util.h
