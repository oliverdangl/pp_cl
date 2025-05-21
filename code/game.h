#ifndef GAME_H
#define GAME_H

#include <cairo.h>
#include <stdbool.h>

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 800
#define CELL_SIZE 40
#define PLAYER_HITBOX_SIZE 32
#define MAX_LIVES 3

typedef enum {
    EMPTY = 0,
    WALL = 1,
    TRAP = 2
}   CellType;

typedef struct {
    float x, y;
    cairo_surface_t *sprite_short;
    cairo_surface_t *sprite;
} Player;

typedef struct {
    Player player;
    int *pressed_keys;
    unsigned int num_pressed_keys;
    
    
    int **maze;
    int **original_maze; //storing original for reset call
    int maze_width;
    int maze_height;
    
    
    int lives;
    int trap_visited;
} GameState;

bool load_maze_from_file(GameState *game_state, const char *filename);
void reset_game(GameState *game_state);
void free_maze(GameState *game_state);

#endif

