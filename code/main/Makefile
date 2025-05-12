# Makefile für das GTK-Projekt

CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0 cairo`
LDFLAGS = `pkg-config --libs gtk+-3.0 cairo`
SRC = main.c  # Alle Quellcode-Dateien
OBJ = $(SRC:.c=.o)
OUT = maze_game  # Name der ausführbaren Datei

# Standardziel
all: $(OUT)

# Erzeugung der ausführbaren Datei
$(OUT): $(OBJ)
	$(CC) $(OBJ) -o $(OUT) $(LDFLAGS)

# Kompilieren der Objektdateien
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Ziel zum Bereinigen
clean:
	rm -f $(OUT) $(OBJ)

# Ziel für den automatischen Build (optional)
.PHONY: all clean
