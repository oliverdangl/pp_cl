#ifndef GAME_H
#define GAME_H

//Forward declaration for GameState struct
typedef struct Maze Maze;
typedef struct PlayerState PlayerState;



typedef struct GameState{
    Maze *maze; //Pointer to maze data (current and original one)
    PlayerState *player; //Pointer to player status (position, lives)
    int *pressed_keys;
    unsigned int num_pressed_keys;
    const char **level_paths; //Array of level file paths
    int num_levels;
    int current_level;
    float elapsed_time; //Seconds remaining in current level
}GameState;


#endif

