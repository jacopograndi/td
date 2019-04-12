#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "dataframe.h" 

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
	WindowOpt *opt = ((WindowPtr*) glfwGetWindowUserPointer(window))->opt;
	opt->scr_width = width;
	opt->scr_height = height;
}



int init_graphics(GLFWwindow** window) {
	// glfw: initialize and configure
    // ------------------------------
    glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 16);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint( GLFW_REFRESH_RATE, 60 );

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    *window = glfwCreateWindow(900, 600, 
		"td"
		, NULL, NULL
	);
    if (*window == NULL)
    {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf( "Failed to initialize GLAD\n");
        return -1;
    }

	glEnable(GL_MULTISAMPLE); 
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	return 0;
}