//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include <stdio.h>
#include <math.h>

#include <stdlib.h>
#include <time.h>

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
	// TEST: read wav
	GameAudioSamples *clip0 = wave_read("bloop.wav");
	GameAudioSamples *clip1 = wave_read("song.wav");

	// gameaudio
	GameAudioData ga_data;
	gameaudio_init(&ga_data);
	gameaudio_add(&ga_data, clip0, 0);
	gameaudio_add(&ga_data, clip1, -1);
	ga_data.volume = 1.0f;
	ga_data.clip[0]->volume = 0;

	// glfw
	GLFWwindow *window = NULL;
	init_graphics(&window);

	// shaders
    int shaderProgram = 0;
	init_shader(&shaderProgram, "vertex.glsl", "fragment.glsl");

	// vaos, vbos, ebos (models)
	model_bind_rect ();

	srand ( time(NULL) );

	WindowOpt opt;
	opt.scr_width = 900;
	opt.scr_height = 600;
	glfwSetWindowUserPointer (window, &opt);

	GameState gst;
	game_init (window, &gst);

	GameInput com;
	input_init(window, &com);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // input
        glfwPollEvents();
		input_process(window, &com);

		// portaudio test
		if (com.mousepress[0] == 1) {
			gameaudio_play(&ga_data, 0);
		} else if (com.mousepress[0] == -1) {
			gameaudio_stop(&ga_data, 0);
		}
		// process
		game_process (window, &gst, &com);

        // render
        game_render(window, shaderProgram, &gst);

        glfwSwapBuffers(window);
    }

	/*
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
	*/

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();

	gameaudio_quit();

    return 0;
}