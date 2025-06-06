#ifndef GAME_H
#define GAME_H

#include "maze.h"
#include "player.h"

typedef struct{
    Maze maze;
    PlayerState player;
    int *pressed_keys;
    unsigned int num_pressed_keys;
}GameState;


#endif

