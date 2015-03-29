#include "player.h"

/* Even tough Player::PENDING, Player::NOT_PLAYING and Player::INVALID
 * are `static constexr`, they still are lvalues.
 * constexpr merely says to the compiler that their value
 * can be computed at compile-time; it still possible to
 * (e.g.) bind references to them, and thus,
 * they must be defined somewhere.
 *
 * Interestingly, their definition here must also say that
 * this values are `constexpr`...
 */
constexpr int Player::PENDING;
constexpr int Player::NOT_PLAYING;
constexpr int Player::INVALID;
