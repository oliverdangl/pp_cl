#include "draw.h"
#include "assets.h"

void render_game(SDL_Renderer *renderer, GameState *state) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (state->game_over) {
        // Game Over Anzeige
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect rect = {200, 200, 240, 80};
        SDL_RenderFillRect(renderer, &rect);
    } else {
        // Spielfigur zeichnen
        SDL_Rect player = {state->player_x, state->player_y, 32, 32};
        SDL_RenderCopy(renderer, witcher_texture, NULL, &player);
    }

    SDL_RenderPresent(renderer);
}

