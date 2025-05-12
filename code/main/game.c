#include "game.h"

void init_game_state(GameState *state) {
    state->player_x = 100;
    state->player_y = 100;
    state->lives = 3;
    state->game_over = 0;
}

void reset_game(GameState *state) {
    init_game_state(state);
}

void update_game(GameState *state) {
    if (state->lives <= 0) {
        state->game_over = 1;
    }
}

