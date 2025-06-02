#include "draw.h"
#include "game.h" // For MAZE_OFFSET_X and MAZE_OFFSET_Y

/**
 * Draws the maze based on the current game state.
 * - Walls: dark gray rectangles with black borders
 * - Traps: red circles
 * - Empty cells: remain as background
 */
static void draw_maze(cairo_t *cr, GameState *game_state) {
    for (int y = 0; y < game_state->maze_height; y++) {
        for (int x = 0; x < game_state->maze_width; x++) {
            int cell = game_state->maze[y][x];

            int draw_x = x * CELL_SIZE + MAZE_OFFSET_X;
            int draw_y = y * CELL_SIZE + MAZE_OFFSET_Y;

            if (cell == WALL) {
                cairo_set_source_rgb(cr, 0.3, 0.3, 0.3);
                cairo_rectangle(cr, draw_x, draw_y, CELL_SIZE, CELL_SIZE);
                cairo_fill(cr);
                cairo_set_source_rgb(cr, 0, 0, 0);
                cairo_set_line_width(cr, 1);
                cairo_rectangle(cr, draw_x, draw_y, CELL_SIZE, CELL_SIZE);
                cairo_stroke(cr);
            } else if (cell == TRAP) {
                cairo_set_source_rgb(cr, 1, 0, 0);
                cairo_arc(cr, draw_x + 20, draw_y + 20, 10, 0, 2 * G_PI);
                cairo_fill(cr);
            }
        }
    }
}

/**
 * Draws the player using the loaded sprite.
 * If no sprite is loaded, nothing is drawn.
 */
static void draw_player(cairo_t *cr, const Player *player) {
    if (!player->sprite_sheet) return;

    int sprite_x = 0;
    int sprite_y = 0;

    switch (player->facing_direction) {
        case 0:
            sprite_x = 0;
            sprite_y = 0;
            break;
        case 1:
            sprite_x = 0;
            sprite_y = 72;
            break;
        case 2:
            sprite_x = 0;
            sprite_y = 48;
            break;
        default:
            sprite_x = 24;
            sprite_y = 0;
            break;
    }

    cairo_surface_t *sprite = cairo_surface_create_for_rectangle(player->sprite_sheet, sprite_x, sprite_y, 24, 24);
    cairo_set_source_surface(cr, sprite, player->x, player->y);
    cairo_paint(cr);
    cairo_surface_destroy(sprite);
}

/**
 * Displays the "GAME OVER" screen if the player has no lives left.
 * Also shows a hint to press ENTER to restart.
 */
static void draw_game_over(cairo_t *cr, GameState *game_state) {
    if (game_state->lives <= 0) {
        cairo_set_source_rgb(cr, 1, 0, 0); // red
        cairo_select_font_face(cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, 40);
        const char *text = "GAME OVER";

        cairo_text_extents_t extents;
        cairo_text_extents(cr, text, &extents);
        double x = (WINDOW_WIDTH - extents.width) / 2.0;
        double y = WINDOW_HEIGHT / 2.0;
        cairo_move_to(cr, x, y);
        cairo_show_text(cr, text);

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
 * Draws the player's remaining lives as small red bars at the top-left corner.
 */
static void draw_lives(cairo_t *cr, GameState *game_state) {
    cairo_set_source_rgb(cr, 1, 0, 0); // red
    int width = 30, height = 10, padding = 10;
    int x = padding;
    int y = 20; // top left

    cairo_select_font_face(cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 18);
    cairo_move_to(cr, x, y);
    cairo_show_text(cr, "Lives:");

    for (int i = 0; i < game_state->lives; i++) {
        cairo_rectangle(cr, x + 60 + i * (width + padding), y + 10, width, height);
        cairo_fill(cr);
    }
}

/**
 * Main rendering function.
 * Called by GTK to draw:
 * - Background
 * - Maze
 * - Player
 * - Game Over overlay (if applicable)
 * - Lives display
 */
gboolean draw_callback(GtkWidget *drawing_area, cairo_t *cr, gpointer user_data) {
    GameState *game_state = (GameState *)user_data;

    cairo_set_source_rgb(cr, 0.9, 0.9, 0.9); // light grey background
    cairo_paint(cr);

    draw_maze(cr, game_state);
    draw_player(cr, &game_state->player);
    draw_game_over(cr, game_state);
    draw_lives(cr, game_state);

    return FALSE;
}



