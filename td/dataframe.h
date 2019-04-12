#ifndef dataframeh
#define dataframeh

#include "gameinput.h"
#include "linmath.h"

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

typedef struct {
	float *hmap;
} Terrain;

typedef struct {
	float pos[2];
	int id;
} Unit;

typedef struct gameState {
	Unit units[1024];
	int unitsnum;
	Terrain terrain;
	vec3 cam_forward;
	vec3 cam_up;
	vec3 cam_pos;
	float cam_yaw;
	float cam_pitch;
	vec3 light_pos;
} GameState;

#endif