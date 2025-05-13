#ifndef DRAW_H
#define DRAW_H

#include <gtk/gtk.h>
#include "game.h"


void draw_map(cairo_t *cr, int **map, int tile_size);


void draw_player(cairo_t *cr, const Player *player);

void draw_visibility(cairo_t *cr, int px, int py, int tile_size, int vision_radius, int map_width, int map_height);

gboolean draw_callback(GtkWidget *drawing_area, cairo_t *cr, gpointer user_data);

#endif

