#ifndef GAME_H
#define GAME_H

#include <cairo.h>
#include <stdbool.h>

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 800
#define CELL_SIZE 40
#define MAX_LIVES 3

// Offset-Definitionen, damit das Spielfeld nach unten/innen versetzt wird
#define MAZE_OFFSET_X 20
#define MAZE_OFFSET_Y 60

// Enum for map cell content
typedef enum {
    EMPTY = 0,
    WALL = 1,
    TRAP = 2
} CellType;

// Struct for the player
typedef struct {
    float x, y;
    cairo_surface_t *sprite_sheet;
    cairo_surface_t *sprite;
    int facing_direction; // 0 = up, 1 = left, 2 = down, 3 = right
} Player;


// Struct for game logic
typedef struct {
    Player player;
    int *pressed_keys;
    unsigned int num_pressed_keys;

    // area for maze information
    int **maze;
    int **original_maze; // storing original for reset call
    int maze_width;
    int maze_height;

    // area for other information 
    int lives;
    int trap_visited;
} GameState;

// Declaration of functions used in game.c
bool load_maze_from_file(GameState *game_state, const char *filename);
void reset_game(GameState *game_state);
void free_maze(GameState *game_state);

#endif


