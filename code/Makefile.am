#Makefile for the auto make generating input for the configure.ac

bin_PROGRAMS = maze_game
maze_game_SOURCES = main.c input.c game.c draw.c


#AM_CFLAGS = `pkg-config --cflags gtk+-3.0` #Tells compiler where headers are
maze_game_CFLAGS = $(GTK_CFLAGS)#Compile flags

#maze_game_LDFLAGS = `pkg-config --libs gtk+-3.0` #Tells linker which librarys are needed
maze_game_LDADD = $(GTK_LIBS) #Linker flags




