#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>

typedef Player * ( *PlayerFactory )( int player_num, int starting_chopsticks );

struct Player {
    std::string name() const = 0;

    int hand() = 0;
    int guess( const std::vector<int>& other_guesses ) = 0;
    void settle_round(
        const std::vector<int>& hands,
        const std::vector<int>& guesses
    ) = 0;
};

#endif // PLAYER_H
