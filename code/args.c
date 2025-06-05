#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <argp.h>
#include "args.h"
#include "game.h"

char args_doc[] = " [--width WIDTH] [--height HEIGHT] [--maze FILE]";
char doc[]      = "Maze Game -- Beispiel zur Argumenterkennung mit argp";



struct argp_option options[] = {
    { "width",  'w', "WIDTH", 0, "Fensterbreite (Default: 1200)" },
    { "height", 'h', "HEIGHT", 0, "Fensterhöhe (Default: 660)" },
    { "maze",   'm', "FILE",  0, "Pfad zur Maze-Datei (Default: \"maze.txt\")" },
    { 0 }
};


error_t parse_opt(int key, char *arg, struct argp_state *state) {
    GameOptions *opts = state->input;

    switch (key) {
        case 'w':
            opts->window_width = atoi(arg);
            break;
        case 'h':
            opts->window_height = atoi(arg);
            break;
        case 'm':
            opts->maze_file = arg;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

struct argp argp = { options, parse_opt, args_doc, doc };


//Initialises default values and calls 
int parse_args(int argc, char **argv, GameOptions *opts) {
    //Setting default values
    opts->window_width  = WINDOW_WIDTH;
    opts->window_height = WINDOW_HEIGHT;
    opts->maze_file     = "../maps/maze.txt";
    
    int index;
    argp_parse(&argp, argc, argv, 0, &index, opts);
    return index;
}




