#ifndef dataframeh
#define dataframeh

#include "gameinput.h"
#include "linmath.h"
#include "model.h"
#include "dynamic_array.h"

typedef struct windowOpt {
	int scr_width;
	int scr_height; 
} WindowOpt;

typedef struct windowPtr {
	WindowOpt *opt;
	GameInput *com;
	float delta_time;
	float last_frame;
} WindowPtr;

typedef struct gameElement {
	Mesh *mesh;
	vec3 pos;
	vec3 scale;
	quat rot;
} GameElement;

DYNAMIC_ARRAY_DEFINE(GameElement)

typedef struct gameState {
	dyn_arr_GameElement gameElements;
	vec3 cam_forward;
	vec3 cam_up;
	vec3 cam_pos;
	float cam_yaw;
	float cam_pitch;
	vec3 light_pos;
} GameState;

#endif