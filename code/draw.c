#include "draw.h"
#include "maze.h"
#include "player.h"
#include "game.h"
#include "config.h"
#include <math.h>
#include <stdio.h>

//Jumpscare picture global for only loading once, not every game over
static cairo_surface_t *scare_img = NULL;

// Hilfsfunktion ganz oben in draw.c
static void draw_heart(cairo_t *cr, double x, double y, double size) {
    double s = size / 2;
    cairo_move_to(cr, x, y);
    // linke Hälfte
    cairo_curve_to(cr, x - s, y - s, x - 2 * s, y + s/2, x, y + 2 * s);
    // rechte Hälfte
    cairo_curve_to(cr, x + 2 * s, y + s/2, x + s, y - s, x, y);
    cairo_close_path(cr);
    cairo_fill(cr);
}


/* 
 *Draw the current level time centered at the top
 */
void draw_time(cairo_t *cr, float seconds, double width, double height){
    int mins = (int)seconds / 60;
    int secs = (int)seconds % 60;
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%02d:%02d", mins, secs);
    
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_select_font_face(cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, height * 0.03);
    
    cairo_text_extents_t ext;
    cairo_text_extents(cr, buffer, &ext);
    cairo_move_to(cr, (width - ext.width)/2, height * 0.05);
    cairo_show_text(cr, buffer);
}

// Frees the memory used by the scare image if it was loaded
void cleanup_scare_resource(){
    if(scare_img){
        cairo_surface_destroy(scare_img);
        scare_img = NULL;
    }
}

//Checks whether a trap at position (y,x) has been revealed
static bool is_revealed_trap(const Maze *mz, int y, int x){
    for(int i = 0; i < mz->trap_count; i++){
        if(mz->traps[i].trap_x == x &&
           mz->traps[i].trap_y == y &&
           mz->traps[i].revealed){
            return true;
           }
    }
    return false;
}


// draw playing field
void draw_maze(cairo_t *cr, const Maze *maze, double cell_width, double cell_height) {
    for (int y = 0; y < maze->height; y++) {
        for (int x = 0; x < maze->width; x++) {
            double draw_x = x * cell_width;
            double draw_y = y * cell_height;

            if (maze->current[y][x] == CELL_WALL) {
                // draw Walls
                cairo_set_source_rgb(cr, 0.3, 0.3, 0.3);
                cairo_rectangle(cr, draw_x, draw_y, cell_width, cell_height);
                cairo_fill(cr);
                cairo_set_source_rgb(cr, 0, 0, 0);
                cairo_set_line_width(cr, 1);
                cairo_rectangle(cr, draw_x, draw_y, cell_width, cell_height);
                cairo_stroke(cr);
            }
            else if (maze->current[y][x] == CELL_TRAP || is_revealed_trap(maze, y, x)) {
                //  draw Traps
                cairo_set_source_rgb(cr, 1, 0, 0);
                cairo_arc(cr, draw_x + cell_width/2, draw_y + cell_height/2,
                         fmin(cell_width, cell_height)/3, 0, 2 * M_PI);
                cairo_fill(cr);

            }
            else if(maze->current[y][x] == CELL_PLATE){
            	// draw pressure plate
                cairo_set_source_rgb(cr, 0, 0, 1);
                cairo_arc(cr, draw_x + cell_width/2, draw_y + cell_height/2,
                         fmin(cell_width, cell_height)/3, 0, 2 * M_PI);
                cairo_fill(cr);
            } else if(maze->current[y][x] == CELL_DOOR){
            	// draw door
                cairo_set_source_rgb(cr, 0, 0, 1);
                cairo_rectangle(cr, draw_x, draw_y, cell_width, cell_height);
                cairo_fill(cr);
                cairo_set_source_rgb(cr, 0, 0, 0);
                cairo_set_line_width(cr, 1);
                cairo_rectangle(cr, draw_x, draw_y, cell_width, cell_height);
                cairo_stroke(cr);
                
                 } else {  // CELL_EMPTY
            cairo_set_source_rgb(cr, 0.9, 0.9, 0.9);
            cairo_rectangle(cr, draw_x, draw_y, cell_width, cell_height);
            cairo_fill(cr);
        }
        }
    }
}

void draw_player(cairo_t *cr, const PlayerState *player, double cell_width, double cell_height) {
    if (!player->sprite_sheet) return;

    // calculate player position in maze cell coordinates
    double normalized_x = player->x / CELL_SIZE;
    double normalized_y = player->y / CELL_SIZE;
    double player_x = normalized_x * cell_width - (cell_width / 2);
    double player_y = normalized_y * cell_height - (cell_height / 2);
    double player_size = fmin(cell_width, cell_height) * 0.8;

    // Determine sprite Y-offset and whether to flip based on player direction

    int sprite_x = 0;
    int sprite_y = 0;
    gboolean flip_horizontal = FALSE;  // mirror the sprite

    switch (player->facing) {
        case DIR_UP:  // up
            sprite_y = 0;
            break;
        case DIR_LEFT:  // left → Use sprite from right, but mirrored
            sprite_y = 24;   // Y-position of your “right” sprite
            flip_horizontal = TRUE;
            break;
        case DIR_DOWN:  // down
            sprite_y = 48;
            break;
        case DIR_RIGHT:  // right
            sprite_y = 24;
            break;
        default:
            sprite_y = 24;
            break;
    }

    // draw the player sprite
    cairo_save(cr);
    cairo_translate(cr, player_x + cell_width / 2, player_y + cell_height / 2);

    if (flip_horizontal) {
        // Flip horizontal for "left"
        cairo_scale(cr, -player_size / 24.0, player_size / 24.0);
        cairo_translate(cr, -12, -12);  // Correct the sprite center offset
        
    } else {
        cairo_scale(cr, player_size / 24.0, player_size / 24.0);
        cairo_translate(cr, -12, -12);
    }

    // Cut out a 24x24 sprite from the sprite sheet at position (sprite_x, sprite_y)
    cairo_surface_t *sprite = cairo_surface_create_for_rectangle(
    player->sprite_sheet, sprite_x, sprite_y, 24, 24);
    cairo_set_source_surface(cr, sprite, 0, 0);
    cairo_paint(cr);
    cairo_surface_destroy(sprite);
    cairo_restore(cr);
}


void draw_game_over(cairo_t *cr, int width, int height) {
    //Draw background picture
    if(scare_img == NULL){
        // load only once
        scare_img = cairo_image_surface_create_from_png("../assets/scare.png");
    }
    // widh and height of image
    int img_w = cairo_image_surface_get_width(scare_img);
    int img_h = cairo_image_surface_get_height(scare_img);
    
    //scaled
    double scale_x = (double)width / img_w;
    double scale_y = (double)height / img_h;
    
    cairo_save(cr);
    cairo_scale(cr, scale_x, scale_y);
    cairo_set_source_surface(cr, scare_img, 0, 0);
    cairo_paint(cr);
    cairo_restore(cr);

    // "GAME OVER" Text
    cairo_set_font_size(cr, 40);
    cairo_set_source_rgb(cr, 1, 0, 0);
    const char *text = "GAME OVER";
    cairo_text_extents_t extents;
    cairo_text_extents(cr, text, &extents);
    cairo_move_to(cr, (width - extents.width)/2, height/2);
    cairo_show_text(cr, text);

    // a notice Text
    cairo_set_font_size(cr, 24);
    const char *hint = "Press ENTER to Restart";
    cairo_text_extents(cr, hint, &extents);
    cairo_move_to(cr, (width - extents.width)/2, height/2 + 40);
    cairo_show_text(cr, hint);
}

void draw_lives(cairo_t *cr, int lives, double width, double height) {
    double life_width = width * 0.015;
    double life_height = height * 0.02;
    double padding = width * 0.01;
    double start_x = width * 0.02;
    double start_y = height * 0.05;

    cairo_set_source_rgb(cr, 1, 0, 0);
    cairo_select_font_face(cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, height * 0.03);
    cairo_move_to(cr, start_x, start_y);
    cairo_show_text(cr, "Lives:");

    cairo_set_source_rgb(cr, 1, 0, 0);  // red Heart
    for (int i = 0; i < lives; i++) {
    double cx = start_x + (life_width + padding) * (i + 3) + life_width/2;
    double cy = start_y - life_height/2;
    draw_heart(cr, cx, cy, life_width);
}

}

gboolean draw_callback(GtkWidget *drawing_area, cairo_t *cr, gpointer user_data) {
    GameState *gs = (GameState *)user_data;
    GtkAllocation alloc;
    gtk_widget_get_allocation(drawing_area, &alloc);
    int width  = alloc.width;
    int height = alloc.height;

    // 1) Right at the beginning: paint the whole window light grey
    cairo_set_source_rgb(cr, 0.9, 0.9, 0.9);
    cairo_paint(cr);

    // 2) Reserve vertical space at the top for the Lives UI

    double top_margin  = height * 0.08;
    double maze_height = height - top_margin;

    // 3) Compute cell size based on the remaining space (excluding the top margin)

    double cell_width  = (double)width / gs->maze->width;
    double cell_height = maze_height   / gs->maze->height;

    // 4) Draw Maze + Player in the lower area
    cairo_save(cr);
    cairo_translate(cr, 0, top_margin);
    draw_maze(cr,   gs->maze,   cell_width, cell_height);
    draw_player(cr, gs->player, cell_width, cell_height);
    cairo_restore(cr);

    // 5) Lives at the top (above the Maze area)
    draw_lives(cr, gs->player->lives, width, height);
    float remaining = fmaxf(0.0f, LEVEL_TIME_LIMIT - gs->elapsed_time);
    draw_time(cr, remaining, width, height);

    // 6) Game Over possibly in the middle of the Maze area
    if (gs->player->lives <= 0 || remaining == 0) {
        draw_game_over(cr, width, height);
    }

    return FALSE;
}
