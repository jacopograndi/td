#ifndef dynamicarrayh
#define dynamicarrayh

#include <stdlib.h>

#define ARRAY_SIZE 1

#define DYNAMIC_ARRAY_DEFINE(type) \
typedef struct {\
	type *arr; \
	int cur; \
	int size; \
} dyn_arr_##type; \
static inline void dyn_arr_##type##_init (dyn_arr_##type *dyn) { \
	dyn->arr = malloc(sizeof(type)*ARRAY_SIZE); \
	dyn->size = ARRAY_SIZE; dyn->cur = 0; \
} \
static inline void dyn_arr_##type##_check (dyn_arr_##type *dyn, int i) { \
	if (dyn->size <= i) { \
		dyn->size *= 2; \
		dyn->arr = realloc(dyn->arr, sizeof(type)*dyn->size); \
	} \
	dyn->cur = i+1; \
} \
static inline void dyn_arr_##type##_free (dyn_arr_##type *dyn) { \
	dyn->size = 0; \
	dyn->cur = 0; \
	free(dyn->arr); \
}

DYNAMIC_ARRAY_DEFINE(int)
DYNAMIC_ARRAY_DEFINE(float)

#endif