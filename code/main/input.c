#include "input.h"

void handle_input(SDL_Event *event, GameState *state, int *running) {
    while (SDL_PollEvent(event)) {
        if (event->type == SDL_QUIT) {
            *running = 0;
        }
        if (event->type == SDL_KEYDOWN) {
            switch (event->key.keysym.sym) {
                case SDLK_ESCAPE:
                    *running = 0;
                    break;
                case SDLK_r:
                    reset_game(state);
                    break;
                case SDLK_LEFT:
                    state->player_x -= 5;
                    break;
                case SDLK_RIGHT:
                    state->player_x += 5;
                    break;
                case SDLK_UP:
                    state->player_y -= 5;
                    break;
                case SDLK_DOWN:
                    state->player_y += 5;
                    break;
            }
        }
    }
}

