//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")


#include <stdio.h>
#include <math.h>

#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <ft2build.h>
#include FT_FREETYPE_H  

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "graphics.h"
#include "linmath.h"
#include "dataframe.h"
#include "gameaudio.h"
#include "gameprocess.h"
#include "gameinput.h"
#include "gamerender.h"
#include "shader.h"
#include "model.h"
#include "wavelib.h"


int main() {
	// font
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		printf("ERROR::FREETYPE: Could not init FreeType Library\n");
	}
	FT_Face face;
	if (FT_New_Face(ft, "fonts/OpenSans-Light.ttf", 0, &face)) {
		printf("ERROR::FREETYPE: Failed to load font\n");
	}
	FT_Set_Pixel_Sizes(face, 0, 48);  
	if (FT_Load_Char(face, 'X', FT_LOAD_RENDER)) {
		printf("ERROR::FREETYTPE: Failed to load Glyph\n");
	}

	// gameaudio
	GameAudioData ga_data;
	gameaudio_init(&ga_data);
	ga_data.volume = 1.0f;

	// glfw
	GLFWwindow *window = NULL;
	init_graphics(&window);
	
	// shaders
    int shaderProgram = 0, shaderTerrain = 0;
	init_shader(&shaderProgram, "vertex.glsl", "fragment.glsl");

	int width, height, nrChannels;
	unsigned char *data = stbi_load("imgs/test.png", &width, &height, &nrChannels, 0);
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	// raw data cleanup
	stbi_image_free(data);

	WindowOpt opt;
	opt.scr_width = 900;
	opt.scr_height = 600;

	GameInput com;
	input_init(window, &com);

	WindowPtr ptr;
	ptr.opt = &opt;
	ptr.com = &com;
	glfwSetWindowUserPointer (window, &ptr);

	GameState gst;
	game_init (window, &gst);
	gst.meshes[0] = model_load("playground.obj");;
	gst.meshes[1] = model_load("cross.obj");;

	dyn_arr_GameElement_init(&gst.gameElements);
	dyn_arr_GameElement_check(&gst.gameElements, 0);
	gst.gameElements.arr[0].mesh = gst.meshes[0];
	for (int i = 0; i < 3; gst.gameElements.arr[0].pos[i] = 0, i++);
	for (int i = 0; i < 3; gst.gameElements.arr[0].scale[i] = 1, i++);
	for (int i = 0; i < 4; gst.gameElements.arr[0].rot[i] = 0, i++);
	gst.gameElements.arr[0].rot[3] = 1;
	float color0[4] = { 1.0f, 0.5f, 0.0f, 1.0f };
	for (int i = 0; i < 4; gst.gameElements.arr[0].color[i] = color0[i], i++);

	dyn_arr_GameElement_check(&gst.gameElements, 1);
	gst.gameElements.arr[1].mesh = gst.meshes[1];
	for (int i = 0; i < 3; gst.gameElements.arr[1].pos[i] = 0, i++);
	for (int i = 0; i < 3; gst.gameElements.arr[1].scale[i] = 0.1f, i++);
	for (int i = 0; i < 4; gst.gameElements.arr[1].rot[i] = 0, i++);
	gst.gameElements.arr[1].rot[3]=1;
	float color1[4] = { 0.0f, 0.5f, 1.0f, 1.0f };
	for (int i = 0; i < 4; gst.gameElements.arr[1].color[i] = color1[i], i++);

    // render loop
    // -----------
	ptr.last_frame = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
		float current_frame = glfwGetTime();
		ptr.delta_time = current_frame - ptr.last_frame;
		ptr.last_frame = current_frame;  

        // input
        glfwPollEvents();
		input_process(window, &com);

		// process
		game_process (window, &gst, &com);

        // render
        game_render(window, shaderProgram, shaderTerrain, &gst, texture);

        glfwSwapBuffers(window);

    }

    glfwTerminate();
	gameaudio_quit();

    return 0;
}