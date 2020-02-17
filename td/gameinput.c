#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "dataframe.h"
#include "gameinput.h"

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void keyboard_callback(GLFWwindow *window)
{
	GameInput *com = ((WindowPtr*) glfwGetWindowUserPointer(window))->com;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		com->keypress[0] = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		com->keypress[1] = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		com->keypress[2] = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		com->keypress[3] = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		com->keypress[4] = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
		com->keypress[4] = 0;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE) {
		com->keypress[0] = 0;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE) {
		com->keypress[1] = 0;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE) {
		com->keypress[2] = 0;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE) {
		com->keypress[3] = 0;
	}
}

void input_init (GLFWwindow *window, GameInput *com) {
    glfwSetKeyCallback(window, keyboard_callback);
	com->first_mouse_flag = 1;
	
	for (int i=0; i<4; i++) { com->keypress[i] = 0; }
	for (int i=0; i<3; i++) { com->mousepress[i] = 0; }
} 

void input_process (GLFWwindow *window, GameInput *com) {
	glfwGetCursorPos(window, &com->xpos, &com->ypos);
	int mouse[3] = { 0, 0, 0 };
	mouse[0] = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	mouse[1] = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
	mouse[2] = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
	for (int i=0; i<3; i++) {
		if (mouse[i] == GLFW_PRESS) {
			com->mousepress[i] += 1;
		} else if (com->mousepress[i] > 0) {
			com->mousepress[i] = -1;
		} else { com->mousepress[i] = 0; }
	}
}