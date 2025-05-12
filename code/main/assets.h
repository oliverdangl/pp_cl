#ifndef ASSETS_H
#define ASSETS_H

#include <SDL.h>

extern SDL_Texture *witcher_texture;

void load_assets(SDL_Renderer *renderer);
void unload_assets();

#endif

