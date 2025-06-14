#ifndef DRAW_H
#define DRAW_H

#include <gtk/gtk.h>
#include <cairo.h>

typedef struct Maze Maze;
typedef struct PlayerState PlayerState;


void draw_maze(cairo_t *cr, const Maze *maze, double cell_width, double cell_height);

void draw_player(cairo_t *cr, const PlayerState *player, double cell_width, double cell_height);

void draw_lives(cairo_t *cr, int lives, double width, double height);

void draw_time(cairo_t *cr, float seconds, double width, double height);

void draw_game_over(cairo_t *cr, int width, int height);

void cleanup_scare_resource();

// Event handler function
gboolean draw_callback(GtkWidget *drawing_area, cairo_t *cr, gpointer user_data);

#endif

