#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "gameinput.h"

void input_init (GLFWwindow *window, GameInput *com) {
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