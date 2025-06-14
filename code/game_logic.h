typedef struct GameState GameState;
typedef struct Maze Maze;

//Reveal traps near the player
void reveal_traps_near_player(GameState *gs, float reveal_distance);

//Handle trap activation and live decrement
void handle_traps(GameState *gs);

//Handle preassure plate steppinng and door opening
void handle_plates(GameState *gs);

//Scan maze for traps
int scan_traps(const Maze *mz);

//Scan maze for plates
int scan_plates(const Maze *mz);

//Place plates and traps in the maze
void place_plates_and_traps(Maze *mz);

//Checks for collision with walls or door
int is_wall_collision(Maze *maze, float x, float y);

//Scan field for door coordinate and if empty for level transition
void check_level_transition(GameState *gs);
