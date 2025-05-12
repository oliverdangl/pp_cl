#include <gtk/gtk.h>
#include <cairo.h>
#include <stdlib.h>
#include <assert.h>

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 800
#define CELL_SIZE 40
#define MAZE_WIDTH (WINDOW_WIDTH / CELL_SIZE)
#define MAZE_HEIGHT (WINDOW_HEIGHT / CELL_SIZE)
#define PLAYER_HITBOX_SIZE 32
#define MAX_LIVES 3

#define WALL 1
#define TRAP 2

typedef struct {
    float x, y;
    cairo_surface_t *sprite_short;
    cairo_surface_t *sprite;
} Player;

typedef struct {
    Player player;
    int *pressed_keys;
    unsigned int num_pressed_keys;
    int maze[MAZE_HEIGHT][MAZE_WIDTH];
    int lives;
    int trap_visited;  // Track if player is in the trap
} GameState;

// Beispiel-Labyrinth (Wand = 1, Trap = 2)
int maze_template[MAZE_HEIGHT][MAZE_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,2,1},
    {1,2,1,0,1,0,1,1,0,1,0,1,1,1,1,1,1,1,0,1},
    {1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,1},
    {1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1},
    {1,0,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,0,1},
    {1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1},
    {1,0,1,0,1,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1},
    {1,0,2,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

static void draw_maze(cairo_t *cr, GameState *game_state) {
    for (int y = 0; y < MAZE_HEIGHT; y++) {
        for (int x = 0; x < MAZE_WIDTH; x++) {
            int cell = game_state->maze[y][x];

            if (cell == WALL) {
                cairo_set_source_rgb(cr, 0.3, 0.3, 0.3); // Dunkelgrau
                cairo_rectangle(cr, x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE);
                cairo_fill(cr);

                cairo_set_source_rgb(cr, 0, 0, 0); // Schwarzer Rand
                cairo_set_line_width(cr, 1);
                cairo_rectangle(cr, x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE);
                cairo_stroke(cr);
            } else if (cell == TRAP) {
                cairo_set_source_rgb(cr, 1, 0, 0); // Rot
                cairo_arc(cr, x * CELL_SIZE + 20, y * CELL_SIZE + 20, 10, 0, 2 * G_PI);
                cairo_fill(cr);
            }
        }
    }
}

static int is_wall_collision(GameState *game_state, float x, float y) {
    const float hitbox_size = 28.0f;
    const float offset = (32.0f - hitbox_size) / 2.0f;

    float hitbox_left   = x + offset;
    float hitbox_top    = y + offset;
    float hitbox_right  = hitbox_left + hitbox_size - 0.01f;
    float hitbox_bottom = hitbox_top + hitbox_size - 0.01f;

    int left_cell   = (int)(hitbox_left / CELL_SIZE);
    int right_cell  = (int)((hitbox_right - 0.01f) / CELL_SIZE);
    int top_cell    = (int)(hitbox_top / CELL_SIZE);
    int bottom_cell = (int)((hitbox_bottom - 0.01f) / CELL_SIZE);

    // Randprüfung
    if (left_cell < 0 || right_cell < 0 || top_cell < 0 || bottom_cell < 0 ||
        left_cell >= MAZE_WIDTH || right_cell >= MAZE_WIDTH ||
        top_cell >= MAZE_HEIGHT || bottom_cell >= MAZE_HEIGHT)
        return 1;

    // Kollision mit Wänden prüfen
    return (
        game_state->maze[top_cell][left_cell] == WALL ||
        game_state->maze[top_cell][right_cell] == WALL ||
        game_state->maze[bottom_cell][left_cell] == WALL ||
        game_state->maze[bottom_cell][right_cell] == WALL
    );
}

static void player_draw(cairo_t *cr, const Player *player) {
    float offset = (32 - PLAYER_HITBOX_SIZE) / 2.0f;
    cairo_set_source_surface(cr, player->sprite, player->x - offset, player->y - offset);
    cairo_paint(cr);
}

static void draw_lives(cairo_t *cr, GameState *game_state) {
    // Wenn keine Leben mehr übrig sind, zeige GAME OVER an
    if (game_state->lives <= 0) {
        // GAME OVER Text zentrieren
        cairo_set_source_rgb(cr, 1, 0, 0); // Rot für GAME OVER
        cairo_select_font_face(cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, 40);

        // Berechne die Position, um den Text zu zentrieren
        const char *text = "GAME OVER";
        cairo_text_extents_t extents;
        cairo_text_extents(cr, text, &extents);
        double x = (WINDOW_WIDTH - extents.width) / 2.0;
        double y = WINDOW_HEIGHT / 2.0;

        cairo_move_to(cr, x, y);
        cairo_show_text(cr, text);

        cairo_set_source_rgb(cr, 1,0,0); // Blau für Anweisung
        cairo_select_font_face(cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        cairo_set_font_size(cr, 20);

        const char *restart_text = "Press Enter to Restart";
        cairo_text_extents(cr, restart_text, &extents);
        x = (WINDOW_WIDTH - extents.width) / 2.0;
        y += 40;  // Setzt den Text unterhalb von "GAME OVER"

        cairo_move_to(cr, x, y);
        cairo_show_text(cr, restart_text);
        return; // Keine Lebensanzeige mehr, wenn GAME OVER
    }

    // Lebensanzeige zeichnen, wenn noch Leben übrig sind
    int life_width = 30;
    int life_height = 10;
    int padding = 10;
    int x_start = padding;  // Setzt den Startpunkt für die Lebensanzeige auf die linke Seite
    int y_start = WINDOW_HEIGHT - 50; // Positioniert die Lebensanzeige 50 Pixel vom unteren Rand entfernt

    cairo_set_source_rgb(cr, 0, 153, 0); // Grün für Text
    cairo_select_font_face(cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 18);
    cairo_move_to(cr, x_start - padding, y_start);
    cairo_show_text(cr, "Lives: ");

    for (int i = 0; i < game_state->lives; i++) {
        cairo_set_source_rgb(cr, 0, 0.7, 0); // Grün für Leben
        cairo_rectangle(cr, x_start + i * (life_width + padding), y_start + 10, life_width, life_height);
        cairo_fill(cr);
    }
}




static gboolean draw_callback(GtkWidget *drawing_area, cairo_t *cr, gpointer user_data) {
    GameState *game_state = (GameState *)user_data;

    // Hintergrund
    cairo_set_source_rgb(cr, 0.9, 0.9, 0.9); // Hellgrau
    cairo_rectangle(cr, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    cairo_fill(cr);

    draw_maze(cr, game_state);
    player_draw(cr, &game_state->player);
    draw_lives(cr, game_state);

    return FALSE;
}

void reset_game(GameState *game_state) {
    game_state->lives = MAX_LIVES;
    game_state->trap_visited = 0;
    game_state->player.x = CELL_SIZE + 4;
    game_state->player.y = CELL_SIZE + 4;
    
    // Maze zurücksetzen, falls notwendig
    for (int y = 0; y < MAZE_HEIGHT; y++) {
        for (int x = 0; x < MAZE_WIDTH; x++) {
            game_state->maze[y][x] = maze_template[y][x];
        }
    }
}



static gboolean update_callback(GtkWidget *widget, GdkFrameClock *clock, gpointer user_data) {
    GameState *game_state = (GameState *)user_data;
    static gint64 previous_time = 0;
    
     if (game_state->lives <= 0) {
        gtk_widget_queue_draw(widget); // Nur weiterzeichnen
        return G_SOURCE_CONTINUE; // Keine Bewegung
    }

    gint64 current_time = gdk_frame_clock_get_frame_time(clock);
    if (previous_time == 0) {
        previous_time = current_time;
        return G_SOURCE_CONTINUE;
    }

    float dt = (current_time - previous_time) / 1000000.0f;
    previous_time = current_time;

    float new_x = game_state->player.x;
    float new_y = game_state->player.y;
    const float speed = 200.0f * dt;

    // Bewegungslogik (W, A, S, D Tasten)
    if (game_state->pressed_keys['w'] || game_state->pressed_keys['W']) new_y -= speed;
    if (game_state->pressed_keys['s'] || game_state->pressed_keys['S']) new_y += speed;
    if (game_state->pressed_keys['a'] || game_state->pressed_keys['A']) new_x -= speed;
    if (game_state->pressed_keys['d'] || game_state->pressed_keys['D']) new_x += speed;

    // Kollision prüfen und Spielerposition aktualisieren
    if (!is_wall_collision(game_state, new_x, new_y)) {
        game_state->player.x = new_x;
        game_state->player.y = new_y;
    }

    // Falle betreten/verlässt Logik
    int is_in_trap = 0;
    const float hitbox_size = 28.0f;
    const float offset = (32.0f - hitbox_size) / 2.0f;

    float hitbox_left = game_state->player.x + offset;
    float hitbox_top = game_state->player.y + offset;
    float hitbox_right = hitbox_left + hitbox_size - 0.01f;
    float hitbox_bottom = hitbox_top + hitbox_size - 0.01f;

    int left_cell = (int)(hitbox_left / CELL_SIZE);
    int right_cell = (int)((hitbox_right - 0.01f) / CELL_SIZE);
    int top_cell = (int)(hitbox_top / CELL_SIZE);
    int bottom_cell = (int)((hitbox_bottom - 0.01f) / CELL_SIZE);

    // Überprüfen, ob die Falle betreten wurde
    if (game_state->maze[top_cell][left_cell] == TRAP || game_state->maze[top_cell][right_cell] == TRAP ||
        game_state->maze[bottom_cell][left_cell] == TRAP || game_state->maze[bottom_cell][right_cell] == TRAP) {
        is_in_trap = 1;
    }

    // Überprüfen, ob die Falle gerade betreten oder verlassen wurde
    if (is_in_trap && !game_state->trap_visited) {
        game_state->trap_visited = 1;
        if (game_state->lives > 0) {
            game_state->lives -= 1;  // Leben verlieren
        }
    } else if (!is_in_trap) {
        game_state->trap_visited = 0; // Falle wurde verlassen
    }

    gtk_widget_queue_draw(widget);
    return G_SOURCE_CONTINUE;
}

static gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    GameState *game_state = (GameState *)user_data;

    // Wenn Game Over: nur Enter zulassen
    if (game_state->lives <= 0) {
        if (event->keyval == GDK_KEY_Return) {
            // Spiel zurücksetzen
            game_state->player.x = CELL_SIZE + 4;
            game_state->player.y = CELL_SIZE + 4;
            game_state->lives = MAX_LIVES;
            game_state->trap_visited = 0;
            memset(game_state->pressed_keys, 0, game_state->num_pressed_keys * sizeof(int));
        }
        return TRUE;
    }

    game_state->pressed_keys[event->keyval] = 1;
    return TRUE;
}




static gboolean on_key_release(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    GameState *game_state = (GameState *)user_data;

    // Bei Game Over keine Tasten speichern
    if (game_state->lives <= 0)
        return TRUE;

    game_state->pressed_keys[event->keyval] = 0;
    return TRUE;
}


static void activate(GtkApplication *app, gpointer user_data) {
    GameState *game_state = (GameState *)user_data;

    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Maze Labyrinth");
    gtk_window_set_default_size(GTK_WINDOW(window), WINDOW_WIDTH, WINDOW_HEIGHT);

    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(window), drawing_area);

    // Callback-Funktionen für Events verbinden
    g_signal_connect(window, "key-press-event", G_CALLBACK(on_key_press), game_state);
    g_signal_connect(window, "key-release-event", G_CALLBACK(on_key_release), game_state);

    // Frame-Clock Signal auf drawing_area anwenden
    g_signal_connect(drawing_area, "draw", G_CALLBACK(draw_callback), game_state);
    gtk_widget_add_tick_callback(drawing_area, update_callback, game_state, NULL);

    // Das Fenster anzeigen
    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    GameState game_state;
    game_state.num_pressed_keys = 256;
    game_state.pressed_keys = calloc(game_state.num_pressed_keys, sizeof(int));
    game_state.lives = MAX_LIVES;
    game_state.trap_visited = 0;

    for (int y = 0; y < MAZE_HEIGHT; y++) {
        for (int x = 0; x < MAZE_WIDTH; x++) {
            game_state.maze[y][x] = maze_template[y][x];
        }
    }

    game_state.player.sprite_short = cairo_image_surface_create_from_png("assets/witcher.png");
    if (cairo_surface_status(game_state.player.sprite_short) != CAIRO_STATUS_SUCCESS) {
        game_state.player.sprite_short = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 32, 32);
        cairo_t *cr = cairo_create(game_state.player.sprite_short);
        cairo_set_source_rgb(cr, 0, 0.7, 0);
        cairo_rectangle(cr, 0, 0, 32, 32);
        cairo_fill(cr);
        cairo_destroy(cr);
    }

    game_state.player.sprite = cairo_surface_create_for_rectangle(
        game_state.player.sprite_short, 0, 0, 32, 32);

    game_state.player.x = CELL_SIZE + 4;
    game_state.player.y = CELL_SIZE + 4;

    app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), &game_state);
    status = g_application_run(G_APPLICATION(app), argc, argv);

    cairo_surface_destroy(game_state.player.sprite);
    cairo_surface_destroy(game_state.player.sprite_short);
    free(game_state.pressed_keys);
    g_object_unref(app);

    return status;
}
