#include <SDL.h>
#include "game.h"
#include "draw.h"
#include "input.h"
#include "assets.h"

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Maze Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    GameState state;
    init_game_state(&state);
    load_assets(renderer);

    int running = 1;
    SDL_Event event;

    while (running) {
        handle_input(&event, &state, &running);
        update_game(&state);
        render_game(renderer, &state);
        SDL_Delay(16);  // ~60 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

