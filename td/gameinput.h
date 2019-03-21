#ifndef gameinputh
#define gameinputh

typedef struct gameInput {
	double xpos;
	double ypos;
	int mousepress[3];
} GameInput;

void input_init (GLFWwindow *window, GameInput *com);
void input_process (GLFWwindow *window, GameInput *com);

#endif