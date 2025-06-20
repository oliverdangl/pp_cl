#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "maze.h"
#include "player.h"
#include "draw.h"
#include "input.h"
#include "cmdline.h"
#include "game.h"

static GameOptions opts;

//Level char array including paths to all level
static const char *level_paths[] = {
    "../maps/level0",
    "../maps/level1",
    "../maps/level2",
    "../maps/level3",
    "../maps/level4",
    "../maps/level5",
};


//Calculating number of levels by dividing full array size by one entry
static const int num_levels = sizeof(level_paths) / sizeof(level_paths[0]);


/*
 *Create and configure the main window, drawing area and event callbacks  
 */
static void activate(GtkApplication *app, gpointer user_data) {
    GameState *gs = user_data; //Gamestate

    GtkWidget *window = gtk_application_window_new(app); //Creates new window
    gtk_window_set_title(GTK_WINDOW(window), "Maze Game");
    gtk_window_set_default_size(GTK_WINDOW(window), opts.window_width, opts.window_height); //Default size

    GtkWidget *area = gtk_drawing_area_new(); //Creates area to draw
    gtk_container_add(GTK_CONTAINER(window), area);

    //Event handler
    g_signal_connect(window, "key-press-event", G_CALLBACK(on_key_press), gs);
    g_signal_connect(window, "key-release-event", G_CALLBACK(on_key_release), gs);
    g_signal_connect(area, "draw", G_CALLBACK(draw_callback), gs);
    gtk_widget_add_tick_callback(area, update_callback, gs, NULL);

    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {

    parse_args(argc, argv, &opts);
    GameState gs;
    //Initialize game state fields
    gs.level_paths = level_paths;
    gs.num_levels = num_levels;
    gs.current_level = 0;
    gs.elapsed_time = 0.0f;
    gs.paused = false;
    for(int i = 0; i < gs.num_levels; i++){
        if(strcmp(level_paths[i], opts.maze_file) == 0){
            gs.current_level = i;
            break;
        }
    }
    printf("Starting with level: %s\n", opts.maze_file);
    
    //Displaying values set when loading game
    printf("window_width  = %d\n", opts.window_width);
    printf("window_height = %d\n", opts.window_height);
    printf("default_start = %s\n", level_paths[0]);
    
    GtkApplication *app;
    int status;

    //Allocating memory for all possible keys
    gs.num_pressed_keys = 256;
    gs.pressed_keys = calloc(gs.num_pressed_keys, sizeof(int));
    
    //Allocate maze and player structures
    gs.maze   = malloc(sizeof *gs.maze);
    gs.player = malloc(sizeof *gs.player);
    gs.maze->current = NULL;
    gs.maze->original = NULL;  
    
    
    
    
    //Initialize maze and player
    load_maze_from_file(gs.maze, gs.level_paths[gs.current_level]);
    spawn_player(gs.player, gs.maze);
    initiate_player_sprite(gs.player, "../assets/slime.png");
   
    //Starting GTK application
    app = gtk_application_new("org.maze.app", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), &gs); //Event listener for activate
    status = g_application_run(G_APPLICATION(app), 0, NULL); //Running application
       
    
    //Release allocated memory
    free(gs.pressed_keys);
    free_maze(gs.maze);
    free(gs.maze);
    cleanup_player_sprite(gs.player);
    cleanup_scare_resource();
    free(gs.player);

    g_object_unref(app);
    return status;
}
