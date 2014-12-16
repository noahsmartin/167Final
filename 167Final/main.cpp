//
//  main.cpp
//  167Final
//
//  Created by Noah Martin on 12/12/14.
//  Copyright (c) 2014 Noah Martin. All rights reserved.
//

/*
 Here are the #defines you need to make this work:
 #define SHADOW_MAP_VERT_SHADER
 #define SHADOW_MAP_FRAG_SHADER
 #define BG_PATH
 #define BUMP_MAP
 #define BUMP_VERT_SHADER
 #define BUMP_FRAG_SHADER
 */


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include "GLee.h"
#include <GL/glut.h>
#endif

#include "Window.h"
#include "paths.h"
#include "SOIL.h"
#include "main.h"

namespace Globals {
    GLuint textures[3] = {0};
    Shader* shader;
}

void loadTexture() {
    int channels;
    unsigned char* data;
    int width, height;
    
    glGenTextures(3, &Globals::textures[0]);
    
    data = SOIL_load_image(BG_PATH,  &width, &height, &channels, SOIL_LOAD_AUTO);
    
    glBindTexture(GL_TEXTURE_2D, Globals::textures[0]);
    
    glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    
    // Make sure no bytes are padded:
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    // Select GL_MODULATE to mix texture with polygon color for shading:
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    // Use bilinear interpolation:
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    
    //The following code will read in our RAW file
    
    data = (unsigned char *) SOIL_load_image(BUMP_MAP, &width, &height, &channels, SOIL_LOAD_AUTO);
    
    glBindTexture( GL_TEXTURE_2D, Globals::textures[2] ); //bind the texture to it’s array
    glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    // Make sure no bytes are padded:
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    // Select GL_MODULATE to mix texture with polygon color for shading:
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    // Use bilinear interpolation:
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

int main(int argc, char *argv[])
{
    srand (time(NULL));

    glutInit(&argc, argv);      	      	      // initialize GLUT
    
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);   // open an OpenGL context with double buffering, RGB colors, and depth buffering
    glutInitWindowSize(Window::width, Window::height);      // set initial window size
    glutCreateWindow("167 Final Project");    	      // open window and set window title
    
    loadTexture();
    
    
    glEnable(GL_DEPTH_TEST);            	      // enable depth buffering
    glClear(GL_DEPTH_BUFFER_BIT);       	      // clear depth buffer
    glClearColor(0.0, 0.0, 0.0, 0.0);   	      // set clear color to black
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // set polygon drawing mode to fill front and back of each polygon
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glShadeModel(GL_SMOOTH);             	      // set shading to smooth
    glMatrixMode(GL_PROJECTION);
    
    glEnable(GL_TEXTURE_2D);   // enable texture mapping
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
	glEnable(GL_NORMALIZE);

	float specular[] = { 1.0, 1.0, 1.0, 1.0 };
	float shininess[] = { 100.0 };
	// Generate material properties:
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
    
	float position[] = { 80.0, 800.0, 0.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, position);
    
    Globals::shader = new Shader(BUMP_VERT_SHADER, BUMP_FRAG_SHADER);
  
    
    // Install callback functions:
    glutDisplayFunc(Window::displayCallback);
    glutReshapeFunc(Window::reshapeCallback);
    glutIdleFunc(Window::idleCallback);
    glutKeyboardFunc(Window::keyboardCallback);
    glutSpecialFunc(Window::specialCallback);
    glutMouseFunc(Window::mouseFunc);
    glutMotionFunc(Window::motionFunc);

	// Enable multiple key press
	glutIgnoreKeyRepeat(1);
	glutSpecialUpFunc(Window::specialUpCallback);
	
	// force to full screen for generateShadowFBO which requires screen width/height
	glutFullScreen();
	Window::width = glutGet(GLUT_SCREEN_WIDTH);
	Window::height = glutGet(GLUT_SCREEN_HEIGHT);
	Window::generateShadowFBO();
	Window::loadShadowShader();

    glutMainLoop();

    return 0;
}
