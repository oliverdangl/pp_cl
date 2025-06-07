#include "maze.h"
#include "game.h"
#include "player.h"
#include "config.h"
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

    //collision check with wall
    if (maze->current[top_cell][left_cell] == CELL_WALL ||
        maze->current[top_cell][right_cell] == CELL_WALL ||
        maze->current[bottom_cell][left_cell] == CELL_WALL ||
        maze->current[bottom_cell][right_cell] == CELL_WALL) {
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


