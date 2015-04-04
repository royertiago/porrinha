#ifndef PLAYER_H
#define PLAYER_H

/* A player of the porrinha game.
 *
 * The semantis of each method (and their call order)
 * is better documented in modus_operandi.md;
 * this is a resumed version.
 *
 * This interface contains only the minimum amount of information
 * to a player make its move.
 * Any non-trivial artificial intelligence will have to request
 * information from the game, avaliable in core/util.h.
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

    /* Ask this player the number of chopsticks that it will held
     * in the table this round.
     *
     * If it's an invalid number (smaller than zero
     * or greater than the number of avaliable chopsticks),
     * it will be resetted by the game to some default value.
     */
    virtual int hand() = 0;

    /* The guess to about the total number of chopsticks
     * held in the table in this round.
     */
    virtual int guess() = 0;

    /* Inform this player that the round ended.
     * This method is called exactly once per round,
     * after every guess had been computed.
     *
     * Note there is no notification to this player that the game ended;
     * this information can be deduced inspecting core::active_player_count().
     */
    virtual void settle_round() = 0;

    virtual ~Player() = default;
};

/* These functions are used by the game's core
 * to generate all the players for a given game.
 * Note that a single factory function might be called several times.
 *
 * The argc/argv parameters function exactly like the argc and argv
 * of the main function, except argv[argc] might have its value changed
 * after the function is called.
 *
 * In particular, all the following is granted to be true:
 * - argv[0] is the string wherewith this player was chosen.
 * - argv[argc] is guaranteed to be a null pointer.
 * - The called function are allowed to modify both argv
 *   and the strings it points to
 *   (always taking care to not overrun buffers).
 * - The called function shall not write to any value
 *   outside argv[0], ..., argv[argc],
 *   nor delete argv or any string it points.
 */
typedef Player * ( *PlayerFactory )( int argc, char * argv[] );

#endif // PLAYER_H
