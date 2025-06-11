#include "game.h"
#include "maze.h"
#include "player.h"
#include "config.h"

/*
 * Revealing traps within the calculated radius of the player
 */
void reveal_traps_near_player(GameState *gs, float reveal_distance){
    //Current player coordinates
    float current_x = gs->player->x;
    float current_y = gs->player->y;
    
    for(int i = 0; i < gs->maze->trap_count; i++){
        Trap *t = &gs->maze->traps[i];
        if(!t->revealed){
        
        //Calculate world coordinate of trap center
        float trap_x = t->trap_x * CELL_SIZE + CELL_SIZE/2;
        float trap_y = t->trap_y * CELL_SIZE + CELL_SIZE/2;
        
        //Calculating radius to the trap around the player
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
int scan_plates(const Maze *mz){
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
int scan_traps(const Maze *mz){
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
 * Helper function to check if in trap
 */
static bool in_trap(Trap *t, const int cell_x, const int cell_y){
    return (t->revealed && !t->triggered &&
             t->trap_x == cell_x && t->trap_y == cell_y);
}


/*
 * Decrement life of a player once he steps into a trap until he leaves the trap
 */
void handle_traps(GameState *gs){
    //Getting current player coordinates as integer
    int cell_x = (int)gs->player->x / CELL_SIZE;
    int cell_y = (int)gs->player->y / CELL_SIZE;

    for(int i = 0; i < gs->maze->trap_count; i++){
        Trap *t = &gs->maze->traps[i];
        if(in_trap(t, cell_x, cell_y)){
            gs->player->lives--;
            t->triggered = true;
            gs->maze->current[cell_y][cell_x] = CELL_EMPTY;
            break;
        }
    }
}


/*
 * Helper function to check if player stands on trap
 */
static bool on_plate(Plate *p, const int cell_x, const int cell_y){
    return (p->visible && !p->plate_pressed &&
                p->plate_x == cell_x && p->plate_y == cell_y);
}


/*
 * Manage stepping on preassure plates to reveal next plate or open the door
 */
void handle_plates(GameState *gs){
    //Getting player coordinates
    int cell_x = (int)gs->player->x / CELL_SIZE;
    int cell_y = (int)gs->player->y / CELL_SIZE;
    
    //Revealing plates one by one
    for(int i = 0; i < gs->maze->plate_count; i++){
        Plate *p = &gs->maze->plates[i];
        if(on_plate(p, cell_x, cell_y)){
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


/*
 *Hide all plates initially, allocate arrays to track them and then reveal the first plate
 */
void place_plates_and_traps(Maze *mz){

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
                mz->plates[p_index].visible = false;
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
    int left_cell = (int)(left_edge / CELL_SIZE);
    int right_cell = (int)(right_edge / CELL_SIZE);
    int top_cell = (int)(top_edge / CELL_SIZE);
    int bottom_cell = (int)(bottom_edge / CELL_SIZE);
    

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


