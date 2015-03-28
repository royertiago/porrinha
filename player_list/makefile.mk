# Get the directory we currently are; it includes a trailing slash
PLAYERLIST_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

# The target built by this makefile
PLAYERLIST := $(PLAYERLIST_DIR)generate_player_list.cpp
PLAYERLIST_OBJ := $(PLAYERLIST:.cpp=.o)

# Add the only file of this directory
NOMAIN += $(PLAYERLIST)

# This works because we run from the root directory.
PLAYERLIST_DEP := $(shell find -name "ai.conf" -o -name "ai.h")

$(PLAYERLIST) : $(PLAYERLIST_DEP)
	$(PLAYERLIST_DIR)generate_player_list.sh > $(PLAYERLIST) 

mostlyclean: playerlist-mostlyclean
playerlist-mostlyclean:
	rm -f $(PLAYERLIST_OBJ)

clean: playerlist-clean
playerlist-clean:
	rm -f $(PLAYERLIST)
