#ifndef gamerenderh
#define gamerenderh

#include "linmath.h"
#include "model.h"

void mat_from_transform (mat4x4 mat, vec3 pos, vec3 scale, quat rot);
void game_render (GLFWwindow *window, int shader, int shaderterrain, GameState *gst);

#endif