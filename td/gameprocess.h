#ifndef gameprocessh
#define gameprocessh

#include "dataframe.h"
#include "gameinput.h"

void game_init(GLFWwindow* window, GameState *gst);
void game_process(GLFWwindow* window, GameState *gst, GameInput *com);

#endif