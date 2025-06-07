#include "input.h"
#include <string.h>
#include "config.h"
#include "game.h"
#include "maze.h"
#include "player.h"


/*
 * Handles key press events for the game
 * 
 * - Processes keyboard input during gameplay
 * - Allows game reset with Enter key when no lives remain
 * - Updates key state in the game state structure
*/
gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    GameState *gs = (GameState *)user_data; // Refreshing current game state
    
    // No lives left and Return pressed => reset game
    if (gs->player->lives <= 0 && event->keyval == GDK_KEY_Return) {
        //reseting game and player respawn
        reset_maze(gs->maze);
        spawn_player(gs->player, gs->maze);
        
        memset(gs->pressed_keys, 0, gs->num_pressed_keys * sizeof(int));
        return TRUE;
    }

    gs->pressed_keys[event->keyval] = 1;
    return TRUE;
}


/*
 * Handles key release events for the game
 * - Marks released keys as unpressed in the game state
 * - Ignores all input when game is over (no lives left)
 * Returns: TRUE to indicate the event was handled
 */
gboolean on_key_release(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    GameState *gs = (GameState *)user_data; // Refreshing current game state
    
    if (gs->player->lives <= 0)
        return TRUE;
        
    gs->pressed_keys[event->keyval] = 0;
    return TRUE;
}



static void process_input(GameState *gs, float dt, float *dx, float *dy){
    float speed = 300.0f * dt; //  Scaling speed
    *dx = 0;
    *dy = 0;

    // Keys for movement in upper and lower case
    if (gs->pressed_keys['w'] || gs->pressed_keys['W']) {
        *dy -= speed;
        gs->player->facing = DIR_UP; // up
    }
    if (gs->pressed_keys['s'] || gs->pressed_keys['S']) {
        *dy += speed;
        gs->player->facing = DIR_DOWN; // down
    }
    if (gs->pressed_keys['a'] || gs->pressed_keys['A']) {
        *dx -= speed;
        gs->player->facing = DIR_LEFT; // left
    }
    if (gs->pressed_keys['d'] || gs->pressed_keys['D']) {
        *dx += speed;
        gs->player->facing = DIR_RIGHT; // right
    }

}



/*
This function detects:
- Playermovement
- Checks for collision
- Refreshes current game state and the lives display
*/

gboolean update_callback(GtkWidget *widget, GdkFrameClock *clock, gpointer user_data) {
    GameState *gs = (GameState *)user_data; // Getting current game state
    static gint64 prev_time = 0;
    Maze *mz = gs->maze;

    // If game over, freeze game 
    if (gs->player->lives <= 0) {
        gtk_widget_queue_draw(widget);
        return G_SOURCE_CONTINUE;
    }

    // Get current frame time
    gint64 now = gdk_frame_clock_get_frame_time(clock);
    
    // Initialize prev_time on first frame
    if (prev_time == 0) {
        prev_time = now;
        return G_SOURCE_CONTINUE;
    }
    
    
    
    // dt = delta time (time since last frame) for movement independent of framerate (smooth = good)
    float dt = (now - prev_time) / 1000000.0f;
    prev_time = now;

    float dx = 0, dy = 0;
    
    process_input(gs, dt, &dx, &dy);
    
    apply_movement(gs, dx, dy);

    handle_trap(gs);
    
    update_player_sprites(gs);
    
    // Request widget redraw
    gtk_widget_queue_draw(widget);
    return G_SOURCE_CONTINUE;
    }




