#ifndef DRAW_H
#define DRAW_H

#include <gtk/gtk.h>
#include <cairo.h>
#include "maze.h"
#include "player.h"


void draw_maze(cairo_t *cr, const Maze *maze, double cell_width, double cell_height);

void draw_player(cairo_t *cr, const PlayerState *player, double cell_width, double cell_height);

void draw_lives(cairo_t *cr, int lives, double width, double height);

void draw_game_over(cairo_t *cr, int width, int height);


// Event handler function
gboolean draw_callback(GtkWidget *drawing_area, cairo_t *cr, gpointer user_data);

#endif

