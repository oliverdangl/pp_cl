#ifndef INPUT_H
#define INPUT_H

#include <SDL.h>
#include "game.h"

void handle_input(SDL_Event *event, GameState *state, int *running);

#endif

