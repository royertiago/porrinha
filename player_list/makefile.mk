# Get the directory we currently are; it includes a trailing slash
playerlist_dir := $(dir $(lastword $(MAKEFILE_LIST)))

# The target built by this makefile
playerlist := $(playerlist_dir)generate_player_list.cpp

# Add the only file of this directory
src += $(playerlist)
dep += $(playerlist:.cpp=.dep.mk)
obj += $(playerlist:.cpp=.o)

# This works because we run from the root directory.
playerlist_dep := $(shell find -name "ai.conf" -o -name "ai.h")

$(playerlist) : $(playerlist_dep)
	$(playerlist_dir)generate_player_list.sh > $(playerlist)

mostlyclean: playerlist-mostlyclean
playerlist-mostlyclean:
	rm -f $(playerlist:.cpp=.o)

clean: playerlist-clean
playerlist-clean:
	rm -f $(playerlist)
