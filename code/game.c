#include "game.h"

int maze_template[MAZE_HEIGHT][MAZE_WIDTH] = {
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
    game_state->player.x = CELL_SIZE + 4; //small offset to wall
    game_state->player.y = CELL_SIZE + 4; //small offset to wall

    for (int y = 0; y < MAZE_HEIGHT; y++) {
        for (int x = 0; x < MAZE_WIDTH; x++) {
            game_state->maze[y][x] = maze_template[y][x];
        }
    }
}

