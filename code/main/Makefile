all: maze_game clean_objs

maze_game: $(OBJS)
	$(CC) -o maze_game $(OBJS) $(LIBS)

clean_objs:
	rm -f *.o

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o maze_game

