Porrinha
========

Assignment of the discipline INE5430 - Artificial Intelligence.

This is a game typically played on bars,
where the loser pays the beer (or some other penalty like that).
See the [game rules](game_rules.md) for more information.


AI integration
==============

Each artificial intelligence has its own repository,
that should be included as a submodule in the root repository.

To allow for integration between several artificial intelligences,
it is imposed some structure in the files and makefiles of the AI's.
This structure is **not** (yet) documented;
you can look to `porrinha-human` and to `player_list/makefile.mk`
to have an example of file and makefile structure,
respectively.
