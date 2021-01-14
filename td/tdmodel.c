#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <direct.h>
#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "tdmodel.h"
#define DEBUG_MODEL 0

//@ ensures: transform str[start:end] in float;
float str_to_float(char *str, int start, int end) {
	float res = 0.0f; int point_tag=0; int minus_flag=0;
	if (str[start]=='-') { minus_flag=1; start++; }
	for (point_tag=start; str[point_tag]!='.'; point_tag++);
	for (int i=start; i<end; i++) {
		if (i<point_tag) {
			float dec=1; for (int k=0;k<point_tag-i-1;dec*=10,k++);
			res+=(str[i]-'0')*dec;
		}
		if (i>point_tag) {
			float dec=1; for (int k=0;k<i-point_tag;dec/=10,k++);
			res+=(str[i]-'0')*dec;
		}
	}
	if (minus_flag) res*=-1;
	return res;
}

//@ ensures: transform str[start:end] in uint;
int str_to_int (char *str, int start, int end) {
	int res = 0;
	for (int i=start; i<end; i++) {
		int dec=1; for (int k=0;k<end-i-1;dec*=10,k++);
		res+=(str[i]-'0')*dec;
	}
	return res;
}

Mesh* model_load (char *filename) {
	// construct cwd full path
	char *buffer = _getcwd( NULL, 0 );
	char fullpath[128]; int i;
	for (i=0; buffer[i] != '\0'; fullpath[i] = buffer[i], i++);
	fullpath[i] = '\\'; i++; 
	for (int s=0; s<6; fullpath[i]="models"[s], i++, s++);
	fullpath[i] = '\\'; i++; 
	for (int j=0; filename[j] != '\0'; fullpath[i] = filename[j], i++, j++);
	fullpath[i] = '\0';

	FILE *file = fopen(fullpath, "r");
	if (file == NULL) { printf("%s, wrong filename\n", fullpath); return NULL; }

	int v_i=0, f_i=0, vn_i=0, ni_i=0, vt_i=0, ti_i=0;
	Mesh *mesh = malloc(sizeof(Mesh));

	dyn_arr_float poss, norm, texs;
	dyn_arr_int faces, normindexes, texsindexes;
	dyn_arr_int_init(&(faces));
	dyn_arr_int_init(&(normindexes));
	dyn_arr_int_init(&(texsindexes));
	dyn_arr_float_init(&(poss));
	dyn_arr_float_init(&(norm));
	dyn_arr_float_init(&(texs));

	char rch; char line[1024]; int index = 0;
	int marker_tag = 0;
	while ((rch = fgetc(file)) != EOF) {
		if (rch == '\n') {
			for (marker_tag=0;line[marker_tag]!=' ';marker_tag++);
			if (marker_tag == 1 && line[0]=='o') { // name
				int namesize = index-marker_tag;
				mesh->name = malloc(sizeof(char)*namesize);
				for (int c=0; c<namesize; mesh->name[c]=line[2+c], c++);
				mesh->name[namesize-1] = '\0';
			}
			if (marker_tag == 1 && line[0]=='v') { // vertex
				int tag[5];
				for (int i=marker_tag+1, j=0; i<=index; i++) {
					if (line[i] == ' ' && j<2) { tag[j]=i; j++; }
				}
				float vx = str_to_float(line, 2, tag[0]);
				float vy = str_to_float(line, tag[0]+1, tag[1]);
				float vz = str_to_float(line, tag[1]+1, index);
				dyn_arr_float_check(&(poss), v_i);
				poss.arr[v_i] = vx; v_i++;
				dyn_arr_float_check(&(poss), v_i);
				poss.arr[v_i] = vy; v_i++;
				dyn_arr_float_check(&(poss), v_i);
				poss.arr[v_i] = vz; v_i++;
				if (DEBUG_MODEL) {
					char str0[20], str1[20], str2[20]; int k;
					for (k=0; k<tag[0]-2; str0[k]=line[k+2], k++); str0[k++]='\0';
					for (k=0; k<tag[1]-tag[0]-1; str1[k]=line[k+tag[0]+1], k++); str1[k++]='\0';
					for (k=0; k<index-tag[1]-1; str2[k]=line[k+tag[1]+1], k++); str2[k++]='\0';
					printf("v= %s, %s, %s\n", str0, str1, str2);
					printf("-> %f, %f, %f\n\n", vx, vy, vz);
				}
			}
			if (marker_tag == 2 && line[0] == 'v' && line[1] == 't') { // vertex textures
				int tag[5];
				for (int i=marker_tag+1,j=0; i<=index; i++) {
					if (line[i]==' '&&j<1) { tag[j]=i; j++; }
				}
				float vx = str_to_float(line, 3, tag[0]);
				float vy = str_to_float(line, tag[0] + 1, index);
				dyn_arr_float_check(&(texs), vt_i);
				texs.arr[vt_i] = vx; vt_i++;
				dyn_arr_float_check(&(texs), vt_i);
				texs.arr[vt_i] = vy; vt_i++;
				if (DEBUG_MODEL) {
					line[index]='\0';
					printf("tag0= %d, index= %d, %s\n",tag[0], index, line);
					char str0[20],str1[20]; int k;
					for (k=0; k<tag[0]-2; str0[k]=line[k+3],k++); str0[k++]='\0';
					for (k=0; k<index-tag[0]-1; str1[k]=line[k+tag[0]+1],k++); str1[k++]='\0';
					printf("t= %s, %s\n", str0, str1);
					printf("-> %f, %f\n\n", vx, vy);
				}
			}
			if (marker_tag == 2 && line[0]=='v' && line[1]=='n') { // normal
				int tag[5];
				for (int i=marker_tag+1, j=0; i<=index; i++) {
					if (line[i] == ' ' && j<2) { tag[j]=i; j++; }
				}
				float vx = str_to_float(line, 3, tag[0]);
				float vy = str_to_float(line, tag[0]+1, tag[1]);
				float vz = str_to_float(line, tag[1]+1, index);
				dyn_arr_float_check(&(norm), vn_i);
				norm.arr[vn_i] = vx; vn_i++;
				dyn_arr_float_check(&(norm), vn_i);
				norm.arr[vn_i] = vy; vn_i++;
				dyn_arr_float_check(&(norm), vn_i);
				norm.arr[vn_i] = vz; vn_i++;
				if (DEBUG_MODEL) {
					char str0[20], str1[20], str2[20]; int k;
					for (k=0; k<tag[0]-2; str0[k]=line[k+3], k++); str0[k++]='\0';
					for (k=0; k<tag[1]-tag[0]-1; str1[k]=line[k+tag[0]+1], k++); str1[k++]='\0';
					for (k=0; k<index-tag[1]-1; str2[k]=line[k+tag[1]+1], k++); str2[k++]='\0';
					printf("n= %s, %s, %s\n", str0, str1, str2);
					printf("-> %f, %f, %f\n\n", vx, vy, vz);
				}
			}
			if (marker_tag == 1 && line[0]=='f') { // face
				int tag[5], tagslash[4][2], tagnum=0;
				for (int i=0; i<=index; i++) {
					if (line[i] == ' ' && tagnum<4) { 
						tag[tagnum]=i; tagnum++; 
					}
				} tag[tagnum]=index; tagnum++;
				for (int i=0; i<tagnum-1; i++) {
					int tagsn=0;
					for (int k=tag[i]; k<tag[i+1]; k++) {
						if (line[k] == '/') {
							tagslash[i][tagsn] = k; tagsn++;
						}
					}
				}
				for (int i=0; i<tagnum-1; i++) {
					dyn_arr_int_check(&(faces), f_i);
					faces.arr[f_i] = str_to_int(line, tag[i]+1, tagslash[i][0])-1; f_i++;
					dyn_arr_int_check(&(texsindexes),ti_i);
					texsindexes.arr[ti_i]=str_to_int(line,tagslash[i][0]+1,tagslash[i][1])-1; ti_i++;
					dyn_arr_int_check(&(normindexes), ni_i);
					normindexes.arr[ni_i] = str_to_int(line, tagslash[i][1]+1, tag[i+1])-1; ni_i++;
					if (DEBUG_MODEL) {
						line[index]='\0';
						printf("%s\n", line);
						char str0[10]; int k;
						for (k=0; k<tagslash[i][0]-tag[i]-1; str0[k]=line[k+tag[i]+1], k++); str0[k++]='\0';
						printf("f =%s->%d\n", str0, faces.arr[f_i-1]);
						char str1[20];
						for (k=0; k<tag[i+1]-tagslash[i][1]-1; str1[k]=line[k+tagslash[i][1]+1], k++); str1[k++]='\0';
						printf("fn=%s->%d, %d,%d\n", str1, normindexes.arr[f_i-1], tag[i+1], tagslash[i][1]);
						char str2[20]; k=0;
						for (k=0; k<tagslash[i][0]-tagslash[i][1]-1; str2[k]=line[k+tagslash[i][0]+1],k++); str2[k++]='\0';
						printf("t =%s->%d, %d,%d\n",str2,texsindexes.arr[f_i-1],tagslash[i][0],tagslash[i][1]);
						printf("\n");
					}
				}
				if (DEBUG_MODEL) { printf("\n"); }
			}
			index=0;
		} else {
			line[index] = rch; index++;
		}
	} 
	fclose(file);	
	
	if (DEBUG_MODEL) {
		printf("poss cur: %d\n", poss.cur);
		printf("norm cur: %d\n",norm.cur);
		printf("normindexes cur: %d\n",normindexes.cur);
		printf("faces cur: %d\n", faces.cur);
		printf("texs cur: %d\n", texs.cur);
	}

	// construct vertex array
	dyn_arr_int_init(&(mesh->indexes));

	dyn_arr_Vertex_init(&(mesh->verts));
	for (int i=0; i<faces.cur; i++) {
		if (DEBUG_MODEL) {
			printf("i=%d, normindexes[i]=%d\n",i,normindexes.arr[i]);
			printf("i=%d, faces[i]=%d\n", i, faces.arr[i]);
			for (int j=0;j<3;j++) printf("%f,", poss.arr[faces.arr[i]*3+j]);
		}
		dyn_arr_int_check(&(mesh->indexes), i);
		mesh->indexes.arr[i] = i;
		dyn_arr_Vertex_check(&(mesh->verts), i);
		for (int j=0; j<3; j++) mesh->verts.arr[i].pos[j]=poss.arr[faces.arr[i]*3+j];
		for (int j=0; j<3; j++) mesh->verts.arr[i].norm[j]=norm.arr[normindexes.arr[i]*3+j];
		for (int j=0; j<2; j++) mesh->verts.arr[i].tex[j]=texs.arr[texsindexes.arr[i]*2+j];
		if (DEBUG_MODEL) {
			printf("\n");
		}
	}
	
	if (DEBUG_MODEL) { 
		printf("verts cur: %d\n", mesh->verts.cur);
	}

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*mesh->verts.cur, mesh->verts.arr, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indexes.cur * sizeof(unsigned int), mesh->indexes.arr, GL_STATIC_DRAW);

	// vertex positions
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, norm));
    // vertex texture coords
    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex));

    glBindVertexArray(VAO);
	mesh->VAO = VAO;
	mesh->VBO = VBO;
	return mesh; 
}

void model_from_heightmap (int **hmap) {
	float * vertices = malloc(sizeof(float)*1024*1024*3);
	for (int y=0; y<1024; y++) for (int x=0; x<1024; x++) {
		vertices[(y*1024+x)*3] = x;
		vertices[(y*1024+x)*3+1] = y;
		vertices[(y*1024+x)*3+2] = hmap[x][y];
	}
}


void model_verts_flat(float *vs, Mesh* mesh) {
	for (int i=0; i<mesh->verts.cur; i++) {
		vs[i*3] = mesh->verts.arr[i].pos[0];
		vs[i*3+1] = mesh->verts.arr[i].pos[1];
		vs[i*3+2] = mesh->verts.arr[i].pos[2];
	}
}