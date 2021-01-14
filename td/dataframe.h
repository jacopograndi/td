#ifndef dataframeh
#define dataframeh

#include "tdinput.h"
#include "linmath.h"
#include "tdmodel.h"
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
	vec4 color;
	quat rot;
} GameElement;

DYNAMIC_ARRAY_DEFINE(GameElement)

typedef struct gameState {
	Mesh *meshes[1024];
	dyn_arr_GameElement gameElements;
	vec3 cam_forward;
	vec3 cam_up;
	vec3 cam_pos;
	vec3 cam_vel;
	float cam_yaw;
	float cam_pitch;
	vec3 light_pos;
} GameState;

#endif