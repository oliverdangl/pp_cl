#include "maze.h"
#include "game.h"
#include "player.h"
#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/*
 * Scan the original maze for preasure plates and the exit door
 */
static int scan_plates_and_door(Maze *mz){
    int count = 0;
    for(int y = 0; y < mz->height; y++){
        for(int x = 0; x < mz->width; x++){
            int current_field = mz->original[y][x];
            if(current_field == CELL_PLATE){
                count++;
            }
            //Storing door coordinates
            else if(current_field == CELL_DOOR){
                mz->door_x = x;
                mz->door_y = y;
            }
        }
    }
    return count;
}


/*
 *Hide all plates initially, allocate arrays to track them and then reveal the first plate
 */
static void place_plates(Maze *mz){
    //Allocate memory for coordinates and state of the plates
    mz->plate_x = malloc(mz->plate_count * sizeof *mz->plate_x);
    mz->plate_y = malloc(mz->plate_count * sizeof *mz->plate_y);
    mz->plate_pressed = malloc(mz->plate_count * sizeof *mz->plate_pressed);
   
    //Scan in original maze for plates and hide current ones
    int plate_index = 0;
    for(int y = 0; y < mz->height; y++){
        for(int x = 0; x < mz->width; x++){
            if(mz->original[y][x] == CELL_PLATE){
                mz->plate_x[plate_index] = x;
                mz->plate_y[plate_index] = y;
                mz->current[y][x] = CELL_EMPTY; //Hide plate
                mz->plate_pressed[plate_index] = false;
                plate_index++;
            }
        }
    }
    //Revealing first plate again
    mz->current_plate = 0;
    int first_plate_x = mz->plate_x[0];
    int first_plate_y = mz->plate_y[0];
    mz->current[first_plate_y][first_plate_x] = CELL_PLATE;
}


/*
 * Allocate 2D arrays for current and original maze
 */
static void allocate_maze(Maze *mz){
    //Allocate memory for the size of a row
    mz->current = malloc(mz->height * sizeof(int *));
    mz->original = malloc(mz->height * sizeof(int *));
    
    //Allocate memory for each rows content amount
    for(int y = 0; y < mz->height; y++){
        mz->current[y] = malloc(mz->width * sizeof(int));
        mz->original[y] = malloc(mz->width * sizeof(int));
    }
}


/*
 * Copy content of current maze in the original one
 */
static void copy_maze(Maze *mz){
    for(int y = 0; y < mz->height; y++){
        for(int x = 0; x < mz->width; x++){
            mz->original[y][x] = mz->current[y][x];
        }
    }
}


/* 
 * Reding in the content of the maze file 
 */
static bool read_content_of_maze(Maze *mz, FILE *file){
    for(int y = 0; y < mz->height; y++){
        for(int x = 0; x < mz->width; x++){
            if(fscanf(file, "%d", &mz->current[y][x]) != 1){
                fprintf(stderr, "Werte der Koordinate nicht einlesbar.\n");
                return false;
            }
        }
    }
    return true;
}


/*
 * Read maze dimensions and layout from text file, initialize structures as well as plates and doors
 */
bool load_maze_from_file(Maze *maze, const char *filename) {

    FILE *file = fopen(filename, "r");//Opening file

    //Error message in case file could not be opened
    if(!file){
        fprintf(stderr, "Datei konnte nicht geöffnet werden: %s\n", filename);
        return false;
    }

    //Reading height and width in first line
    if(fscanf(file, "%d %d", &maze->width, &maze->height) != 2){
        fprintf(stderr, "Höhe und Breite nicht korrekt gelesen.\n");
        fclose(file);
        return false;
    }
    
    allocate_maze(maze);
    if(!read_content_of_maze(maze, file)){
        fclose(file);
    }
    fclose(file);
    copy_maze(maze);

    maze->plate_count = scan_plates_and_door(maze);
    place_plates(maze);
    
    return true;
}


/*
 * Restore current maze state with the copy of the original one
 */
void reset_maze(Maze *maze) {
    for (int y = 0; y < maze->height; y++) {
        for (int x = 0; x < maze->width; x++) {
            maze->current[y][x] = maze->original[y][x];
        }
    }
}


/*
 * Free allocated memory associated with the maze
 */
void free_maze(Maze *maze) {
    if (!maze->current) return; //No maze existing

    //Releasing memory of the content 
    for(int y = 0; y < maze->height; y++) {
        free(maze->current[y]);            
        free(maze->original[y]);
            
    }
    //Releasing memory of the structures
    free(maze->current);
    maze->current = NULL;
    free(maze->original);
    maze->original = NULL;
    free(maze->plate_x);
    free(maze->plate_y);
    free(maze->plate_pressed);
}


/*
 * Check collision between player hitbox and maze walls or door
 */
int is_wall_collision(Maze *maze, float x, float y) {
    const float hitbox_half = 8.0f; //For calculating space to the border of a cell

    //Compute 4 edges of the hitbox in world coordinates
    float left_edge = x - hitbox_half;
    float right_edge = x + hitbox_half;
    float top_edge = y - hitbox_half;
    float bottom_edge = y + hitbox_half;

    //Converting world coordinates into maze cell indices
    int left_cell = (int)((left_edge - MAZE_OFFSET_X) / CELL_SIZE);
    int right_cell = (int)((right_edge - MAZE_OFFSET_X) / CELL_SIZE);
    int top_cell = (int)((top_edge - MAZE_OFFSET_Y) / CELL_SIZE);
    int bottom_cell = (int)((bottom_edge - MAZE_OFFSET_Y) / CELL_SIZE);
    

    //In case reaching outside maze boundaries
    if (left_cell < 0 || right_cell >= maze->width ||
        top_cell < 0 || bottom_cell >= maze->height) {
        return 1;
    }
    
    //Look up the content of the four corner cells
    int top_left = maze->current[top_cell][left_cell];
    int top_right = maze->current[top_cell][right_cell];
    int bottom_left = maze->current[bottom_cell][left_cell];
    int bottom_right = maze->current[bottom_cell][right_cell];
    
    //If the corner cell is a wall or door we have a collision
    if (top_left == CELL_WALL || top_left == CELL_DOOR ||
        top_right == CELL_WALL ||top_right == CELL_DOOR ||
        bottom_left == CELL_WALL || bottom_left == CELL_DOOR ||
        bottom_right == CELL_WALL || bottom_right == CELL_DOOR){
        return 1;
    }

    return 0;
}


/*
 * Decrement life of a player once he steps into a trap until he leaves the trap
 */
void handle_trap(GameState *gs){
    //Getting current player coordinates
    int cell_x = (int)(gs->player->x - MAZE_OFFSET_X) / CELL_SIZE;
    int cell_y = (int)(gs->player->y - MAZE_OFFSET_Y) / CELL_SIZE;
    bool in_trap = gs->maze->current[cell_y][cell_x] == CELL_TRAP;

    //Reducing lives in case player stands on trap and clear field
    if (in_trap && !gs->player->traps_visited) {
        gs->player->lives--;
        gs->player->traps_visited = true;
        gs->maze->current[cell_y][cell_x] = CELL_EMPTY;
    } else if (!in_trap) {
        gs->player->traps_visited = false;
    }
}


/*
 * Manage stepping on preassure plates to reveal next plate or open the door
 */
void handle_plates(GameState *gs){
    //Getting player coordinates
    int cell_x = (int)(gs->player->x - MAZE_OFFSET_X) / CELL_SIZE;
    int cell_y = (int)(gs->player->y - MAZE_OFFSET_Y) / CELL_SIZE;
    
    //Player stepping on new preassure plate
    if(gs->maze->current[cell_y][cell_x] == CELL_PLATE && gs->player->plate_visited == false){
        int current_plate = gs->maze->current_plate; //Initial plate 0
        
        //Marking plate as pressed
        gs->maze->plate_pressed[current_plate] = true;
        gs->maze->current[cell_y][cell_x] = CELL_EMPTY;
        gs->player->plate_visited = true;
        
        //Revealing next plate on map in case there is another one
        gs->maze->current_plate++;
        if(gs->maze->current_plate < gs->maze->plate_count){
            int next_x = gs->maze->plate_x[gs->maze->current_plate];
            int next_y = gs->maze->plate_y[gs->maze->current_plate];
            gs->maze->current[next_y][next_x] = CELL_PLATE; //Revealing next plate
        } else {
            gs->maze->current[gs->maze->door_y][gs->maze->door_x] = CELL_EMPTY;
        }
    }
    //Reset when player moved on
    else if(gs->maze->current[cell_y][cell_x] != CELL_PLATE){
        gs->player->plate_visited = false;
    }
}


