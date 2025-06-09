#include "player.h"
#include "game.h"
#include "maze.h"
#include "config.h"
#include "game_logic.h"
#include <cairo.h>
#include <stdio.h>
#include <stdbool.h>


/*
 * Load sprite sheet from file and initialize the first sprite frame
 */
void initiate_player_sprite(PlayerState *player, const char *sprite_path){
    player->sprite_sheet = cairo_image_surface_create_from_png(sprite_path);
    if(!player->sprite_sheet){
        fprintf(stderr, "Sprite konnte nicht geladen werden.\n");
        return;
    }
    //Start with direction "down" frame (y = 48px)
    player->sprite = cairo_surface_create_for_rectangle(player->sprite_sheet, 0, 48, 24, 24); //x, y, width, height
}


/*
 * Destroy the sprite surface and sheet
 */
void cleanup_player_sprite(PlayerState *player){
    if(player->sprite){
        cairo_surface_destroy(player->sprite);
        player->sprite = NULL;
    }
    if(player->sprite_sheet){
        cairo_surface_destroy(player->sprite_sheet);
        player->sprite_sheet = NULL;
    }
}


/* 
 * Find the first available empty cell in the maze and place the player.
 */
void spawn_player(PlayerState *player, const Maze *maze){
    bool spwn_found = false;
    player->facing = DIR_DOWN; //Initial player direction
    player->lives = MAX_LIVES; //Full lives
    player->traps_visited = 0; 
    
    //Search row by row for the first empty cell
    for(int y = 0; y < maze->height && !spwn_found; y++){
        for(int x = 0; x < maze->width; x++){
            if(maze->current[y][x] == 0){
                //Setting spawn coordinates
                player->x = MAZE_OFFSET_X + CELL_SIZE * x + CELL_SIZE / 2.0f; //Divided by 2 for centering
                player->y = MAZE_OFFSET_Y + CELL_SIZE * y + CELL_SIZE / 2.0f; //Divided by 2 for centering
                spwn_found = true;
                break;
            }
        }
    }
    if(!spwn_found){
        fprintf(stderr, "Kein Spawn möglich!");
    }
}


/*
 * Move the player by dx, dy and check for wall collision
 */
void apply_movement(GameState *gs, float dx, float dy){
    //Calculate new position
    float new_x = gs->player->x + dx;
    float new_y = gs->player->y + dy;
    
    //Only update position if no wall collision
    if (!is_wall_collision(gs->maze, new_x, new_y)) {
        gs->player->x = new_x;
        gs->player->y = new_y;
    }
}


/*
 * Update the player's current sprite image based on the facing direction
 */
void update_player_sprites(GameState *gs){
   //Initial sprite sheet coordinates
   int sprite_x = 0;
   int sprite_y = 0;

    switch (gs->player->facing) {
        case DIR_UP: 
            sprite_x = 0; 
            sprite_y = 0; 
            break;   
        case DIR_LEFT: 
            sprite_x = 0; 
            sprite_y = 24;
            break;  
        case DIR_DOWN: 
            sprite_x = 0; 
            sprite_y = 48;
            break;  
        case DIR_RIGHT: 
            sprite_x = 0; 
            sprite_y = 24;
            break;  
    }

    //Destroy previous sprite surface
    if (gs->player->sprite) {
        cairo_surface_destroy(gs->player->sprite);
    }
    //Create new sprite surface
    gs->player->sprite = cairo_surface_create_for_rectangle(gs->player->sprite_sheet, sprite_x, sprite_y, 24, 24);
}



