#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>

struct Player {
    virtual std::string name() const = 0;

    virtual int hand() = 0;
    virtual int guess( const std::vector<int>& other_guesses ) = 0;
    virtual void settle_round(
        const std::vector<int>& hands,
        const std::vector<int>& guesses
    ) = 0;

    virtual ~Player() = default;
};

typedef Player * ( *PlayerFactory )( int player_num, int starting_chopsticks );

#endif // PLAYER_H
