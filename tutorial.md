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

He begins by creating a git repository and adding a README file.
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

Ricardo implements this function in `random.cpp`.
His code includes global variables inside the namespace,
but he does not care since he aims to understand the core of the program
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
and proceeds to _actually_ implementing its AI.
He knows that his artificial intelligence
must implements the `Player` interface,
defined in the file `player.h`.

His artificial intelligence is very boring,
so he does not put any field in its class.
Since there is no private members,
he chooses to use a struct instead of a class,
just to get rid of the `public:` that he would need to put
in the beginning of the class.

    #include "player.h"

    namespace random_player {

        struct RandomPlayer : public Player {
            std::string name() const override;
            int hand() override;
            int guess( const std::vector<int>& other_guesses ) override;
            void settle_round(
                const std::vector<int>& hands,
                const std::vector<int>& guesses
            ) override;
        };

    } // namespace random_player

He saves this file as `random_player.h`.

Ricardo is pleased to discover that the compiler flags
specifically allows source files to refer to other source files
by its name relative to the root directory.
So, when he includes his repository as a git submodule in the main project,
everything will work fine.

Ricardo then implements its artificial intelligence
in `random_player.cpp`.
Since the intelligence plays randomly,
every argument to its methods are ignored,
so Ricardo choses to put them in commentaries
to document that he intend to ignore that arguments.

    #include "random_player.h"
    #include "random.h"

    namespace random_player {
        std::string RandomPlayer::name() const {
            return "Random";
        }

        int RandomPlayer::hand() {
            return random(3);
        }

        int RandomPlayer::guess( const std::vector<int>& /*other_guesses*/ ) {
            return random(9);
        }

        void RandomPlayer::settle_round(
            const std::vector<int>& /*hands*/,
            const std::vector<int>& /*guesses*/
        ) {
            // no-op
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
He complains that the files must be named exactly
`ai.h` and `ai.conf`,
but does so anyway.

`ai.h` merely contains a PlayerFactory function
--- a function that takes two `int`s and returns a `Player *`.

    namespace random_player {
        Player * generate( int players, int chopsticks );
    } // namespace human_player

Ricardo knows that this file will be included
by some automatically generated source file,
that will gather all these functions.
He thinks that this file seems reasonable.
But Ricardo don't quite understand why the `ai.conf`
must follow an obscure format.

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

Now, Ricardo have another function to implement
(`random_player::generate`);
he decides to do so on `ai.cpp` for no particular reason.

    #include "random_player.h"
    #include "ai.h"

    namespace random_player {

        Player * generate( int /*players*/, int /*chopsticks*/ ) {
            return new RandomPlayer;
        }

    } // namespace random_player

Ricardo could have used the variable `players`
and `chopsticks` to parametrize the code of the player,
but, since the player does random movements,
he simply ignore that values.

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
and have nothing to do with the method Player::name().

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

The interface `core/util.h`
===========================

Ricardo is upset that the game rants about its artificial intelligence
making "stupid" moves
(like trying to play 3 chopsticks when it have only two)
and decides to rewrite his AI to strictly conform the game rules.

However, he do not like the idea of having to reimplement the game rules;
so, he decides to use the header `core/util.h`,
that gives him the possibility to query the state of the game.

The function `core::chopsticks` can be used
to query the number of chopsticks any player has,
given its index.
The function `core::index` returns the index of a player
based on its pointer.
Ricardo can use `core::index(this)` to get the index of the current object
and then `core::chopsticks` to find the upper limit
for the `hand` method.
The modified code looks like

    int RandomPlayer::hand() {
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

    int RandomPlayer::guess( const std::vector<int>& /*other_guesses*/ ) {
        int guess = random(core::chopstick_count());
        while( !core::valid_guess(guess) )
            guess = random(core::chopstick_count());
        return guess;
    }

Ricardo is pleased that he got rid of the hardcoded constants.

Ricardo knows that RandomPlayer::guess now might run forever;
however, the probability of this hapenning decreases exponentially
at each loop iteration,
so he does not modify its code.

There are other functions in that file.
Ricardo decides to read the file directly,
since their descriptions lie above their declaration in the code.
