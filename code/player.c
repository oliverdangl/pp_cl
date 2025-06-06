#include "player.h"
#include "config.h"
#include <stdio.h>
#include <stdbool.h>


//function to set spawn point on first found free field (field = 0)
void spawn_player(PlayerState *player, const Maze *maze){
    bool spwn_found = false;
    player->facing = DIR_DOWN;
    player->lives = MAX_LIVES;
    player->traps_visited = 0;
    
    
    for(int y = 0; y < maze->height && !spwn_found; y++){
        for(int x = 0; x < maze->width; x++){
            if(maze->current[y][x] == 0){
                //spawn location
                player->x = MAZE_OFFSET_X + CELL_SIZE * x + CELL_SIZE / 2.0f;
                player->y = MAZE_OFFSET_Y + CELL_SIZE * y + CELL_SIZE / 2.0f;
                spwn_found = true;
                break;
            }
        }
    }
    if(!spwn_found){
        fprintf(stderr, "Kein Spawn möglich!");
    }
}
