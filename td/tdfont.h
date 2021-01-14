#ifndef tdfonth
#define tdfonth

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "linmath.h"

typedef struct fcharacter {
	GLuint     TextureID;  // ID handle of the glyph texture
	vec2       Size;       // Size of glyph
	vec2       Bearing;    // Offset from baseline to left/top of glyph
	GLuint     Advance;    // Offset to advance to next glyph
} Fcharacter;

typedef struct tdf {
	Fcharacter fchars[128];
	GLuint VAO,VBO;
} Tdf;

void tdfont_init(Tdf* tdf);
void tdfont_render_text(Tdf* tdf,int shader,char* text,GLfloat x,GLfloat y,GLfloat scale,
	vec3 color,vec2 res);

#endif