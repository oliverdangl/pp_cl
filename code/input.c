#include "input.h"
#include <string.h>
#include "config.h"
#include "game.h"
#include "maze.h"
#include "player.h"
#include "game_logic.h"


/*
 * Handles key press events during the game, including game reset when runnin out of lives
 */
gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    GameState *gs = (GameState *)user_data;
    
    //No lives left and Return pressed => reset game
    if (gs->player->lives <= 0 && event->keyval == GDK_KEY_Return) {
        reset_maze(gs->maze);
        spawn_player(gs->player, gs->maze);
        
        memset(gs->pressed_keys, 0, gs->num_pressed_keys * sizeof(int)); //Releasing allocated memory for pressed keys
        return TRUE;
    }
    gs->pressed_keys[event->keyval] = 1; //Marking key as pressed
    return TRUE;
}


/*
 * Handles key release events and ignore input when game over
 */
gboolean on_key_release(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    GameState *gs = (GameState *)user_data;
    
    if (gs->player->lives <= 0)
        return TRUE;
        
    gs->pressed_keys[event->keyval] = 0; //Marking key as released
    return TRUE;
}


/* 
 * Compute movement delta based on pressed currently pressed key and frame time.
 */
static void process_input(GameState *gs, float dt, float *dx, float *dy){
    float speed = 300.0f * dt; //Scaling speed for frame independent movement
    *dx = 0;
    *dy = 0;

    //Checking movement keys in upper and lower case and compute movement deltas
    if (gs->pressed_keys['w'] || gs->pressed_keys['W']) {
        *dy -= speed;
        gs->player->facing = DIR_UP; 
    }
    if (gs->pressed_keys['s'] || gs->pressed_keys['S']) {
        *dy += speed;
        gs->player->facing = DIR_DOWN; 
    }
    if (gs->pressed_keys['a'] || gs->pressed_keys['A']) {
        *dx -= speed;
        gs->player->facing = DIR_LEFT; 
    }
    if (gs->pressed_keys['d'] || gs->pressed_keys['D']) {
        *dx += speed;
        gs->player->facing = DIR_RIGHT; 
    }
}



/*
 * Called each frame to update the game state, handle collisions, traps and plates
 */
gboolean update_callback(GtkWidget *widget, GdkFrameClock *clock, gpointer user_data) {
    GameState *gs = (GameState *)user_data;
    static gint64 prev_time = 0; //Last frame for delta calculation

    //If game over, stop game
    if (gs->player->lives <= 0) {
        gtk_widget_queue_draw(widget);
        return G_SOURCE_CONTINUE;
    }

    //Get current frame time
    gint64 now = gdk_frame_clock_get_frame_time(clock);
    
    //Initialize first call
    if (prev_time == 0) {
        prev_time = now;
        return G_SOURCE_CONTINUE;
    }
    
    //Delta time in seconds for frame independent time  
    float dt = (now - prev_time) / 1000000.0f;
    prev_time = now;

    //Distant coordinates a player has moved
    float dx, dy;
    
    process_input(gs, dt, &dx, &dy);
    apply_movement(gs, dx, dy);

    reveal_traps_near_player(gs, REVEAL_DISTANCE);
    handle_traps(gs);
    handle_plates(gs);    
    update_player_sprites(gs);
    
    //Redrawing window
    gtk_widget_queue_draw(widget);
    return G_SOURCE_CONTINUE;
}




