#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "dataframe.h"
#include "tdinput.h"

#define DEBUG_RAYCAST 0

float raycast_nearest(GameState* gst, vec3 out, vec3 start, vec3 dir);

void game_init (GLFWwindow* window, GameState *gst) {
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	gst->cam_pos[0] = 0; gst->cam_pos[1] = 2; gst->cam_pos[2] = 0;
	gst->cam_forward[0] = 0; gst->cam_forward[1] = 0; gst->cam_forward[2] = 1;
	gst->cam_up[0] = 0; gst->cam_up[1] = 1; gst->cam_up[2] = 0;
	gst->cam_pitch = 0; gst->cam_yaw = -90;
	gst->cam_vel[0] = 0; gst->cam_vel[1] = 0; gst->cam_vel[2] = 0;
	gst->light_pos[0] = 0; gst->light_pos[1] = 1; gst->light_pos[2] = 0; 
	dyn_arr_GameElement_init(&gst->gameElements);
}

void game_process (GLFWwindow* window, GameState *gst, GameInput *com) {
	WindowPtr *ptr = (WindowPtr*) glfwGetWindowUserPointer(window);
	WindowOpt *opt = ptr->opt;

	float time = glfwGetTime();
	gst->light_pos[0] = cos(time)*3;
	gst->light_pos[2] = sin(time)*3;

	vec3 acc = { 0, 0, 0 };
	float cam_speed = ptr->delta_time * 1;
	float jump_speed = ptr->delta_time * 100;

    if(com->first_mouse_flag)
    {
        com->last_xpos = com->xpos;
        com->last_ypos = com->ypos;
        com->first_mouse_flag = 0;
    }
  
    float xoffset = com->xpos - com->last_xpos;
    float yoffset = com->last_ypos - com->ypos; 
    com->last_xpos = com->xpos;
    com->last_ypos = com->ypos;

    float sensitivity = ptr->delta_time * 2;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    gst->cam_yaw   += xoffset;
    gst->cam_pitch += yoffset;

    if(gst->cam_pitch > 89.0f)
        gst->cam_pitch = 89.0f;
    if(gst->cam_pitch < -89.0f)
        gst->cam_pitch = -89.0f;

	float *f = gst->cam_forward;
	f[0] = cos(deg_to_rad(gst->cam_yaw)) * cos(deg_to_rad(gst->cam_pitch));
	f[1] = sin(deg_to_rad(gst->cam_pitch));
	f[2] = sin(deg_to_rad(gst->cam_yaw)) * cos(deg_to_rad(gst->cam_pitch));
	vec3_norm(f, f);


	vec3 normCam;  vec3_norm(normCam, gst->cam_forward);
	vec3 dir; vec3_scale(dir, gst->cam_up, -1); vec3 out;
	float dist = raycast_nearest(gst, out, gst->cam_pos, dir);
	int grounded = 0;
	if (dist < 2) {
		grounded = 1;
	}

	if (com->keypress[1]) {
		vec3 dp; vec3_mul_cross(dp, gst->cam_forward, gst->cam_up);
		vec3_norm(dp, dp); vec3_scale(dp, dp, cam_speed);
		vec3_sub(acc, acc, dp);
	}
	if (com->keypress[3]) {
		vec3 dp; vec3_mul_cross(dp, gst->cam_forward, gst->cam_up);
		vec3_norm(dp, dp); vec3_scale(dp, dp, cam_speed);
		vec3_add(acc, acc, dp);
	}
	if (com->keypress[0]) {
		vec3 dp; vec3_scale(dp, f, cam_speed);
		vec3_add(acc, acc, dp);
	}
	if (com->keypress[2]) {
		vec3 dp; vec3_scale(dp, f, cam_speed);
		vec3_sub(acc, acc, dp);
	}
	if (com->keypress[4] && grounded) {
		// space
	}

	vec3_add(gst->cam_vel, gst->cam_vel, acc);
	vec3_add(gst->cam_pos, gst->cam_pos, gst->cam_vel);
	gst->cam_pos[0] *= 0.85f;
	gst->cam_pos[1] *= 0.85f;
	gst->cam_pos[2] *= 0.85f;
	//printf("(%f, %f, %f)\n", gst->cam_pos[0], gst->cam_pos[1], gst->cam_pos[2]);
	/*
	float dist = raycast_nearest(gst, out, gst->cam_pos, dir);
	int grounded = 0;
	if (dist < 2) {
		grounded = 1;
	}*/
}

float raycast_nearest (GameState *gst, vec3 out, vec3 start, vec3 dir) {
	vec3 outPoint; Mesh* mesh; int hitFlag = 0;
	vec3 norm;  vec3_norm(norm, dir);
	vec3 hits[128]; int hitsnum = 0;
	for (int i = 0; i < 1; i++) { // check only the first element
		mesh = gst->gameElements.arr[i].mesh;
		for (int j = 0; j < mesh->verts.cur; j += 3) {
			hitFlag = raytrace_trigon(gst->cam_pos, norm,
				mesh->verts.arr[j].pos, mesh->verts.arr[j + 1].pos, mesh->verts.arr[j + 2].pos,
				outPoint
			);
			if (hitFlag == 1) {
				for (int k = 0; k < 3; k++)  hits[hitsnum][k] = outPoint[k];
				hitsnum++;
			}
		}
	}
	if (hitsnum > 0) {
		vec3 diff;
		int numviz = 0;
		float maxdist = 100000000.0f; int imax = 0; float dist = 0;
		for (int k = 0; k < hitsnum; k++) {
			vec3_sub(diff, hits[k], gst->cam_pos);
			dist = vec3_mul_inner(diff, diff);
			if (dist < maxdist) {
				maxdist = dist; imax = k;
			}

			if (DEBUG_RAYCAST) {
				dyn_arr_GameElement_check(&gst->gameElements, 1 + numviz);
				gst->gameElements.arr[1 + numviz].mesh = gst->meshes[1];
				for (int i = 0; i < 3; gst->gameElements.arr[1 + numviz].pos[i] = hits[k][i], i++);
				for (int i = 0; i < 3; gst->gameElements.arr[1 + numviz].scale[i] = 0.1f, i++);
				for (int i = 0; i < 4; gst->gameElements.arr[1 + numviz].rot[i] = 0, i++);
				gst->gameElements.arr[1 + numviz].rot[3] = 1;
				float color0[4] = { 1.0f, 1.0f, 0.0f, 1.0f };
				for (int i = 0; i < 4; gst->gameElements.arr[1 + numviz].color[i] = color0[i], i++);
				numviz++;
			}
		}
		if (DEBUG_RAYCAST) {
			dyn_arr_GameElement_check(&gst->gameElements, 1 + numviz);
			gst->gameElements.arr[1 + numviz].mesh = gst->meshes[1];
			for (int i = 0; i < 3; gst->gameElements.arr[1 + numviz].pos[i] = hits[imax][i], i++);
			for (int i = 0; i < 3; gst->gameElements.arr[1 + numviz].scale[i] = 0.12f, i++);
			for (int i = 0; i < 4; gst->gameElements.arr[1 + numviz].rot[i] = 0, i++);
			gst->gameElements.arr[1 + numviz].rot[3] = 1;
			float color1[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
			for (int i = 0; i < 4; gst->gameElements.arr[1 + numviz].color[i] = color1[i], i++);
		}
		for (int k = 0; k < 3; k++) out[k] = hits[imax][k];
		return sqrtf(maxdist);
	}
	return 100000000.0f;
}