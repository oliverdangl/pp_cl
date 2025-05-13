#include "input.h"

static int is_wall_collision(GameState *game_state, float x, float y) {
    const float hitbox_size = 28.0f;
    const float offset = (32.0f - hitbox_size) / 2.0f;

    float hitbox_left = x + offset;
    float hitbox_top = y + offset;
    float hitbox_right = hitbox_left + hitbox_size - 0.01f;
    float hitbox_bottom = hitbox_top + hitbox_size - 0.01f;

    int left = (int)(hitbox_left / CELL_SIZE);
    int right = (int)(hitbox_right / CELL_SIZE);
    int top = (int)(hitbox_top / CELL_SIZE);
    int bottom = (int)(hitbox_bottom / CELL_SIZE);

    if (left < 0 || right >= MAZE_WIDTH || top < 0 || bottom >= MAZE_HEIGHT)
        return 1;

    return game_state->maze[top][left] == WALL || game_state->maze[top][right] == WALL ||
           game_state->maze[bottom][left] == WALL || game_state->maze[bottom][right] == WALL;
}

gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    GameState *game_state = (GameState *)user_data;
    if (game_state->lives <= 0 && event->keyval == GDK_KEY_Return) {
        reset_game(game_state);
        memset(game_state->pressed_keys, 0, game_state->num_pressed_keys * sizeof(int));
        return TRUE;
    }

    game_state->pressed_keys[event->keyval] = 1;
    return TRUE;
}

gboolean on_key_release(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    GameState *game_state = (GameState *)user_data;
    if (game_state->lives <= 0)
        return TRUE;
    game_state->pressed_keys[event->keyval] = 0;
    return TRUE;
}

gboolean update_callback(GtkWidget *widget, GdkFrameClock *clock, gpointer user_data) {
    GameState *gs = (GameState *)user_data;
    static gint64 prev_time = 0;

    if (gs->lives <= 0) {
        gtk_widget_queue_draw(widget);
        return G_SOURCE_CONTINUE;
    }

    gint64 now = gdk_frame_clock_get_frame_time(clock);
    if (prev_time == 0) {
        prev_time = now;
        return G_SOURCE_CONTINUE;
    }

    float dt = (now - prev_time) / 1000000.0f;
    prev_time = now;

    float dx = 0, dy = 0, speed = 200.0f * dt;
    if (gs->pressed_keys['w'] || gs->pressed_keys['W']) dy -= speed;
    if (gs->pressed_keys['s'] || gs->pressed_keys['S']) dy += speed;
    if (gs->pressed_keys['a'] || gs->pressed_keys['A']) dx -= speed;
    if (gs->pressed_keys['d'] || gs->pressed_keys['D']) dx += speed;

    float new_x = gs->player.x + dx;
    float new_y = gs->player.y + dy;
    if (!is_wall_collision(gs, new_x, new_y)) {
        gs->player.x = new_x;
        gs->player.y = new_y;
    }

    int cell_x = gs->player.x / CELL_SIZE;
    int cell_y = gs->player.y / CELL_SIZE;
    int in_trap = gs->maze[cell_y][cell_x] == TRAP;

    if (in_trap && !gs->trap_visited) {
        gs->lives--;
        gs->trap_visited = 1;
    } else if (!in_trap) {
        gs->trap_visited = 0;
    }

    gtk_widget_queue_draw(widget);
    return G_SOURCE_CONTINUE;
}

