echo '#include <utility>' 
echo '#include "player.h"'
find -name "ai.h" -printf '#include "%P"\n'
echo
echo 'std::vector< std::pair<const char *,' \
     'PlayerFactory> > generate_player_list() {'
echo 'return std::vector<std::pair<const char *, PlayerFactory>>{'
find -name "ai.conf" -printf '#include "%P"\n'
echo '};'
echo '}'
