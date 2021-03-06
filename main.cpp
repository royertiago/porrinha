#include <utility>
#include "player.h"
#include "core/game.h"

// This function is generated by player_list/generate_player_list.sh.
std::vector< std::pair<const char *, PlayerFactory> > generate_player_list();

int main( int argc, char ** argv ) {
    return core::play( cmdline::args(argc, argv), generate_player_list() );
}
