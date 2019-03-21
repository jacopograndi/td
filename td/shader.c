#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <direct.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

const char * load_shader (char name[]) {
	// construct cwd full path
	char *buffer = _getcwd( NULL, 0 );
	char fullpath[128]; int i;
	for (i=0; buffer[i] != '\0'; fullpath[i] = buffer[i], i++);
	fullpath[i] = '\\'; i++; 
	for (int s=0; s<7; fullpath[i]="shaders"[s], i++, s++);
	fullpath[i] = '\\'; i++; 
	for (int j=0; name[j] != '\0'; fullpath[i] = name[j], i++, j++);
	fullpath[i] = '\0';

	FILE *file = fopen(fullpath, "r");
	if (file == NULL) { printf("wrong filename\n"); return NULL; }

	char *b = malloc(1024*16);
	char rch; int index = 0;
	while ((rch = fgetc(file)) != EOF) {
		b[index] = rch; index++;
	} 
	b[index] = '\0';
	fclose(file);
	return b; 
}

void init_shader (int *shaderProgram, char *vx, char *fm) {
	// build and compile our shader program
    // ------------------------------------
    // vertex shader
	const char *buf = load_shader(vx);
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &buf, NULL);
	free((char*)buf);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n");
		printf("%s\n", infoLog);
    }
    // fragment shader
	buf = load_shader(fm);
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &buf, NULL);
	free((char*)buf);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n");
		printf("%s\n", infoLog);
    }
    // link shaders
    *shaderProgram = glCreateProgram();
    glAttachShader(*shaderProgram, vertexShader);
    glAttachShader(*shaderProgram, fragmentShader);
    glLinkProgram(*shaderProgram);
    // check for linking errors
    glGetProgramiv(*shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(*shaderProgram, 512, NULL, infoLog);
		printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n");
		printf("%s\n", infoLog);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}