CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0` -Wall -g
LIBS = `pkg-config --libs gtk+-3.0` -lm

OBJS = main.o game.o draw.o input.o

all: maze_game

maze_game: $(OBJS)
	$(CC) -o maze_game $(OBJS) $(LIBS)
	rm -f $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o maze_game

