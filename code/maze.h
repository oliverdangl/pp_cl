#ifndef MAZE_H
#define MAZE_H

#include <stdbool.h>


//enum for celltype
typedef enum{
    CELL_EMPTY = 0, //field to walk on
    CELL_WALL = 1, //wall
    CELL_TRAP = 2 //trap
}CellType;


//struct holding data of current and original maze
typedef struct {
    int **current; //current maze
    int **original; //maze when loaded 
    int width; //columns
    int height; //rows
} Maze;


/*loads maze from text file
- first 2 values: widht, height
*/
bool load_maze_from_file(Maze *maze, const char *filename);

//resets the maze to the original state
void reset_maze(Maze *maze);

//frees all dynamically alocated memory
void free_maze(Maze *maze);

int is_wall_collision(Maze *mz, float x, float y);

#endif
