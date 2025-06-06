#include "maze.h"
#include <stdlib.h>
#include <stdio.h>




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

    if (maze->current) {
        for (int y = 0; y < maze->height; y++) {
            free(maze->current[y]);
        }
        free(maze->current);
        maze->current = NULL;
    }

    if (maze->original) {
        for (int y = 0; y < maze->height; y++) {
            free(maze->original[y]);
        }
        free(maze->original);
        maze->original = NULL;
    }
}



