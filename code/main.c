#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>

#include "config.h"
#include "maze.h"
#include "player.h"
#include "draw.h"
#include "input.h"
#include "cmdline.h"
#include "game.h"

static GameOptions opts;


/*
This function starts the App and reacts on events untill window is closed
*/
static void activate(GtkApplication *app, gpointer user_data) {
    GameState *gs = user_data;

    GtkWidget *window = gtk_application_window_new(app); // Creates new window
    gtk_window_set_title(GTK_WINDOW(window), "Maze Game"); // Sets title for window
    gtk_window_set_default_size(GTK_WINDOW(window), opts.window_width, opts.window_height);

    GtkWidget *area = gtk_drawing_area_new(); // Creates area to draw
    gtk_container_add(GTK_CONTAINER(window), area);

    //event handler
    g_signal_connect(window, "key-press-event", G_CALLBACK(on_key_press), gs);
    g_signal_connect(window, "key-release-event", G_CALLBACK(on_key_release), gs);
    g_signal_connect(area, "draw", G_CALLBACK(draw_callback), gs);
    gtk_widget_add_tick_callback(area, update_callback, gs, NULL); // For updating

    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    int arg_index = parse_args(argc, argv, &opts);
    
    //displaying values set when loading game
    printf("window_width  = %d\n", opts.window_width);
    printf("window_height = %d\n", opts.window_height);
    printf("maze_file     = %s\n", opts.maze_file);
    
    GtkApplication *app;
    int status;
    GameState gs;

    gs.num_pressed_keys = 256; //Keys on keyboard
    gs.pressed_keys = calloc(gs.num_pressed_keys, sizeof(int)); //Memory allocation for which key is pressed
    
    // allocate maze and player structures
    gs.maze   = malloc(sizeof *gs.maze);
    gs.player = malloc(sizeof *gs.player);
    
    gs.maze->current = NULL;
    gs.maze->original = NULL;

    //sprite section
    gs.player->sprite_sheet = cairo_image_surface_create_from_png("../assets/slime.png");
    gs.player->sprite = cairo_surface_create_for_rectangle(gs.player->sprite_sheet, 0, 48, 24, 24); //startsprite values

    //loading maze
    load_maze_from_file(gs.maze, opts.maze_file);
    spawn_player(gs.player, gs.maze);
   
    app = gtk_application_new("org.maze.app", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), &gs); // Connects activate signal with callback function
    status = g_application_run(G_APPLICATION(app), argc - arg_index, argv + arg_index);

    //seting allocated memory free
    free(gs.pressed_keys);
    free_maze(gs.maze);
    free(gs.maze);
    cairo_surface_destroy(gs.player->sprite);
    cairo_surface_destroy(gs.player->sprite_sheet);
    free(gs.player);

    g_object_unref(app);
    return status;
}
