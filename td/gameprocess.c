#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "dataframe.h"
#include "gameinput.h"

void game_init (GLFWwindow* window, GameState *gst) {
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	gst->cam_pos[0] = 0; gst->cam_pos[1] = 0; gst->cam_pos[2] = 0;
	gst->cam_forward[0] = 0; gst->cam_forward[1] = 0; gst->cam_forward[2] = 1;
	gst->cam_up[0] = 0; gst->cam_up[1] = 1; gst->cam_up[2] = 0;
	gst->cam_pitch = 0; gst->cam_yaw = -90;
	/*
	gst->terrain.hmap = malloc(sizeof(float)*1024*1024);
	for (int y = 0; y<1024; y++) {
		for (int x = 0; x<1024; x++) {
			gst->terrain.hmap[y*1024+x] = 0;
		}
	}*/

	gst->light_pos[0] = 0; gst->light_pos[1] = 1; gst->light_pos[2] = 0; 
	dyn_arr_GameElement_init(&gst->gameElements);
}

void game_process (GLFWwindow* window, GameState *gst, GameInput *com) {
	WindowPtr *ptr = (WindowPtr*) glfwGetWindowUserPointer(window);
	WindowOpt *opt = ptr->opt;

	float time = glfwGetTime();
	gst->light_pos[0] = cos(time)*3;
	gst->light_pos[2] = sin(time)*3;

	float cam_speed = ptr->delta_time * 6;

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

    float sensitivity = ptr->delta_time * 20;
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

	if (com->keypress[1]) {
		vec3 dp; vec3_mul_cross(dp, gst->cam_forward, gst->cam_up);
		vec3_norm(dp, dp); vec3_scale(dp, dp, cam_speed);
		vec3_sub(gst->cam_pos, gst->cam_pos, dp);
	}
	if (com->keypress[3]) {
		vec3 dp; vec3_mul_cross(dp, gst->cam_forward, gst->cam_up);
		vec3_norm(dp, dp); vec3_scale(dp, dp, cam_speed);
		vec3_add(gst->cam_pos, gst->cam_pos, dp);
	}
	if (com->keypress[0]) {
		vec3 dp; vec3_scale(dp, gst->cam_forward, cam_speed);
		vec3_add(gst->cam_pos, gst->cam_pos, dp);
	}
	if (com->keypress[2]) {
		vec3 dp; vec3_scale(dp, gst->cam_forward, cam_speed);
		vec3_sub(gst->cam_pos, gst->cam_pos, dp);
	}

	if (com->mousepress[0] == 1 ||1) {
		vec3 outPoint; Mesh *mesh; int hitFlag = 0;
		vec3 normCam;  vec3_norm(normCam, gst->cam_forward);
		vec3 hits[128]; int hitsnum = 0;
		for (int i=0; i<1; i++) { // check only the first element
			mesh = gst->gameElements.arr[i].mesh;
			for (int j=0; j<mesh->verts.cur; j+=3) {
				hitFlag = raytrace_trigon(gst->cam_pos, normCam,
					mesh->verts.arr[j].pos, mesh->verts.arr[j+1].pos, mesh->verts.arr[j+2].pos,
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
				dyn_arr_GameElement_check(&gst->gameElements, 1 + numviz);
				gst->gameElements.arr[1+numviz].mesh = gst->meshes[1];
				for (int i = 0; i < 3; gst->gameElements.arr[1 + numviz].pos[i] = hits[k][i], i++);
				for (int i = 0; i < 3; gst->gameElements.arr[1 + numviz].scale[i] = 0.1f, i++);
				for (int i = 0; i < 4; gst->gameElements.arr[1 + numviz].rot[i] = 0, i++);
				gst->gameElements.arr[1 + numviz].rot[3] = 1;
				float color0[4] = { 1.0f, 1.0f, 0.0f, 1.0f };
				for (int i = 0; i < 4; gst->gameElements.arr[1 + numviz].color[i] = color0[i], i++);
				numviz++;
			}
			dyn_arr_GameElement_check(&gst->gameElements, 1 + numviz);
			gst->gameElements.arr[1 + numviz].mesh = gst->meshes[1];
			for (int i = 0; i < 3; gst->gameElements.arr[1 + numviz].pos[i] = hits[imax][i], i++);
			for (int i = 0; i < 3; gst->gameElements.arr[1 + numviz].scale[i] = 0.08f, i++);
			for (int i = 0; i < 4; gst->gameElements.arr[1 + numviz].rot[i] = 0, i++);
			gst->gameElements.arr[1 + numviz].rot[3] = 1;
			float color1[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
			for (int i = 0; i < 4; gst->gameElements.arr[1 + numviz].color[i] = color1[i], i++);

			//for (int k = 0; k < 3; k++) gst->gameElements.arr[1].pos[k] = hits[imax][k];
		}
	}
}