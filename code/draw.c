#include "draw.h"

/**
 * Draws the maze structure based on the game state
 * 
 * This function renders each cell of the maze according to its type:
 * - Walls: Dark gray filled rectangles with black borders
 * - Traps: Red circles
 * - Empty cells: Left as background
 */
static void draw_maze(cairo_t *cr, GameState *game_state) {
    for (int y = 0; y < MAZE_HEIGHT; y++) {
        for (int x = 0; x < MAZE_WIDTH; x++) {
            int cell = game_state->maze[y][x]; // Storing current field type

            if (cell == WALL) {
                cairo_set_source_rgb(cr, 0.3, 0.3, 0.3);
                cairo_rectangle(cr, x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE);
                cairo_fill(cr);
                cairo_set_source_rgb(cr, 0, 0, 0);
                cairo_set_line_width(cr, 1);
                cairo_rectangle(cr, x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE);
                cairo_stroke(cr); 
            } else if (cell == TRAP) {
                cairo_set_source_rgb(cr, 1, 0, 0);
                cairo_arc(cr, x * CELL_SIZE + 20, y * CELL_SIZE + 20, 10, 0, 2 * G_PI);
                cairo_fill(cr);
            }
        }
    }
}

/*
 * This function renders the player as a green rectangle centered on their position,
 * with visual representation larger than the actual hitbox for better visibility.
 * In future: use sprite
 */
void draw_player(cairo_t *cr, const Player *player) {
    float offset = (32 - PLAYER_HITBOX_SIZE) / 2.0f;
    cairo_set_source_rgb(cr, 0, 0.7, 0); // color green
    cairo_rectangle(cr, player->x - offset, player->y - offset, 32, 32);
    cairo_fill(cr);
}

/**
 * Draws the "GAME OVER" message when the player has no lives remaining
 * Includes hint to press Enter to restart
 */
static void draw_game_over(cairo_t *cr, GameState *game_state){
    if (game_state->lives <= 0) {
        cairo_set_source_rgb(cr, 1, 0, 0); // red
        cairo_select_font_face(cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, 40);
        const char *text = "GAME OVER";

        // Dimensions for text centering
        cairo_text_extents_t extents;
        cairo_text_extents(cr, text, &extents);

        double x = (WINDOW_WIDTH - extents.width) / 2.0;
        double y = WINDOW_HEIGHT / 2.0;
        cairo_move_to(cr, x, y);
        cairo_show_text(cr, text);

        // Show hint to restart
        cairo_set_font_size(cr, 24);
        const char *hint = "Press ENTER to Restart";
        cairo_text_extents(cr, hint, &extents);
        double hint_x = (WINDOW_WIDTH - extents.width) / 2.0;
        double hint_y = y + 40;
        cairo_move_to(cr, hint_x, hint_y);
        cairo_show_text(cr, hint);
    }
}

/**
 * Draws the player's remaining lives indicator
 */
static void draw_lives(cairo_t *cr, GameState *game_state) {
    cairo_set_source_rgb(cr, 1, 0, 0); // red
    int width = 30, height = 10, padding = 10, x = padding, y = WINDOW_HEIGHT - 50;
    cairo_select_font_face(cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 18);
    cairo_move_to(cr, x, y);
    cairo_show_text(cr, "Lives:");

    for (int i = 0; i < game_state->lives; i++) {
        cairo_rectangle(cr, x + 60 + i * (width + padding), y + 10, width, height);
        cairo_fill(cr);
    }
}

/*
 * Function that gets called to draw:
 * - Background
 * - Maze
 * - Player
 * - Overlay (Game Over Text)
 * - Healthbar
 */
gboolean draw_callback(GtkWidget *drawing_area, cairo_t *cr, gpointer user_data) {
    GameState *game_state = (GameState *)user_data;

    // Drawing light grey background
    cairo_set_source_rgb(cr, 0.9, 0.9, 0.9);
    cairo_paint(cr);

    // Drawing maze and player
    draw_maze(cr, game_state);
    draw_player(cr, &game_state->player); 

    draw_game_over(cr, game_state);
    draw_lives(cr, game_state);

    return FALSE;
}



