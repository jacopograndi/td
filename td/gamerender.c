#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "linmath.h"
#include "dataframe.h"

void render_quad (int shaderProgram, mat4x4 mat_ortho, 
	vec3 vec_trans, vec3 vec_scale, quat rot, vec4 color) 
{
	mat4x4 mat_trans;
	mat4x4_identity(mat_trans);
	mat4x4_translate(mat_trans, vec_trans[0], vec_trans[1], vec_trans[2]);
	
	mat4x4 mat_scale;
	vec3 vec_one = { 1, 1, 1 };
	mat4x4_identity(mat_scale);
	mat_scale[0][0] = vec_scale[0];
	mat_scale[1][1] = vec_scale[1];
	mat_scale[2][2] = vec_scale[2];

	mat4x4 mat_rot;
	mat4x4_from_quat(mat_rot, rot);

	// V = S*R*T = S*(R*T)
	mat4x4 mat_view;
	mat4x4_mul(mat_view, mat_trans, mat_rot);	
	mat4x4_mul(mat_view, mat_view, mat_scale);	

	int projectionLocation = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, (const GLfloat*) mat_ortho);
	int model_viewLocation = glGetUniformLocation(shaderProgram, "model_view");
	glUniformMatrix4fv(model_viewLocation, 1, GL_FALSE, (const GLfloat*) mat_view);
	int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
	glUniform4f(vertexColorLocation, color[0], color[1], color[2], color[3]);

	// render the triangle
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void game_render (GLFWwindow *window, int shader, GameState *gst) {
	WindowOpt *opt = (WindowOpt*) glfwGetWindowUserPointer(window);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
	glDepthFunc(GL_LESS);  

    // be sure to activate the shader before any calls to glUniform
    glUseProgram(shader);
		
	mat4x4 mat_ortho;
	mat4x4_ortho(mat_ortho, 0, opt->scr_width, opt->scr_height, 0, 0, 1);
	for (int y=0; y<8; y++) {
		for (int x=0; x<8; x++) {
			int sel_flag = 0;
			int hover_flag = 0;
			int mask_flag = 0;
			if (x+y*8==gst->sel) { sel_flag = 1; }
			if (x+y*8==gst->hover) { hover_flag = 1; }
			for (int i=0; i<6; i++) {
				if (x+y*8==gst->mask[i]) { mask_flag = 1; }
			}
				
			float vec_trans[3] = {
				(float)(x-3.5)*52+10 + opt->scr_width/2, 
				(float)(y-3.5)*52+10 + opt->scr_height/2, 
				-0.5+hover_flag*0.25
			};
			float vec_scale[3] = {
				1+hover_flag*0.1, 
				1+hover_flag*0.1, 
				1
			};
			float color[4];
			switch(gst->board.cell[x][y]) {
				case 0: color[0] = 0.7f; color[1]= 0.7f; color[2] = 0.7f; break;
				case 1: color[0] = 1.0f; color[1]= 0.5f; color[2] = 0.0f; break;
				case 2: color[0] = 1.0f; color[1]= 0.0f; color[2] = 0.0f; break;
			}
			if (sel_flag) { for (int c=0; c<3; color[c]=(2+color[c])/3, c++); }
			if (hover_flag) { for (int c=0; c<3; color[c]=1, c++); }
			if (mask_flag) { for (int c=0; c<3; color[c]=(0.7+color[c])/1.7f, c++); }
			color[3] = 1.0f;

			quat rot;
			quat_identity(rot);
			if (hover_flag) { 
				quat_rotate(rot, gst->rot, (float[]){ 0, 0, 1 });
			}

			render_quad(shader, mat_ortho, vec_trans, vec_scale, rot, color);
		}
	}
	int turn_flag = 0;
	if (gst->turn%2==1) turn_flag = 1;
	float vec_trans[3] = {
		(float)(-4.5)*52 + opt->scr_width/2, 
		(float)(-3.5+7*(1-turn_flag))*52+10 + opt->scr_height/2, 
		-0.5
	};
	float vec_scale[3] = { 1, 1, 1 };
	float color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	if (turn_flag) { color[0] = 1.0f; color[1]= 0.5f; color[2] = 0.0f; }
	quat rot; quat_identity(rot); 
	render_quad(shader, mat_ortho, vec_trans, vec_scale, rot, color);
		
	for(int i=0; i<gst->score[0]; i++) {
		float vec_trans[3] = {
			(float)(4.5)*52+5 + opt->scr_width/2, 
			(float)(3.5)*52-((float)i)*26+22 + opt->scr_height/2,  
			-0.5
		};
		float vec_scale[3] = { 0.5, 0.5, 1 };
		float color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
		quat rot; quat_identity(rot); 
		render_quad(shader, mat_ortho, vec_trans, vec_scale, rot, color);
	}

	for(int i=0; i<gst->score[1]; i++) {
		float vec_trans[3] = {
			(float)(4.5)*52+5 + opt->scr_width/2, 
			(float)(-3.5)*52+((float)i)*26-2 + opt->scr_height/2, 
			-0.5
		};
		float vec_scale[3] = { 0.5, 0.5, 1 };
		float color[4] = { 1.0f, 0.5f, 0.0f, 1.0f };
		quat rot; quat_identity(rot); 
		render_quad(shader, mat_ortho, vec_trans, vec_scale, rot, color);
	}
}