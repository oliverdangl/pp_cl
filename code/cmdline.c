#include <stdio.h>
#include <stdlib.h>
#include "cmdline.h"
#include "config.h"


char args_doc[] = " [-w width WIDTH] [-h height HEIGHT] [-m maze FILE]";
char doc[]      = "";


/* Following order: name, key, arg, flags, doc */
struct argp_option options[] = {
    { "width",  'w', "WIDTH", 0, "Fensterbreite (Default: 1200)" },
    { "height", 'h', "HEIGHT", 0, "Fensterhöhe (Default: 660)" },
    { "maze", 'm', "FILE",  0, "Pfad zur Maze-Datei (Default: \"../maps/level0\")" },
    { 0 }
};

/* Parse commandline option and set corresponding field in GameOptions */
error_t parse_opt(int key, char *arg, struct argp_state *state) {
    GameOptions *opts = state->input;

    switch (key) {
        case 'w': 
            opts->window_width = atoi(arg); //Convert into window width
            break;
        case 'h':
            opts->window_height = atoi(arg); //Convert into window height
            break;
        case 'm':
            opts->maze_file = arg; //Stores path to maze file
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}


//Register of options at argp
struct argp argp = { options, parse_opt, args_doc, doc };


/* Initialize default values and parse command line arguments */
void parse_args(int argc, char **argv, GameOptions *opts) {
    //Setting default values
    opts->window_width  = WINDOW_WIDTH;
    opts->window_height = WINDOW_HEIGHT;
    opts->maze_file     = "../maps/level0";
    
    //Starting argp, takes over argc/argv and writes in opts
    argp_parse(&argp, argc, argv, 0, NULL, opts);
}





