#include "maze.h"
#include "config.h"
#include "game_logic.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


/*
 * Allocate 2D arrays for current and original maze
 */
static void allocate_maze(Maze *mz){
    //Allocate memory for the size of a row
    mz->current = malloc(mz->height * sizeof *mz->current);
    mz->original = malloc(mz->height * sizeof *mz->original);
    
    //Allocate memory for each rows content amount
    for(int y = 0; y < mz->height; y++){
        mz->current[y] = malloc(mz->width * sizeof *mz->current[y]);
        mz->original[y] = malloc(mz->width * sizeof *mz->original[y]);
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
            if(mz->current[y][x] == CELL_DOOR){
                mz->door_x = x;
                mz->door_y = y;
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
        
    //Count plates and traps
    maze->plate_count = scan_plates(maze);
    maze->trap_count = scan_traps(maze);
    
    //Allocate arrays of Plate and Trap structs
    maze->plates = malloc(maze->plate_count * sizeof *maze->plates);
    maze->traps = malloc(maze->trap_count * sizeof *maze->traps);

    place_plates_and_traps(maze);
    
    return true;
}


/*
 * Restore current maze state with the copy of the original one
 */
void reset_maze(Maze *maze) {
    //Reseting traps
    for(int i = 0; i < maze->trap_count; i++){
        maze->traps[i].revealed = false;
        maze->traps[i].triggered = false;
    }
    //Reseting plates
    for(int i = 0; i < maze->trap_count; i++){
        maze->plates[i].visible = false;
        maze->plates[i].plate_pressed = false;
    }
    
    for (int y = 0; y < maze->height; y++) {
        for (int x = 0; x < maze->width; x++) {
            maze->current[y][x] = maze->original[y][x];
        }
    }
    //Seting plates and traps again
    place_plates_and_traps(maze);
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
    free(maze->original);
    maze->current = NULL;
    maze->original = NULL;
    
    
    //Free Plate struct
    free(maze->plates);
    maze->plates = NULL;
    
    
    //Free Traps struct
    free(maze->traps);
    maze->traps = NULL;
}




