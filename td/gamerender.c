#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "linmath.h"
#include "dataframe.h"
#include "model.h"

void mat_from_transform (mat4x4 mat, vec3 pos, vec3 scale, quat rot) {
	mat4x4 mat_trans;
	mat4x4_identity(mat_trans);
	mat4x4_translate(mat_trans, pos[0], pos[1], pos[2]);
	
	mat4x4 mat_scale;
	mat4x4_identity(mat_scale);
	mat_scale[0][0] = scale[0];
	mat_scale[1][1] = scale[1];
	mat_scale[2][2] = scale[2];

	mat4x4 mat_rot;
	mat4x4_from_quat(mat_rot, rot);

	// V = S*R*T = S*(R*T)
	mat4x4_mul(mat, mat_trans, mat_rot);	
	mat4x4_mul(mat, mat, mat_scale);	
}

void render_mesh (int shaderProgram, Mesh *mesh, mat4x4 mat_persp, mat4x4 cam, 
	vec3 vec_trans, vec3 vec_scale, quat rot, vec4 color, vec3 light) 
{
	mat4x4 mat_view;
	mat_from_transform(mat_view, vec_trans, vec_scale, rot);

	int projectionLocation = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, (const GLfloat*) mat_persp);
	int model_viewLocation = glGetUniformLocation(shaderProgram, "model_view");
	glUniformMatrix4fv(model_viewLocation, 1, GL_FALSE, (const GLfloat*) mat_view);
	int camLocation = glGetUniformLocation(shaderProgram, "cam");
	glUniformMatrix4fv(camLocation, 1, GL_FALSE, (const GLfloat*) cam);
	int objectColorLocation = glGetUniformLocation(shaderProgram, "objectColor");
	glUniform3f(objectColorLocation, color[0], color[1], color[2]);
	int lightColorLocation = glGetUniformLocation(shaderProgram, "lightColor");
	glUniform3f(lightColorLocation, 1, 1, 1);
	int lightPosLocation = glGetUniformLocation(shaderProgram, "lightPos");
	glUniform3f(lightPosLocation, light[0], light[1], light[2]);
	
    glBindVertexArray(mesh->VAO);

	glDrawElements(GL_TRIANGLES, mesh->indexes.cur, GL_UNSIGNED_INT, 0);
}

void game_render (GLFWwindow *window, int shader, int shaderterrain, GameState *gst) {
	WindowOpt *opt = ((WindowPtr*) glfwGetWindowUserPointer(window))->opt;

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
	glDepthFunc(GL_LESS);  

    // be sure to activate the shader before any acalls to glUniform
    glUseProgram(shader);
		
	mat4x4 mat_persp;
	mat4x4_perspective(mat_persp, 1.0f, (float)opt->scr_width/opt->scr_height, 0.1f, 1000.0f);

	mat4x4 cam;
	vec3 center; vec3_add(center, gst->cam_pos, gst->cam_forward);
	mat4x4_look_at(cam, gst->cam_pos, center, gst->cam_up);
	
	for (int i=0; i<gst->gameElements.cur; i++) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		render_mesh(shader, gst->gameElements.arr[i].mesh, mat_persp, cam, 
			gst->gameElements.arr[i].pos, gst->gameElements.arr[i].scale, 
			gst->gameElements.arr[i].rot, gst->gameElements.arr[i].color, gst->light_pos);
		/*
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		float _color[4] = { 0.5f, 0.0f, 0.0f, 1.0f };
		render_mesh(shader, t, mat_persp, cam, vec_trans, vec_scale, rot, _color);*/
	}
}