# Makefile für das GTK-Projekt

CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0`
LDFLAGS = `pkg-config --libs gtk+-3.0`
SRC = main.c  # Deine C-Datei
OUT = maze_game  # Name der ausführbaren Datei

# Standardziel
all: $(OUT)

# Erzeugung der ausführbaren Datei
$(OUT): $(SRC)
	$(CC) $(SRC) -o $(OUT) $(CFLAGS) $(LDFLAGS)

# Ziel zum Bereinigen
clean:
	rm -f $(OUT)

# Ziel für den automatischen Build (optional)
.PHONY: all clean

