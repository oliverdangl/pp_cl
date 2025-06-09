#ifndef MAZE_H
#define MAZE_H

#include <stdbool.h>


//Enum for celltype
typedef enum{
    CELL_EMPTY = 0, 
    CELL_WALL = 1, 
    CELL_TRAP = 2, 
    CELL_PLATE = 3, 
    CELL_DOOR = 4 
}CellType;

//Struct for plates
typedef struct Plate{
    int plate_x; 
    int plate_y;   
    bool plate_pressed; 
    int current_plate; //Current visible plate
    bool visible;
} Plate;

//Struct for traps
typedef struct Trap{
    int trap_x;
    int trap_y;    
    bool triggered;
    bool revealed;
} Trap;


/*
 * Holds original layout for resets as well as mutable current state.
 * Also includes special elements like plates and door coordinates.
 */
typedef struct Maze{
    int **current; //Current maze
    int **original; //Copy of loaded maze
    int width; 
    int height; 
    Plate *plates;
    int plate_count;
    Trap *traps;
    int trap_count;
    int door_x, door_y;
} Maze;


//Loads map from text file
bool load_maze_from_file(Maze *maze, const char *filename);

//Resets the maze to the original state
void reset_maze(Maze *maze);

//Free all dynamically alocated memory
void free_maze(Maze *maze);


#endif
