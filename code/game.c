#include "game.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>


//Function to read in a map from another file
bool load_maze_from_file(GameState *game_state, const char *filename) {
    FILE *file = fopen(filename, "r");// opening file

    // Error message in case file could not be opened
    if(!file){
        fprintf(stderr, "Datei konnte nicht geöffnet werden: %s\n", filename);
        return false;
    }

    // Reading height and width in first line
    if (fscanf(file, "%d %d", &game_state->maze_width, &game_state->maze_height) != 2) {
        fprintf(stderr, "Fehler beim Einlesen der Maze-Dimensionen\n");
        fclose(file);
        return false;
    }

    // Allocating memory for the maze
    game_state->maze = malloc(game_state->maze_height * sizeof(int *));
    for (int y = 0; y < game_state->maze_height; y++) {
        game_state->maze[y] = malloc(game_state->maze_width * sizeof(int));
        for (int x = 0; x < game_state->maze_width; x++) {
            //Reading content of the maze
            if (fscanf(file, "%d", &game_state->maze[y][x]) != 1) {
                fprintf(stderr, "Fehler beim Einlesen der Maze-Daten bei (%d,%d)\n", y, x);
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

    //Playerspawn location [1][1]
    game_state->player.x = MAZE_OFFSET_X + CELL_SIZE * 1 + CELL_SIZE / 2.0f;
    game_state->player.y = MAZE_OFFSET_Y + CELL_SIZE * 1 + CELL_SIZE / 2.0f;

    game_state->lives = MAX_LIVES;
    game_state->trap_visited = 0;

    fclose(file);
    return true;
}


/*
Function to reset a game by reseting player location and redrawing the maze
*/
void reset_game(GameState *game_state) {
    game_state->lives = MAX_LIVES;
    game_state->trap_visited = 0;

    //Player respawn location [1][1]
    game_state->player.x = MAZE_OFFSET_X + CELL_SIZE * 1 + CELL_SIZE / 2.0f;
    game_state->player.y = MAZE_OFFSET_Y + CELL_SIZE * 1 + CELL_SIZE / 2.0f;

    for (int y = 0; y < game_state->maze_height; y++) {
        for (int x = 0; x < game_state->maze_width; x++) {
            game_state->maze[y][x] = game_state->original_maze[y][x]; // Loading original content of the maze in current maze
        }
    }
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


