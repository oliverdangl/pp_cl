#ifndef GAME_H
#define GAME_H

typedef struct {
    int player_x, player_y;
    int lives;
    int game_over;
} GameState;

void init_game_state(GameState *state);
void reset_game(GameState *state);
void update_game(GameState *state);

#endif

