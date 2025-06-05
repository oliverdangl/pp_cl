# Makefile.am for the Maze Game project

bin_PROGRAMS = maze_game
maze_game_SOURCES = main.c input.c game.c draw.c args.c

# Enable GNU extensions (for argp) and include GTK compiler flags
AM_CFLAGS        = -D_GNU_SOURCE $(GTK_CFLAGS)
maze_game_CFLAGS = $(AM_CFLAGS)

# Link against GTK libraries and the math library
maze_game_LDADD  = $(GTK_LIBS) -lm

