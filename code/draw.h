#ifndef DRAW_H
#define DRAW_H

#include <gtk/gtk.h>
#include "game.h"


// Event handler function
gboolean draw_callback(GtkWidget *drawing_area, cairo_t *cr, gpointer user_data);

#endif

