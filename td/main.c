//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")


#include <stdio.h>
#include <math.h>

#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "tdgraphics.h"
#include "linmath.h"
#include "dataframe.h"
#include "tdaudio.h"
#include "tdprocess.h"
#include "tdinput.h"
#include "tdrender.h"
#include "tdshader.h"
#include "tdmodel.h"
#include "tdwavelib.h"
#include "tdfont.h"

#define dIDESINGLE
#include <ode/ode.h>

#define true 1
#define false 0

typedef struct {
	dWorldID world;
	dJointGroupID contactgroup;
} ode_callbackdata;

static void nearCallback(void* data,dGeomID o1,dGeomID o2) {
	ode_callbackdata *ds=(ode_callbackdata*)data;
	dContact contact[10];
	int numc=dCollide(o1,o2,1,&contact[0].geom,sizeof(dContact)); 
	for (int i=0; i<numc; i++) {
		contact[i].surface.mode=dContactBounce;
		contact[i].surface.mu=dInfinity;
		contact[i].surface.bounce=0.0; // (0.0~1.0) restitution parameter
		contact[i].surface.bounce_vel=0.0; // minimum incoming velocity for bounce
		dJointID c=dJointCreateContact(ds->world,ds->contactgroup,&contact[i]);
		dJointAttach(c,dGeomGetBody(contact[i].geom.g1),dGeomGetBody(contact[i].geom.g2));
	}
}

int main() {
	// gameaudio
	GameAudioData ga_data;
	gameaudio_init(&ga_data);
	ga_data.volume=1.0f;

	// glfw
	WindowOpt opt;
	opt.scr_width=1200;
	opt.scr_height=700;

	GLFWwindow* window=NULL;
	init_graphics(&window, opt.scr_width, opt.scr_height);

	// shaders
	int shaderProgram=0;
	init_shader(&shaderProgram,"vertex.glsl","fragment.glsl");
	int shaderFont=0;
	init_shader(&shaderFont,"font_vertex.glsl","font_fragment.glsl");

	stbi_set_flip_vertically_on_load(1);
	int width,height,nrChannels;
	unsigned char* data=stbi_load("imgs/test.png",&width,&height,&nrChannels,0);
	unsigned int texture;
	glGenTextures(1,&texture);
	glBindTexture(GL_TEXTURE_2D,texture);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,data);
	glGenerateMipmap(GL_TEXTURE_2D);
	// raw data cleanup
	stbi_image_free(data);

	// font 
	Tdf tdf;
	tdfont_init(&tdf);

	GameInput com;
	input_init(window,&com);

	WindowPtr ptr;
	ptr.opt=&opt;
	ptr.com=&com;
	glfwSetWindowUserPointer(window,&ptr);

	GameState gst;
	game_init(window,&gst);
	gst.meshes[0]=model_load("plane.obj");
	gst.meshes[1]=model_load("cube.obj");

	dyn_arr_GameElement_init(&gst.gameElements);
	dyn_arr_GameElement_check(&gst.gameElements,0);
	gst.gameElements.arr[0].mesh=gst.meshes[0];
	for (int i=0; i<3; gst.gameElements.arr[0].pos[i]=0,i++);
	gst.gameElements.arr[0].pos[0]=0;
	gst.gameElements.arr[0].pos[1]=0;
	gst.gameElements.arr[0].pos[2]=-5;
	for (int i=0; i<3; gst.gameElements.arr[0].scale[i]=1,i++);
	//float rot[3]={0,0,-M_PI_2};
	float rot[3]={0,0,0};
	euler_to_quat(gst.gameElements.arr[0].rot,rot);
	float color0[4]={1.0f,1.0f,1.0f,1.0f};
	for (int i=0; i<4; gst.gameElements.arr[0].color[i]=color0[i],i++);

	dInitODE2(0);
	dWorldID world = dWorldCreate();
	dJointGroupID contactgroup=dJointGroupCreate(0);
	dSpaceID space = dHashSpaceCreate(0);
	dWorldSetGravity(world,0,0,-9.8);
	dWorldSetQuickStepNumIterations(world,64);

	//dGeomID ground=dCreatePlane(space,0,0,1,0);
	
	dTriMeshDataID Data=dGeomTriMeshDataCreate();
	float vs[1024];
	model_verts_flat(vs, gst.meshes[0]);
	dGeomTriMeshDataBuildSingle
	(
		Data,
		vs,
		3*sizeof(float),
		gst.meshes[0]->verts.cur,
		gst.meshes[0]->indexes.arr,
		gst.meshes[0]->indexes.cur,
		3*sizeof(dTriIndex)
	);
	dMatrix3 R;
	dGeomID world_mesh=dCreateTriMesh(space,Data,0,0,0);
	dGeomTriMeshEnableTC(world_mesh,dSphereClass,false);
	dGeomTriMeshEnableTC(world_mesh,dBoxClass,false);
	dGeomSetPosition(world_mesh,0,0,-5);
	dRSetIdentity(R);
	//dRFromAxisAndAngle(R,0,0,1,-M_PI);
	dGeomSetRotation(world_mesh,R);


	dyn_arr_GameElement_check(&gst.gameElements, 1);
	gst.gameElements.arr[1].mesh = gst.meshes[1];
	for (int i = 0; i < 3; gst.gameElements.arr[1].pos[i] = 0, i++);
	for (int i = 0; i < 3; gst.gameElements.arr[1].scale[i] = 1, i++);
	for (int i = 0; i < 4; gst.gameElements.arr[1].rot[i] = 0, i++);
	gst.gameElements.arr[1].rot[3]=1;
	float color1[4] = { 0.0f, 0.5f, 1.0f, 1.0f };
	for (int i = 0; i < 4; gst.gameElements.arr[1].color[i] = color1[i], i++);

	dTriMeshDataID Data1=dGeomTriMeshDataCreate();
	float vs1[1024];
	model_verts_flat(vs1,gst.meshes[1]);
	dGeomTriMeshDataBuildSingle
	(
		Data1,
		vs1,
		3*sizeof(float),
		gst.meshes[1]->verts.cur,
		gst.meshes[1]->indexes.arr,
		gst.meshes[1]->indexes.cur,
		3*sizeof(dTriIndex)
	);

	dMass m;
	dBodyID sphbody=dBodyCreate(world);
	dMassSetSphere(&m,1,1);
	dBodySetMass(sphbody,&m);
	dGeomID sphgeom=dCreateTriMesh(space,Data1,0,0,0);
	dGeomSetBody(sphgeom,sphbody);  
	dQuaternion q;
	dQSetIdentity(q);
	dBodySetPosition(sphbody,0,0,0);
	dBodySetQuaternion(sphbody,q);
	dBodySetLinearVel(sphbody,0,0,0);
	dBodySetAngularVel(sphbody,0,0,0);
	
    // render loop
    // -----------
	ptr.last_frame = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
		float current_frame = glfwGetTime();
		ptr.delta_time = current_frame - ptr.last_frame;
		ptr.last_frame = current_frame;  

        // input
        glfwPollEvents();
		input_process(window, &com);

		// physics
		float *pos = dBodyGetPosition(sphbody);
		gst.gameElements.arr[1].pos[0]=pos[0];
		gst.gameElements.arr[1].pos[1]=pos[1];
		gst.gameElements.arr[1].pos[2]=pos[2];
		float* rot = dBodyGetQuaternion(sphbody);
		gst.gameElements.arr[1].rot[0]=rot[0];
		gst.gameElements.arr[1].rot[1]=rot[1];
		gst.gameElements.arr[1].rot[2]=rot[2];
		gst.gameElements.arr[1].rot[3]=rot[3];
		ode_callbackdata data={ world, contactgroup };
		dSpaceCollide(space,&data,&nearCallback);
		dWorldQuickStep(world,0.01);
		dJointGroupEmpty(contactgroup);

		// process
		game_process (window, &gst, &com);

        // render
        game_render(window, shaderProgram, shaderFont, &gst, texture, &tdf, &com);

        glfwSwapBuffers(window);

    }

    glfwTerminate();
	gameaudio_quit();

    return 0;
}

