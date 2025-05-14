#include "input.h"
/**
 * Checks for collision between player's hitbox and walls in the maze
 */
static int is_wall_collision(GameState *game_state, float x, float y) {
    // Player hitbox dimensions (smaller than cell size for smoother movement)
    const float hitbox_size = 28.0f;
    // Offset to center the hitbox within the cell
    const float offset = (32.0f - hitbox_size) / 2.0f;

    // Calculate hitbox boundaries with slight shrinkage to prevent getting stuck on edges
    float hitbox_left = x + offset;
    float hitbox_top = y + offset;
    float hitbox_right = hitbox_left + hitbox_size - 0.01f;
    float hitbox_bottom = hitbox_top + hitbox_size - 0.01f;

    // Convert pixel coordinates to maze grid coordinates
    int left = (int)(hitbox_left / CELL_SIZE);
    int right = (int)(hitbox_right / CELL_SIZE);
    int top = (int)(hitbox_top / CELL_SIZE);
    int bottom = (int)(hitbox_bottom / CELL_SIZE);


    // Checking if outside of the maze
    if (left < 0 || right >= MAZE_WIDTH || top < 0 || bottom >= MAZE_HEIGHT){
        return 1;
        }

    // Check all four corners of the hitbox against wall cells
    return game_state->maze[top][left] == WALL || game_state->maze[top][right] == WALL ||
           game_state->maze[bottom][left] == WALL || game_state->maze[bottom][right] == WALL;
}


/*
 * Handles key press events for the game
 * 
 * - Processes keyboard input during gameplay
 * - Allows game reset with Enter key when no lives remain
 * - Updates key state in the game state structure
*/
gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    GameState *game_state = (GameState *)user_data; // Refreshing current game state
    
    // No lives left and Return pressed => reset game
    if (game_state->lives <= 0 && event->keyval == GDK_KEY_Return) {
        reset_game(game_state);
        memset(game_state->pressed_keys, 0, game_state->num_pressed_keys * sizeof(int));
        return TRUE;
    }

    game_state->pressed_keys[event->keyval] = 1;
    return TRUE;
}


/*
 * Handles key release events for the game
 * - Marks released keys as unpressed in the game state
 * - Ignores all input when game is over (no lives left)
 * Returns: TRUE to indicate the event was handled
 */
gboolean on_key_release(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    GameState *game_state = (GameState *)user_data; // Refreshing current game state
    
    if (game_state->lives <= 0)
        return TRUE;
        
        
    game_state->pressed_keys[event->keyval] = 0;
    return TRUE;
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

    // If game over, just trigger redraw and exit
    if (gs->lives <= 0) {
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

    // dt = delta time (time since last frame)
    float dt = (now - prev_time) / 1000000.0f;
    prev_time = now;


    float dx = 0, dy = 0;
    float speed = 500.0f * dt; //  Scaling speed
    
    // Keys for movement in upper and lower case
    if (gs->pressed_keys['w'] || gs->pressed_keys['W']) dy -= speed;
    if (gs->pressed_keys['s'] || gs->pressed_keys['S']) dy += speed;
    if (gs->pressed_keys['a'] || gs->pressed_keys['A']) dx -= speed;
    if (gs->pressed_keys['d'] || gs->pressed_keys['D']) dx += speed;

    // Calculate new position and check for wall collisions
    float new_x = gs->player.x + dx;
    float new_y = gs->player.y + dy;
    
    // Only update position if no wall collision
    if (!is_wall_collision(gs, new_x, new_y)) {
        gs->player.x = new_x;
        gs->player.y = new_y;
    }

    // Check if player is standing on a trap
    int cell_x = gs->player.x / CELL_SIZE;
    int cell_y = gs->player.y / CELL_SIZE;
    int in_trap = gs->maze[cell_y][cell_x] == TRAP;

     // Trap handling logic:
    if (in_trap && !gs->trap_visited) {
        gs->lives--;
        gs->trap_visited = 1;
    } else if (!in_trap) {
        gs->trap_visited = 0;
    }
    
    // Request widget redraw
    gtk_widget_queue_draw(widget);
    return G_SOURCE_CONTINUE;
}

