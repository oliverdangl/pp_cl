#include "draw.h"

static void draw_maze(cairo_t *cr, GameState *game_state) {
    for (int y = 0; y < MAZE_HEIGHT; y++) {
        for (int x = 0; x < MAZE_WIDTH; x++) {
            int cell = game_state->maze[y][x];

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

 void draw_player(cairo_t *cr, const Player *player) {
    float offset = (32 - PLAYER_HITBOX_SIZE) / 2.0f;
    cairo_set_source_rgb(cr, 0, 0.7, 0);
    cairo_rectangle(cr, player->x - offset, player->y - offset, 32, 32);
    cairo_fill(cr);
}

static void draw_game_over(cairo_t *cr, GameState *game_state){
    if (game_state->lives <= 0) {
        cairo_set_source_rgb(cr, 1, 0, 0); //rot
        cairo_select_font_face(cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, 40);
        const char *text = "GAME OVER";
        cairo_text_extents_t extents;
        cairo_text_extents(cr, text, &extents);
        double x = (WINDOW_WIDTH - extents.width) / 2.0;
        double y = WINDOW_HEIGHT / 2.0;
        cairo_move_to(cr, x, y);
        cairo_show_text(cr, text);
        return;
    }
}


static void draw_lives(cairo_t *cr, GameState *game_state) {
    

    cairo_set_source_rgb(cr, 0, 0.7, 0);
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
void draw_visibility(cairo_t *cr, int px, int py, int tile_size, int vision_radius, int map_width, int map_height) {
    cairo_set_source_rgb(cr, 0, 0, 0);  // Schwarz

    for (int y = 0; y < map_height; y++) {
        for (int x = 0; x < map_width; x++) {
            int dx = x - px;
            int dy = y - py;
            if (abs(dx) > vision_radius || abs(dy) > vision_radius) {
                cairo_rectangle(cr, x * tile_size, y * tile_size, tile_size, tile_size);
            }
        }
    }

    cairo_fill(cr);
}
*/



gboolean draw_callback(GtkWidget *drawing_area, cairo_t *cr, gpointer user_data) {
    GameState *game_state = (GameState *)user_data;

    cairo_set_source_rgb(cr, 0.9, 0.9, 0.9);
    cairo_paint(cr);

    draw_maze(cr, game_state);
    draw_player(cr, &game_state->player);

    draw_game_over(cr, game_state);
   
    draw_lives(cr, game_state);

    // Sichtkegel auf Basis der Zellposition
    int player_cell_x = game_state->player.x / CELL_SIZE;
    int player_cell_y = game_state->player.y / CELL_SIZE;
    //draw_visibility(cr, player_cell_x, player_cell_y, CELL_SIZE, 2, MAZE_WIDTH, MAZE_HEIGHT);

    return FALSE;
}


