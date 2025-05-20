#include "game.h"
#include <stdlib.h>

static int maze_template[MAZE_HEIGHT][MAZE_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,2,1},
    {1,2,1,0,1,0,1,1,0,1,0,1,1,1,1,1,1,1,0,1},
    {1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,1},
    {1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1},
    {1,0,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,0,1},
    {1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1},
    {1,0,1,0,1,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1},
    {1,0,2,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

/*
Function to reset a game by reseting player location and redrawing the maze
*/
void reset_game(GameState *game_state) {
    game_state->lives = MAX_LIVES;
    game_state->trap_visited = 0;
    game_state->player.x = CELL_SIZE + 4;
    game_state->player.y = CELL_SIZE + 4;

    // Free maze if existing
    if (game_state->maze) {
        free_maze(game_state);
    }

    // Reallocate maze
    game_state->maze = malloc(MAZE_HEIGHT * sizeof(int *));
    for (int y = 0; y < MAZE_HEIGHT; y++) {
        game_state->maze[y] = malloc(MAZE_WIDTH * sizeof(int));
        for (int x = 0; x < MAZE_WIDTH; x++) {
            game_state->maze[y][x] = maze_template[y][x];
        }
    }
}


void free_maze(GameState *game_state) {
    if (!game_state->maze) return;

    for (int y = 0; y < MAZE_HEIGHT; y++) {
        free(game_state->maze[y]);
    }
    free(game_state->maze);
    game_state->maze = NULL;
}
