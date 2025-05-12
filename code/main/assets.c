#include "assets.h"
#include <SDL_image.h>

SDL_Texture *witcher_texture = NULL;

void load_assets(SDL_Renderer *renderer) {
    SDL_Surface *tmp = IMG_Load("assets/witcher.png");
    if (tmp != NULL) {
        witcher_texture = SDL_CreateTextureFromSurface(renderer, tmp);
        SDL_FreeSurface(tmp);
    }
}

void unload_assets() {
    if (witcher_texture != NULL) {
        SDL_DestroyTexture(witcher_texture);
        witcher_texture = NULL;
    }
}

