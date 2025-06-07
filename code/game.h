#ifndef GAME_H
#define GAME_H

typedef struct Maze Maze;
typedef struct PlayerState PlayerState;

typedef struct GameState{
    Maze *maze;
    PlayerState *player;
    int *pressed_keys;
    unsigned int num_pressed_keys;
}GameState;


#endif

