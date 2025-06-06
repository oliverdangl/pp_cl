#include "game.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>


//function to set spawn point on first found free field (field = 0)
static void spawn_player(GameState *gs){
    bool spwn_found = false;
    gs->player.facing_direction = 2; //startdirection: 2 = bottom(S)
    for(int y = 0; y < gs->maze_height && !spwn_found; y++){
        for(int x = 0; x < gs->maze_width; x++){
            if(gs->maze[y][x] == 0){
                //spawn location
                gs->player.x = MAZE_OFFSET_X + CELL_SIZE * x + CELL_SIZE / 2.0f;
                gs->player.y = MAZE_OFFSET_Y + CELL_SIZE * y + CELL_SIZE / 2.0f;
                gs->lives = MAX_LIVES;
                gs->trap_visited = 0;
                spwn_found = true;
                break;
            }
        }
    }
    if(!spwn_found){
        fprintf(stderr, "Kein Spawn möglich!");
    }
}


//Function to read in a map from another file
bool load_maze_from_file(GameState *game_state, const char *filename) {

    FILE *file = fopen(filename, "r");// opening file

    // Error message in case file could not be opened
    if(!file){
        fprintf(stderr, "Datei konnte nicht geöffnet werden: %s\n", filename);
        return false;
    }

    // Reading height and width in first line
    if(fscanf(file, "%d %d", &game_state->maze_width, &game_state->maze_height) != 2){
        fprintf(stderr, "Höhe und Breite nicht korrekt gelesen.\n");
        fclose(file);
        return false;
    }
    
        
    // Allocating memory for the maze
    game_state->maze = malloc(game_state->maze_height * sizeof(int *));
    for (int y = 0; y < game_state->maze_height; y++) {
        game_state->maze[y] = malloc(game_state->maze_width * sizeof(int));
        for (int x = 0; x < game_state->maze_width; x++) {
            if(fscanf(file, "%d", &game_state->maze[y][x]) != 1){;//reading in content in coordinate
                fprintf(stderr, "Werte der Koordinaten nicht einlesbar.\n");
                fclose(file);
                return false;
            }
        }
    }

    // Original maze as copie from read in maze for game reset
    game_state->original_maze = malloc(game_state->maze_height * sizeof(int *));
    for (int y = 0; y < game_state->maze_height; y++) {
        game_state->original_maze[y] = malloc(game_state->maze_width * sizeof(int));
        for (int x = 0; x < game_state->maze_width; x++) {
            game_state->original_maze[y][x] = game_state->maze[y][x];
        }
    }
    
    fclose(file);
    spawn_player(game_state);
    return true;
}


/*
Function to reset a game by reseting player location and redrawing the maze
*/
void reset_game(GameState *game_state) {
  
   //copying original map in current one
    for (int y = 0; y < game_state->maze_height; y++) {
        for (int x = 0; x < game_state->maze_width; x++) {
            game_state->maze[y][x] = game_state->original_maze[y][x];
        }
    }
    spawn_player(game_state);
}


/*
Release memory allocated for the maze
*/
void free_maze(GameState *game_state) {
    if (!game_state->maze) return; // No maze existing

    if (game_state->maze) {
        for (int y = 0; y < game_state->maze_height; y++) {
            free(game_state->maze[y]);
        }
        free(game_state->maze);
        game_state->maze = NULL;
    }

    if (game_state->original_maze) {
        for (int y = 0; y < game_state->maze_height; y++) {
            free(game_state->original_maze[y]);
        }
        free(game_state->original_maze);
        game_state->original_maze = NULL;
    }
}


