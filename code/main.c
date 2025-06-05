#include <gtk/gtk.h>
#include "game.h"
#include "draw.h"
#include "input.h"
#include "args.h"

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

    // Keyboard event for key press
    g_signal_connect(window, "key-press-event", G_CALLBACK(on_key_press), gs);

    // Keyboard event for key release
    g_signal_connect(window, "key-release-event", G_CALLBACK(on_key_release), gs);

    // Event for drawing area
    g_signal_connect(area, "draw", G_CALLBACK(draw_callback), gs);
    gtk_widget_add_tick_callback(area, update_callback, gs, NULL); // For updating

    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {


    int arg_index = parse_args(argc, argv, &opts);
    
    printf("DEBUG: window_width  = %d\n", opts.window_width);
    printf("DEBUG: window_height = %d\n", opts.window_height);
    printf("DEBUG: maze_file     = %s\n", opts.maze_file);
    
    
    GtkApplication *app;
    int status;

    GameState gs;
    gs.num_pressed_keys = 256; //Keys on keyboard
    gs.pressed_keys = calloc(gs.num_pressed_keys, sizeof(int)); //Memory allocation for which key is pressed
    gs.maze = NULL;
    gs.original_maze = NULL;

    // Sprite Sheet laden
    gs.player.sprite_sheet = cairo_image_surface_create_from_png("../assets/slime.png");

    // Anfangsrichtung: nach unten (S)
    gs.player.facing_direction = 2;

    // Start-Sprite setzen: nach unten (0,48)
    gs.player.sprite = cairo_surface_create_for_rectangle(gs.player.sprite_sheet, 0, 48, 24, 24);

    if (cairo_surface_status(gs.player.sprite) != CAIRO_STATUS_SUCCESS) {
        fprintf(stderr, "Konnte Sprite-Bild nicht laden!\n");
        return 1;
    }

    // Labyrinth laden
    if (!load_maze_from_file(&gs, opts.maze_file)) {
        fprintf(stderr, "Fehler beim Laden des Labyrinths \"%s\".\n", opts.maze_file);
        return 1;
    }
    

    app = gtk_application_new("org.maze.app", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), &gs); // Connects activate signal with callback function

    status = g_application_run(G_APPLICATION(app), argc - arg_index, argv + arg_index);

    free(gs.pressed_keys);
    free_maze(&gs);

    // Sprite freigeben
    cairo_surface_destroy(gs.player.sprite);
    cairo_surface_destroy(gs.player.sprite_sheet);
    

    g_object_unref(app);
    return status;
}
