#include "draw.h"
#include "game.h"
#include <math.h>

static void draw_maze(cairo_t *cr, GameState *game_state, double cell_width, double cell_height) {
    for (int y = 0; y < game_state->maze_height; y++) {
        for (int x = 0; x < game_state->maze_width; x++) {
            double draw_x = x * cell_width;
            double draw_y = y * cell_height;

            if (game_state->maze[y][x] == WALL) {
                // Wand zeichnen
                cairo_set_source_rgb(cr, 0.3, 0.3, 0.3);
                cairo_rectangle(cr, draw_x, draw_y, cell_width, cell_height);
                cairo_fill(cr);
                cairo_set_source_rgb(cr, 0, 0, 0);
                cairo_set_line_width(cr, 1);
                cairo_rectangle(cr, draw_x, draw_y, cell_width, cell_height);
                cairo_stroke(cr);
            } else if (game_state->maze[y][x] == TRAP) {
                // Falle zeichnen
                cairo_set_source_rgb(cr, 1, 0, 0);
                cairo_arc(cr, draw_x + cell_width/2, draw_y + cell_height/2, 
                         fmin(cell_width, cell_height)/3, 0, 2 * M_PI);
                cairo_fill(cr);
            }
        }
    }
}

static void draw_player(cairo_t *cr, const Player *player, double cell_width, double cell_height) {
    if (!player->sprite_sheet) return;

    // Spielerposition in Zellkoordinaten berechnen
    double player_x = (player->x - MAZE_OFFSET_X) / CELL_SIZE * cell_width - cell_width / 2;
    double player_y = (player->y - MAZE_OFFSET_Y) / CELL_SIZE * cell_height - cell_height / 2;

    
    // Skalierte Spielergröße
    double player_size = fmin(cell_width, cell_height) * 0.8;

    // Sprite-Ausschnitt basierend auf Blickrichtung
    int sprite_x = 0, sprite_y = 0;
    switch (player->facing_direction) {
        case 0: sprite_y = 0; break;   // oben
        case 1: sprite_y = 24; break;  // links
        case 2: sprite_y = 48; break;  // unten
        default: sprite_y = 24; break; // rechts
    }

    // Sprite skalieren und zeichnen
    cairo_save(cr);
    cairo_translate(cr, player_x + cell_width/2 - player_size/2, 
                    player_y + cell_height/2 - player_size/2);
    cairo_scale(cr, player_size/24.0, player_size/24.0);
    
    cairo_surface_t *sprite = cairo_surface_create_for_rectangle(
        player->sprite_sheet, sprite_x, sprite_y, 24, 24);
    cairo_set_source_surface(cr, sprite, 0, 0);
    cairo_paint(cr);
    cairo_surface_destroy(sprite);
    cairo_restore(cr);
}

static void draw_game_over(cairo_t *cr, int width, int height) {
    cairo_set_source_rgb(cr, 1, 0, 0);
    cairo_select_font_face(cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    
    // "GAME OVER" Text
    cairo_set_font_size(cr, 40);
    const char *text = "GAME OVER";
    cairo_text_extents_t extents;
    cairo_text_extents(cr, text, &extents);
    cairo_move_to(cr, (width - extents.width)/2, height/2);
    cairo_show_text(cr, text);

    // Hinweis Text
    cairo_set_font_size(cr, 24);
    const char *hint = "Press ENTER to Restart";
    cairo_text_extents(cr, hint, &extents);
    cairo_move_to(cr, (width - extents.width)/2, height/2 + 40);
    cairo_show_text(cr, hint);
}

static void draw_lives(cairo_t *cr, int lives, double width, double height) {
    double life_width = width * 0.05;
    double life_height = height * 0.02;
    double padding = width * 0.01;
    double start_x = width * 0.02;
    double start_y = height * 0.05;

    cairo_set_source_rgb(cr, 1, 0, 0);
    cairo_select_font_face(cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, height * 0.03);
    cairo_move_to(cr, start_x, start_y);
    cairo_show_text(cr, "Lives:");

    for (int i = 0; i < lives; i++) {
        cairo_rectangle(cr, 
            start_x + (life_width + padding) * (i + 3), 
            start_y - life_height, 
            life_width, 
            life_height);
        cairo_fill(cr);
    }
}

gboolean draw_callback(GtkWidget *drawing_area, cairo_t *cr, gpointer user_data) {
    GameState *game_state = (GameState *)user_data;

    // Fenstergröße ermitteln
    GtkAllocation alloc;
    gtk_widget_get_allocation(drawing_area, &alloc);
    int width = alloc.width;
    int height = alloc.height;

    // Hintergrund
    cairo_set_source_rgb(cr, 0.9, 0.9, 0.9);
    cairo_paint(cr);

    // Zellgröße berechnen
    double cell_width = (double)width / game_state->maze_width;
    double cell_height = (double)height / game_state->maze_height;

    // Labyrinth zeichnen
    draw_maze(cr, game_state, cell_width, cell_height);

    // Spieler zeichnen
    draw_player(cr, &game_state->player, cell_width, cell_height);

    // Leben anzeigen
    draw_lives(cr, game_state->lives, width, height);

    // Game Over anzeigen falls nötig
    if (game_state->lives <= 0) {
        draw_game_over(cr, width, height);
    }

    return FALSE;
}
