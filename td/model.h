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
	dyn_arr_Vertex verts; 
	int trigon_num;
} Mesh;

Mesh* model_load (char *filename);

#endif