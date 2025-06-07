#ifndef PLAYER_H
#define PLAYER_H

#include <cairo.h>

typedef struct Maze Maze;
typedef struct GameState GameState;


//Enum for directions a player can face
typedef enum{
    DIR_UP = 0,
    DIR_LEFT = 1,
    DIR_DOWN = 2,
    DIR_RIGHT = 3
}FacingDirection;


typedef struct PlayerState{
    float x, y;
    FacingDirection facing;
    int lives;
    int traps_visited;
    cairo_surface_t *sprite_sheet;
    cairo_surface_t *sprite;
}PlayerState;


void spawn_player(PlayerState *player, const Maze *maze);
void apply_movement(GameState *gs, float dx, float dy);
void update_player_sprites(GameState *gs);
#endif 
