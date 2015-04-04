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
     * The values of other_guesses follows the same conventions
     * as core::guess(), in the file core/util.h.
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

/* These functions are used by the game's core
 * to generate all the players for a given game.
 * Note that a single factory function might be called several times.
 *
 * Parameters:
 *  player_count is the number of players in the game.
 *  starting_chopsticks is the number of chopsticks each player starts with.
 */
typedef Player * ( *PlayerFactory )( int player_count, int starting_chopsticks );

#endif // PLAYER_H
