#ifndef modelh
#define modelh

#include "linmath.h"
#include "dynamic_array.h"

typedef struct {
	vec3 pos;
	vec3 norm;
	vec2 tex;
} Vertex;

DYNAMIC_ARRAY_DEFINE(Vertex);

typedef struct {
	char *name;
	unsigned int VAO;
	unsigned int VBO;
	dyn_arr_Vertex verts; 
	dyn_arr_int indexes;
} Mesh;

Mesh* model_load (char *filename);

#endif