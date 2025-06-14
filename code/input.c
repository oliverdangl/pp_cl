#include "input.h"
#include <string.h>
#include <stdlib.h>
#include "config.h"
#include "game.h"
#include "maze.h"
#include "player.h"
#include "game_logic.h"

/* 
 * Display the pause dialog and handle the user's choice
 */
static void show_pause_menu(GameState *gs, GtkWidget *parent){
    gs->paused = true;
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        "Pause", GTK_WINDOW(parent), 
        
        GTK_DIALOG_MODAL, 
        "Continue", GTK_RESPONSE_OK,
        "Restart Level", GTK_RESPONSE_REJECT,
        "Quit Game", GTK_RESPONSE_CLOSE,
        NULL
    );
    int resp = gtk_dialog_run(GTK_DIALOG(dialog));
    
    switch(resp){
        case GTK_RESPONSE_OK: //Continue
            gs->paused = false;
            break;
        case GTK_RESPONSE_REJECT: //Restart level
            reset_maze(gs->maze);
            spawn_player(gs->player, gs->maze);
            gs->elapsed_time = 0;
            gs->paused = false;
            break;
        case GTK_RESPONSE_DELETE_EVENT: //Window closed by x or Esc
            gs->paused = false;
            break; 
        default: //Quit
            exit(0);
    }
    memset(gs->pressed_keys, 0, gs->num_pressed_keys * sizeof(int));
    gtk_widget_destroy(dialog);
}


/*
 * Handles key press events during the game, including game reset when runnin out of lives
 */
gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    GameState *gs = (GameState *)user_data;
    
    //No lives left and Return pressed => reset game
    if (gs->player->lives <= 0 && event->keyval == GDK_KEY_Return) {
        reset_maze(gs->maze);
        spawn_player(gs->player, gs->maze);
        gs->elapsed_time = 0;
        
        memset(gs->pressed_keys, 0, gs->num_pressed_keys * sizeof(int)); //Releasing allocated memory for pressed keys
        return TRUE;
    }
    
    //Open pause menu on Escape
    if(event->keyval == GDK_KEY_Escape && gs->player->lives > 0){
        show_pause_menu(gs,widget);
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
    
    if (gs->player->lives <= 0 || gs->paused)
        return TRUE;
        
    gs->pressed_keys[event->keyval] = 0; //Marking key as released
    return TRUE;
}


/* 
 * Compute movement delta based on pressed currently pressed key and frame time.
 */
static void process_input(GameState *gs, float dt, float *dx, float *dy){
    float speed = 600.0f * dt; //Scaling speed for frame independent movement
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

    //If game over or pause hold game
    if (gs->player->lives <= 0 || gs->paused) {
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

    gs->elapsed_time += dt;
    

    //Distant coordinates a player has moved
    float dx, dy;
    
    process_input(gs, dt, &dx, &dy);
    apply_movement(gs, dx, dy);

    reveal_traps_near_player(gs, REVEAL_DISTANCE);
    handle_traps(gs);
    handle_plates(gs);    
    check_level_transition(gs);
    update_player_sprites(gs);
    
    //Redrawing window
    gtk_widget_queue_draw(widget);
    return G_SOURCE_CONTINUE;
}




