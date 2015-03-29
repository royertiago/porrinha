#ifndef PLAYER_H
#define PLAYER_H

/* A player of the porrinha game.
 *
 * The semantis of each method (and their call order)
 * is better documented in modus_operandi.md;
 * this is a resumed version.
 */

#include <string>
#include <vector>

struct Player {
    /* Name of this player.
     * This name will be used to identify the player
     * during the game and on placar screens,
     * so this name should ideally be unique across all players
     * (or, at least, across all players from the same AI).
     */
    virtual std::string name() const = 0;

    /* Number of chopsticks that this player will held
     * in the table this round.
     *
     * If it's an invalid number (smaller than zero
     * or greater than the number of avaliable chopsticks),
     * it will be resetted by the game to some default value.
     */
    virtual int hand() = 0;

    /* The guess to about the total number of chopsticks
     * held in the table in this round.
     *
     * The vector other_guesses contains all the guesses
     * that the other players made.
     *
     * If other_guesses[i] >= 0, then other_guesses[i]
     * is guaranteed to be, at most, the total number of choptsicks in the table.
     * It can have three negative values:
     *  -1: the player still did not made its guess.
     *  -2: the player is not playing anymore.
     *  -3: the player made an invalid guess.
     *
     * Note that the guess of this player is, still, -1
     * (since its guess was not recorded).
     */
    virtual int guess( const std::vector<int>& other_guesses ) = 0;

    /* Inform this player that the round ended.
     * hands[i] is what the i-th player's method hand() returned;
     * guesses[i] is wath the i-th player's method guess() returned.
     *
     * Negative guesses have the same meaning as in Player::guess().
     */
    virtual void settle_round(
        const std::vector<int>& hands,
        const std::vector<int>& guesses
    ) = 0;

    virtual ~Player() = default;
};

typedef Player * ( *PlayerFactory )( int player_num, int starting_chopsticks );

#endif // PLAYER_H
