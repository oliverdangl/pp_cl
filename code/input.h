#ifndef INPUT_H
#define INPUT_H

#include <gtk/gtk.h>


//Callbacks for key events
gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
gboolean on_key_release(GtkWidget *widget, GdkEventKey *event, gpointer user_data);

//Callback for game update
gboolean update_callback(GtkWidget *widget, GdkFrameClock *clock, gpointer user_data);

#endif

