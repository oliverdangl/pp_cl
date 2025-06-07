#include "player.h"
#include "game.h"
#include "maze.h"
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


void apply_movement(GameState *gs, float dx, float dy){
    Maze *mz = gs->maze;
    // Calculate new position and check for wall collisions
    float new_x = gs->player->x + dx;
    float new_y = gs->player->y + dy;
    
    // Only update position if no wall collision
    if (!is_wall_collision(mz, new_x, new_y)) {
        gs->player->x = new_x;
        gs->player->y = new_y;
    }
}



void update_player_sprites(GameState *gs){
   //Update sprite based on facing direction
   int sprite_x = 0;
   int sprite_y = 0;

    switch (gs->player->facing) {
        case DIR_UP: 
            sprite_x = 0; 
            sprite_y = 0; //up
            break;   
        case DIR_LEFT: 
            sprite_x = 0; 
            sprite_y = 24; //left
            break;  
        case DIR_DOWN: 
            sprite_x = 0; 
            sprite_y = 48; //down
            break;  
        case DIR_RIGHT: 
            sprite_x = 0; 
            sprite_y = 24; //right
            break;  
    }

    if (gs->player->sprite) {
    cairo_surface_destroy(gs->player->sprite);
    }
    gs->player->sprite = cairo_surface_create_for_rectangle(gs->player->sprite_sheet, sprite_x, sprite_y, 24, 24);
}



