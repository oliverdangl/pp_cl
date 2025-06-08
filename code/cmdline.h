#ifndef ARGS_H
#define ARGS_H

#include <argp.h>

/*
 *Struct to hold command-line options
 */
typedef struct GameOptions{
    int window_width;
    int window_height;
    char *maze_file;
} GameOptions;

extern  char args_doc[];
extern  char doc[];


extern struct argp_option options[];
error_t parse_opt(int key, char *arg, struct argp_state *state);
extern struct argp argp;

void parse_args(int argc, char **argv, GameOptions *opts);

#endif

