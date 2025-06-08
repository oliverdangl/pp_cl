#include "maze.h"
#include "game.h"
#include "player.h"
#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


void reveal_traps_near_player(GameState *gs, float reveal_distance){
    float current_x = gs->player->x;
    float current_y = gs->player->y;
    
    for(int i = 0; i < gs->maze->trap_count; i++){
        Trap *t = &gs->maze->traps[i];
        if(!t->revealed){
        
        float trap_x = t->trap_x * CELL_SIZE + MAZE_OFFSET_X + CELL_SIZE/2;
        float trap_y = t->trap_y * CELL_SIZE + MAZE_OFFSET_Y + CELL_SIZE/2;
        
        float dist_x = current_x - trap_x;
        float dist_y = current_y - trap_y;
        float dist2 = dist_x * dist_x + dist_y * dist_y;
        if(dist2 <= reveal_distance * reveal_distance){
            //reveal
            t->revealed = true;
            gs->maze->current[t->trap_y][t->trap_x] = CELL_TRAP;
        }
        }
    }
}



/*
 * Scan the original maze for preasure plates
 */
static int scan_plates(const Maze *mz){
    int count = 0;
    for(int y = 0; y < mz->height; y++){
        for(int x = 0; x < mz->width; x++){
            int current_field = mz->original[y][x];
            if(current_field == CELL_PLATE){
                count++;
            }
        }
    }
    return count;
}

/*
 * Scan the original maze for traps
 */
static int scan_traps(const Maze *mz){
    int count = 0;
    for(int y = 0; y < mz->height; y++){
        for(int x = 0; x < mz->width; x++){
            int current_field = mz->original[y][x];
            if(current_field == CELL_TRAP){
                count++;
            }
        }
    }
    return count;
}



/*
 *Hide all plates initially, allocate arrays to track them and then reveal the first plate
 */
static void place_plates_and_traps(Maze *mz){

    //Scan in original maze for plates and hide current ones
    int p_index = 0;
    int t_index = 0;
    for(int y = 0; y < mz->height; y++){
        for(int x = 0; x < mz->width; x++){
            if(mz->original[y][x] == CELL_PLATE){
                mz->plates[p_index].plate_x = x;
                mz->plates[p_index].plate_y = y;
                mz->current[y][x] = CELL_EMPTY; //Hide plate
                mz->plates[p_index].plate_pressed = false;
                p_index++;
            }
            else if(mz->original[y][x] == CELL_TRAP){
                mz->traps[t_index].trap_x = x;
                mz->traps[t_index].trap_y = y;
                mz->traps[t_index].triggered = false;
                mz->traps[t_index].revealed = false;
                mz->current[y][x] = CELL_EMPTY;
                t_index++;
            }
        }
    }
    //Revealing first plate again
    mz->plates[0].visible = true;
    int plate_x = mz->plates[0].plate_x;
    int plate_y = mz->plates[0].plate_y;
    mz->current[plate_y][plate_x] = CELL_PLATE;
}





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

    for(int i = 0; i < gs->maze->trap_count; i++){
        Trap *t = &gs->maze->traps[i];
        if(t->revealed && !t->triggered && t->trap_x == cell_x && t->trap_y == cell_y){
            gs->player->lives--;
            t->triggered = true;
            gs->maze->current[cell_y][cell_x] = CELL_EMPTY;
            break;
        }
    }
}


/*
 * Manage stepping on preassure plates to reveal next plate or open the door
 */
void handle_plates(GameState *gs){
    //Getting player coordinates
    int cell_x = (int)(gs->player->x - MAZE_OFFSET_X) / CELL_SIZE;
    int cell_y = (int)(gs->player->y - MAZE_OFFSET_Y) / CELL_SIZE;
    
    //Revealing plates one by one
    for(int i = 0; i < gs->maze->plate_count; i++){
        Plate *p = &gs->maze->plates[i];
        if(p->visible && !p->plate_pressed && p->plate_x == cell_x && p->plate_y == cell_y){ //Plate visible, not pressed and right coordinates
            p->plate_pressed = true;
            gs->maze->current[cell_y][cell_x] = CELL_EMPTY;
            //Revealing next plate
            if(i+1 < gs->maze->plate_count){
                gs->maze->plates[i+1].visible = true;
                int next_x = gs->maze->plates[i+1].plate_x;
                int next_y = gs->maze->plates[i+1].plate_y;
                gs->maze->current[next_y][next_x] = CELL_PLATE;
            }
            else{
            //Open the door
            gs->maze->current[gs->maze->door_y][gs->maze->door_x] = CELL_EMPTY;
            }
            break;
        }
    }
    //Reset when player moved on
    if(gs->maze->current[cell_y][cell_x] != CELL_PLATE){
        gs->player->plate_visited = false;
    }
}


