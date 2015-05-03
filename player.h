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
#include "cmdline/args.hpp"

struct Player {
    /* Name of this player.
     * This name will be used to identify the player
     * during the game and on placar screens,
     * so this name should ideally be unique across all players
     * (or, at least, across all players from the same AI).
     */
    virtual std::string name() const = 0;

    /* Tell this player that a new game will start.
     *
     * This method is called exactly once at beginning of each game.
     * All the core:: functions in the section "Overall game queries"
     * shall be valid and correct when this method is called.
     */
    virtual void begin_game() = 0;

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
     * Note that there is no corresponding 'begin_round' method;
     * when the round "officially" starts, the method 'hand' will be called,
     * so round begin can be easily deduced using this information.
     */
    virtual void end_round() = 0;

    /* Tell this player the game ended.
     *
     * The "Overall game queries" will still be valid when this method is called,
     * to allow for players to record statistics for future games.
     */
    virtual void end_game() = 0;

    virtual ~Player() = default;
};

/* These functions are used by the game's core
 * to generate all the players for a given game.
 * Note that a single factory function might be called several times.
 *
 * The "program_name" of the argument vector
 * is the string wherewith this playew as chosen.
 */
typedef Player * ( *PlayerFactory )( cmdline::args&& );

#endif // PLAYER_H
