#include <gtk/gtk.h>
#include "game.h"
#include "draw.h"
#include "input.h"

static void activate(GtkApplication *app, gpointer user_data) {
    GameState *gs = user_data;

    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Maze Game");
    gtk_window_set_default_size(GTK_WINDOW(window), WINDOW_WIDTH, WINDOW_HEIGHT);

    GtkWidget *area = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(window), area);

    g_signal_connect(window, "key-press-event", G_CALLBACK(on_key_press), gs);
    g_signal_connect(window, "key-release-event", G_CALLBACK(on_key_release), gs);
    g_signal_connect(area, "draw", G_CALLBACK(draw_callback), gs);
    gtk_widget_add_tick_callback(area, update_callback, gs, NULL);

    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    GameState gs;
    gs.num_pressed_keys = 256;
    gs.pressed_keys = calloc(gs.num_pressed_keys, sizeof(int));

    gs.player.sprite = NULL;
    gs.player.sprite_short = NULL;

    reset_game(&gs);

    app = gtk_application_new("org.maze.app", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), &gs);
    status = g_application_run(G_APPLICATION(app), argc, argv);

    free(gs.pressed_keys);
    g_object_unref(app);
    return status;
}

