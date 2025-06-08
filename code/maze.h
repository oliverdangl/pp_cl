#ifndef MAZE_H
#define MAZE_H

#include <stdbool.h>
typedef struct GameState GameState;

//enum for celltype
typedef enum{
    CELL_EMPTY = 0, //field to walk on
    CELL_WALL = 1, //wall
    CELL_TRAP = 2, //trap
    CELL_PLATE = 3, //plate
    CELL_DOOR = 4 //door block to open
}CellType;


//struct holding data of current and original maze
typedef struct Maze{
    int **current; //current maze
    int **original; //maze when loaded 
    int width; //columns
    int height; //rows
    int plate_count;
    int *plate_x;
    int *plate_y;
    bool *plate_pressed;
    int current_plate; //current visible plate
    int door_x, door_y; //door coordinate
} Maze;


/*loads maze from text file
- first 2 values: widht, height
*/
bool load_maze_from_file(Maze *maze, const char *filename);

//resets the maze to the original state
void reset_maze(Maze *maze);

//frees all dynamically alocated memory
void free_maze(Maze *maze);

int is_wall_collision(Maze *maze, float x, float y);

void handle_trap(GameState *gs);

void handle_plates(GameState *gs);
#endif
