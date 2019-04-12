#ifndef gameinputh
#define gameinputh

typedef struct gameInput {
	double xpos;
	double ypos;
	double last_xpos;
	double last_ypos;
	int first_mouse_flag;
	int mousepress[3];
	int keypress[4];
} GameInput;

void input_init (GLFWwindow *window, GameInput *com);
void input_process (GLFWwindow *window, GameInput *com);

#endif