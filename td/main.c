//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")


#include <stdio.h>
#include <math.h>

#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

enum bool { false, true };

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


int main()
{
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

	// vaos, vbos, ebos (models)
	Mesh *t = model_load("test.obj");
	Mesh *t2 = model_load("cube2.obj");

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

    // render loop
    // -----------
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
        game_render(window, shaderProgram, shaderTerrain, &gst, t, t2);

        glfwSwapBuffers(window);
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();

	gameaudio_quit();

    return 0;
}