#include "maze.h"
#include "game.h"
#include "player.h"
#include "config.h"
#include <stdlib.h>
#include <stdio.h>

//scans original maze for plates and dorrs, returns amount of plates
static int scan_plates_and_door(Maze *mz){
    int count = 0;
    for(int y = 0; y < mz->height; y++){
        for(int x = 0; x < mz->width; x++){
            int field = mz->original[y][x];
            if(field == CELL_PLATE){
                count++;
            }else if(field == CELL_DOOR){
                mz->door_x = x;
                mz->door_y = y;
            }
        }
    }
    return count;
}

static void place_door(Maze *mz){
    mz->current[mz->door_y][mz->door_x] = CELL_DOOR;
}


/*
**function to hide preasure plates and only reveal one at a time
*/
static void place_plates(Maze *mz){
    mz->plate_x = malloc(mz->plate_count * sizeof *mz->plate_x);
    mz->plate_y = malloc(mz->plate_count * sizeof *mz->plate_y);
    mz->plate_pressed = malloc(mz->plate_count * sizeof *mz->plate_pressed);
   
    //scan in original for plates and hide current ones
    int current_field = 0;
    for(int y = 0; y < mz->height; y++){
        for(int x = 0; x < mz->width; x++){
            if(mz->original[y][x] == CELL_PLATE){
                mz->plate_x[current_field] = x;
                mz->plate_y[current_field] = y;
                mz->current[y][x] = CELL_EMPTY; //hide plate
                mz->plate_pressed[current_field] = false;
                current_field++;
            }
        }
    }
    mz->current_plate = 0;
    if(mz->plate_count > 0){
        int plate_x = mz->plate_x[0];
        int plate_y = mz->plate_y[0];
        mz->current[plate_y][plate_x] = CELL_PLATE; //reveal first plate
    }
}





//Function to read in a map from another file
bool load_maze_from_file(Maze *maze, const char *filename) {

    FILE *file = fopen(filename, "r");// opening file

    // Error message in case file could not be opened
    if(!file){
        fprintf(stderr, "Datei konnte nicht geöffnet werden: %s\n", filename);
        return false;
    }

    // Reading height and width in first line
    if(fscanf(file, "%d %d", &maze->width, &maze->height) != 2){
        fprintf(stderr, "Höhe und Breite nicht korrekt gelesen.\n");
        fclose(file);
        return false;
    }
    
        
    // Allocating memory for the maze
    maze->current = malloc(maze->height * sizeof(int *));
    for (int y = 0; y < maze->height; y++) {
        maze->current[y] = malloc(maze->width * sizeof(int));
        for (int x = 0; x < maze->width; x++) {
            if(fscanf(file, "%d", &maze->current[y][x]) != 1){;//reading in content in coordinate
                fprintf(stderr, "Werte der Koordinaten nicht einlesbar.\n");
                fclose(file);
                return false;
            }
        }
    }
    // Original maze as copie from read in maze for game reset
    maze->original = malloc(maze->height * sizeof(int *));
    for (int y = 0; y < maze->height; y++) {
        maze->original[y] = malloc(maze->width * sizeof(int));
        for (int x = 0; x < maze->width; x++) {
            maze->original[y][x] = maze->current[y][x];
        }
    }
    fclose(file);
    
    maze->plate_count = scan_plates_and_door(maze);
    place_plates(maze);
    place_door(maze);
    
    return true;
}


/*
Function to reset a game by reseting player location and redrawing the maze
*/
void reset_maze(Maze *maze) {
  
   //copying original map in current one
    for (int y = 0; y < maze->height; y++) {
        for (int x = 0; x < maze->width; x++) {
            maze->current[y][x] = maze->original[y][x];
        }
    }
}


/*
Release memory allocated for the maze
*/
void free_maze(Maze *maze) {
    if (!maze->current) return; // No maze existing

    
    for(int y = 0; y < maze->height; y++) {
            free(maze->current[y]);            
            free(maze->original[y]);
            
        }
    free(maze->current);
    maze->current = NULL;
    free(maze->original);
    maze->original = NULL;
    free(maze->plate_x);
    free(maze->plate_y);
    free(maze->plate_pressed);
}

/**
 * Checks for collision between player's hitbox and walls in the maze
 */
int is_wall_collision(Maze *maze, float x, float y) {
    const float hitbox_half = 8.0f; // 16x16 Hitbox, also Radius = 8

    //center of the player
    float left = x - hitbox_half;
    float right = x + hitbox_half;
    float top = y - hitbox_half;
    float bottom = y + hitbox_half;

    //calculating maze cells
    int left_cell = (int)((left - MAZE_OFFSET_X) / CELL_SIZE);
    int right_cell = (int)((right - MAZE_OFFSET_X) / CELL_SIZE);
    int top_cell = (int)((top - MAZE_OFFSET_Y) / CELL_SIZE);
    int bottom_cell = (int)((bottom - MAZE_OFFSET_Y) / CELL_SIZE);
    

    //checking for maze borders
    if (left_cell < 0 || right_cell >= maze->width ||
        top_cell < 0 || bottom_cell >= maze->height) {
        return 1;
    }
    
    int top_left = maze->current[top_cell][left_cell];
    int top_right = maze->current[top_cell][right_cell];
    int bottom_left = maze->current[bottom_cell][left_cell];
    int bottom_right = maze->current[bottom_cell][right_cell];
    
    //collision check with wall
    if (top_left == CELL_WALL || top_left == CELL_DOOR ||
        top_right == CELL_WALL ||top_right == CELL_DOOR ||
        bottom_left == CELL_WALL || bottom_left == CELL_DOOR ||
        bottom_right == CELL_WALL || bottom_right == CELL_DOOR){
        return 1;
    }

    return 0;
}


void handle_trap(GameState *gs){
    // Check if player is standing on a trap
    int cell_x = (int)(gs->player->x - MAZE_OFFSET_X) / CELL_SIZE;
    int cell_y = (int)(gs->player->y - MAZE_OFFSET_Y) / CELL_SIZE;
    bool in_trap = gs->maze->current[cell_y][cell_x] == CELL_TRAP;

    // Trap handling logic:
    if (in_trap && !gs->player->traps_visited) {
        gs->player->lives--;
        gs->player->traps_visited = 1;
        gs->maze->current[cell_y][cell_x] = CELL_EMPTY;
    } else if (!in_trap) {
        gs->player->traps_visited = 0;
    }
}


void handle_plates(GameState *gs){
    //check if player stands on preasure plate
    int cell_x = (int)(gs->player->x - MAZE_OFFSET_X) / CELL_SIZE;
    int cell_y = (int)(gs->player->y - MAZE_OFFSET_Y) / CELL_SIZE;
    
    //player stepping on new preasure plate
    if(gs->maze->current[cell_y][cell_x] == CELL_PLATE && gs->player->plate_visited == false){
        int current_field = gs->maze->current_plate;
        
        //marking plate as pressed
        gs->maze->plate_pressed[current_field] = true;
        gs->maze->current[cell_y][cell_x] = CELL_EMPTY;
        gs->player->plate_visited = true;
        
        //revealing next plate
        gs->maze->current_plate++;
        if(gs->maze->current_plate < gs->maze->plate_count){
            int next_x = gs->maze->plate_x[gs->maze->current_plate];
            int next_y = gs->maze->plate_y[gs->maze->current_plate];
            gs->maze->current[next_y][next_x] = CELL_PLATE;
            //all plates pressed
        } else {
            gs->maze->current[gs->maze->door_y][gs->maze->door_x] = CELL_EMPTY;
        }
    }
    //retrigger when stepping on again
    else if(gs->maze->current[cell_y][cell_x] != CELL_PLATE){
        gs->player->plate_visited = false;
    }
}


