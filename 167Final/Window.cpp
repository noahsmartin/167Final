//
//  Window.cpp
//  167Final
//
//  Created by Noah Martin on 12/12/14.
//  Copyright (c) 2014 Noah Martin. All rights reserved.
//

#include <iostream>
#include "Camera.h"
#include "Window.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include "GLee.h"
#include <GL/glut.h>
#endif
#include <math.h>
#include "Mountain.h"

using namespace std;

int Window::width  = 512;   // set window width in pixels here
int Window::height = 512;   // set window height in pixels here

Camera camera(Vector3(0, 0, 10), Vector3(0, 0, -1), Vector3(0, 1, 0));

Matrix4 model;

Mountain mountains[10];
bool genMountains = true;

bool loadOnce = true;

void loadOnceF() {
    model.identity();
    loadOnce = false;
}

void Window::idleCallback()
{
    if(loadOnce) {
        loadOnceF();
    }
    displayCallback();         // call display routine to show the cube
}

void Window::reshapeCallback(int w, int h)
{
    cerr << "Window::reshapeCallback called" << endl;
    glViewport(0, 0, w, h);  // set new viewport size
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    double aspect = double(width)/(double)height;
    gluPerspective(60.0, aspect, 1.0, 1000); // set perspective projection viewing frustum
    //glTranslatef(0, 0, -20);    // move camera back 20 units so that it looks at the origin (or else it's in the origin)
    glMatrixMode(GL_MODELVIEW);
}

void genMountain(Vector3* array, int start, int end, int depth) {
    if(end <= start || end - start <= 1) {
        return;
    }
    int middle = (end-start)/2;
    middle += start;
    if(middle == start || middle == end) {
        return;
    } else {
        double rise = (array[end].y() - array[start].y());
        double run =( array[end].x() - array[start].x());
        Vector3 tan(-1*rise, run, 0);
        double random = (rand() % 100 - 50) / ((double)60);
        random = random / (depth * 2);
        tan.scale(random);
        array[middle] = Vector3((array[end].x() + array[start].x())/2, (array[end].y() + array[start].y())/2, 0);
        array[middle] = array[middle] + tan;
        depth++;
        genMountain(array, start, middle, depth);
        genMountain(array, middle, end, depth);
    }
}


/*~~~~~~~~~~~~~~~~SHADOWS~~~~~~~~~~~~~~*/

// Expressed as float so gluPerspective division returns a float and not 0 (640/480 != 640.0/480.0).
#define SHADOW_MAP_RATIO 4


//Camera position
float p_camera[3] = { 0, 10, 20 };

//Camera lookAt
float l_camera[3] = { 0, 5, -10 };

//Light position
float p_light[3] = { 3, 20, 0 };

//Light lookAt
float l_light[3] = { 0, 0, -5 };


//Light mouvement circle radius
float light_mvnt = 30.0f;

// Hold id of the framebuffer for light POV rendering
GLuint fboId;

// Z values will be rendered to this texture when using fboId framebuffer
GLuint depthTextureId;

// Use to activate/disable shadowShader
GLhandleARB shadowShaderId;
GLuint shadowMapUniform;



// Loading shader function
GLhandleARB loadShader(char* filename, unsigned int type)
{
	FILE *pfile;
	GLhandleARB handle;
	const GLcharARB* files[1];

	// shader Compilation variable
	GLint result;				// Compilation code result
	GLint errorLoglength;
	char* errorLogText;
	GLsizei actualErrorLogLength;

	char buffer[400000];
	memset(buffer, 0, 400000);

	// This will raise a warning on MS compiler
	pfile = fopen(filename, "rb");
	if (!pfile)
	{
		printf("Sorry, can't open file: '%s'.\n", filename);
		exit(0);
	}

	fread(buffer, sizeof(char), 400000, pfile);
	//printf("%s\n",buffer);


	fclose(pfile);

	handle = glCreateShaderObjectARB(type);
	if (!handle)
	{
		//We have failed creating the vertex shader object.
		printf("Failed creating vertex shader object from file: %s.", filename);
		exit(0);
	}

	files[0] = (const GLcharARB*)buffer;
	glShaderSourceARB(
		handle, //The handle to our shader
		1, //The number of files.
		files, //An array of const char * data, which represents the source code of theshaders
		NULL);

	glCompileShaderARB(handle);

	//Compilation checking.
	glGetObjectParameterivARB(handle, GL_OBJECT_COMPILE_STATUS_ARB, &result);

	// If an error was detected.
	if (!result)
	{
		//We failed to compile.
		printf("Shader '%s' failed compilation.\n", filename);

		/*//Attempt to get the length of our error log.
		glGetObjectParameterivARB(handle, GL_OBJECT_INFO_LOG_LENGTH_ARB, &errorLoglength);

		//Create a buffer to read compilation error message
		errorLogText = malloc(sizeof(char) * errorLoglength);

		//Used to get the final length of the log.
		glGetInfoLogARB(handle, errorLoglength, &actualErrorLogLength, errorLogText);

		// Display errors.
		printf("%s\n", errorLogText);

		// Free the buffer malloced earlier
		free(errorLogText);*/
	}

	return handle;
}

void Window::loadShadowShader()
{
	GLhandleARB vertexShaderHandle;
	GLhandleARB fragmentShaderHandle;

	vertexShaderHandle = loadShader("/Users/Noah/Desktop/shadow_map.vert", GL_VERTEX_SHADER);
	fragmentShaderHandle = loadShader("/Users/Noah/Desktop/shadow_map.frag", GL_FRAGMENT_SHADER);

	shadowShaderId = glCreateProgramObjectARB();

	glAttachObjectARB(shadowShaderId, vertexShaderHandle);
	glAttachObjectARB(shadowShaderId, fragmentShaderHandle);
	glLinkProgramARB(shadowShaderId);

	shadowMapUniform = glGetUniformLocationARB(shadowShaderId, "ShadowMap");
}

void Window::generateShadowFBO()
{
	int shadowMapWidth = Window::width * SHADOW_MAP_RATIO;
	int shadowMapHeight = Window::height * SHADOW_MAP_RATIO;

	//GLfloat borderColor[4] = {0,0,0,0};

	GLenum FBOstatus;

	// Try to use a texture depth component
	glGenTextures(1, &depthTextureId);
	glBindTexture(GL_TEXTURE_2D, depthTextureId);

	// GL_LINEAR does not make sense for depth texture. However, next tutorial shows usage of GL_LINEAR and PCF
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Remove artefact on the edges of the shadowmap
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	//glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor );



	// No need to force GL_DEPTH_COMPONENT24, drivers usually give you the max precision if available 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// create a framebuffer object
	glGenFramebuffersEXT(1, &fboId);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);

	// Instruct openGL that we won't bind a color texture with the currently binded FBO
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	// attach the texture to FBO depth attachment point
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depthTextureId, 0);

	// check FBO status
	FBOstatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if (FBOstatus != GL_FRAMEBUFFER_COMPLETE_EXT)
		printf("GL_FRAMEBUFFER_COMPLETE_EXT failed, CANNOT use FBO\n");

	// switch back to window-system-provided framebuffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void setupMatrices(float position_x, float position_y, float position_z, float lookAt_x, float lookAt_y, float lookAt_z)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (double)Window::width / (double)Window::height, 10, 40000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(position_x, position_y, position_z, lookAt_x, lookAt_y, lookAt_z, 0, 1, 0);
}


// This update only change the position of the light.
//int elapsedTimeCounter = 0;
void update(void)
{

	p_light[0] = light_mvnt * cos(glutGet(GLUT_ELAPSED_TIME) / 1000.0);
	p_light[2] = light_mvnt * sin(glutGet(GLUT_ELAPSED_TIME) / 1000.0);

	//p_light[0] = light_mvnt * cos(3652/1000.0);
	//p_light[2] = light_mvnt * sin(3652/1000.0);
}


void setTextureMatrix(void)
{
	static double modelView[16];
	static double projection[16];

	// This is matrix transform every coordinate x,y,z
	// x = x* 0.5 + 0.5 
	// y = y* 0.5 + 0.5 
	// z = z* 0.5 + 0.5 
	// Moving from unit cube [-1,1] to [0,1]  
	const GLdouble bias[16] = {
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0 };

	// Grab modelview and transformation matrices
	glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);


	glMatrixMode(GL_TEXTURE);
	glActiveTextureARB(GL_TEXTURE7);

	glLoadIdentity();
	glLoadMatrixd(bias);

	// concatating all matrice into one.
	glMultMatrixd(projection);
	glMultMatrixd(modelView);

	// Go back to normal matrix mode
	glMatrixMode(GL_MODELVIEW);
}

// During translation, we also have to maintain the GL_TEXTURE8, used in the shadow shader
// to determine if a vertex is in the shadow.
void startTranslate(float x, float y, float z)
{
	glPushMatrix();
	glTranslatef(x, y, z);

	glMatrixMode(GL_TEXTURE);
	glActiveTextureARB(GL_TEXTURE7);
	glPushMatrix();
	glTranslatef(x, y, z);
}

void endTranslate()
{
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void drawObjects(void)
{
	// Ground
	glColor4f(0.3f, 0.3f, 0.3f, 1);
	glBegin(GL_QUADS);
	glVertex3f(-35, 2, -35);
	glVertex3f(-35, 2, 15);
	glVertex3f(15, 2, 15);
	glVertex3f(15, 2, -35);
	glEnd();

	glColor4f(0.9f, 0.9f, 0.9f, 1);

	// Instead of calling glTranslatef, we need a custom function that also maintain the light matrix
	startTranslate(0, 4, -16);
	glutSolidCube(4);
	endTranslate();

	startTranslate(-6, 4, -5);
	glutSolidCube(4);
	endTranslate();

	// save the current matrix
	glPushMatrix();
	Matrix4 modelToWorld = model;
	modelToWorld.transpose();
	glMultMatrixd(modelToWorld.getPointer());
	startTranslate(0, -4, -50);
    if (genMountains) {
        double startY = (rand() % 100 - 50) / ((double)60);
        for(int i = 0; i < 10; i++) {
            mountains[i] = Mountain(4*i-12, 4, 2+startY);
            mountains[i].generate();
            startY = mountains[i].endY;
        }
        genMountains = false;
    }
    
    glBegin(GL_QUADS);
    
    for (int i = 0; i < 10; i++) {
        mountains[i].draw();
    }
	glPopMatrix();
	endTranslate();
}

/*~~~~~~~~~~~~~~~~SHADOWS~~~~~~~~~~~~~~*/

void Window::displayCallback()
{
    /*glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // clear color and depth buffers
    glMatrixMode(GL_MODELVIEW);  // make sure we're in Modelview mode
    glDisable(GL_LIGHTING);
    
    glFlush();
    glutSwapBuffers();*/

	update();

	//First step: Render from the light POV to a FBO, story depth values only
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);	//Rendering offscreen

	//Using the fixed pipeline to render to the depthbuffer
	glUseProgramObjectARB(0);

	// In the case we render the shadowmap to a higher resolution, the viewport must be modified accordingly.
	glViewport(0, 0, Window::width * SHADOW_MAP_RATIO, Window::height * SHADOW_MAP_RATIO);

	// Clear previous frame values
	glClear(GL_DEPTH_BUFFER_BIT);

	//Disable color rendering, we only want to write to the Z-Buffer
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	setupMatrices(p_light[0], p_light[1], p_light[2], l_light[0], l_light[1], l_light[2]);

	// Culling switching, rendering only backface, this is done to avoid self-shadowing
	glCullFace(GL_FRONT);
	drawObjects();
	
	//Save modelview/projection matrice into texture7, also add a biais
	setTextureMatrix();


	// Now rendering from the camera POV, using the FBO to generate shadows
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	glViewport(0, 0, Window::width, Window::height);

	//Enabling color write (previously disabled for light POV z-buffer rendering)
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	// Clear previous frame values
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Using the shadow shader
	glUseProgramObjectARB(shadowShaderId);
	glUniform1iARB(shadowMapUniform, 7);
	glActiveTextureARB(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, depthTextureId);






	setupMatrices(p_camera[0], p_camera[1], p_camera[2], l_camera[0], l_camera[1], l_camera[2]);

	glCullFace(GL_BACK);
	drawObjects();

	// DEBUG only. this piece of code draw the depth buffer onscreen
	/*
	glUseProgramObjectARB(0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-RENDER_WIDTH/2,RENDER_WIDTH/2,-RENDER_HEIGHT/2,RENDER_HEIGHT/2,1,20);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor4f(1,1,1,1);
	glActiveTextureARB(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,depthTextureId);
	glEnable(GL_TEXTURE_2D);
	glTranslated(0,0,-1);
	glBegin(GL_QUADS);
	glTexCoord2d(0,0);glVertex3f(0,0,0);
	glTexCoord2d(1,0);glVertex3f(RENDER_WIDTH/2,0,0);
	glTexCoord2d(1,1);glVertex3f(RENDER_WIDTH/2,RENDER_HEIGHT/2,0);
	glTexCoord2d(0,1);glVertex3f(0,RENDER_HEIGHT/2,0);


	glEnd();
	glDisable(GL_TEXTURE_2D);
	*/

	glutSwapBuffers();
}

void translate(double tx, double ty, double tz)
{
    Matrix4 m;
    m.makeTranslate(tx, ty, tz);
    model = m * model;
}


void Window::keyboardCallback(unsigned char key, int x, int y)
{
        if(key == 's')
        {
            Matrix4 m;
            m.makeScale(0.9, 0.9, 0.9);
            model = model * m;
        } else if(key == 'S')
        {
            Matrix4 m;
            m.makeScale(1.1, 1.1, 1.1);
            model = model * m;
        }
        if(key == 'x')
        {
            translate(-1, 0, 0);
        } else if(key == 'X')
        {
            translate(1, 0, 0);
        } else if(key == 'y')
        {
            translate(0, -1, 0);
        } else if(key == 'Y')
        {
            translate(0, 1, 0);
        } else if(key == 'z')
        {
            translate(0, 0, -1);
        } else if(key == 'Z')
        {
            translate(0, 0, 1);
        } else if(key == 'r')
        {
            model.identity();
        }else if(key == 'g') {
            genMountains = true;
        }
}

void Window::mouseFunc(int button, int state, int x, int y) {

}

void Window::motionFunc(int x, int y) {

}

void Window::specialCallback(int key, int b, int c)
{

}