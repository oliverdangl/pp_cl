#ifndef MAZE_H
#define MAZE_H

#include <stdbool.h>
typedef struct GameState GameState;

//Enum for celltype
typedef enum{
    CELL_EMPTY = 0, 
    CELL_WALL = 1, 
    CELL_TRAP = 2, 
    CELL_PLATE = 3, 
    CELL_DOOR = 4 
}CellType;


/*
 * Holds original layout for resets as well as mutable current state.
 * Also includes special elements like plates and door coordinates.
 */
typedef struct Maze{
    int **current; //Current maze
    int **original; //Copy of loaded maze
    int width; 
    int height; 
    int plate_count; //Amount of plates
    int *plate_x; 
    int *plate_y;
    bool *plate_pressed; 
    int current_plate; //Current visible plate
    int door_x, door_y;
} Maze;


//Loads map from text file
bool load_maze_from_file(Maze *maze, const char *filename);

//Resets the maze to the original state
void reset_maze(Maze *maze);

//Free all dynamically alocated memory
void free_maze(Maze *maze);

//Checks for collision with walls or door
int is_wall_collision(Maze *maze, float x, float y);

//Handle trap activation and live decrement
void handle_trap(GameState *gs);

//Handle preassure plate steppinng and door opening
void handle_plates(GameState *gs);

#endif
