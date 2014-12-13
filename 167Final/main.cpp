//
//  main.cpp
//  167Final
//
//  Created by Noah Martin on 12/12/14.
//  Copyright (c) 2014 Noah Martin. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "Window.h"

int main(int argc, char *argv[])
{
    srand (time(NULL));

    glutInit(&argc, argv);      	      	      // initialize GLUT
    
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);   // open an OpenGL context with double buffering, RGB colors, and depth buffering
    glutInitWindowSize(Window::width, Window::height);      // set initial window size
    glutCreateWindow("167 Final Project");    	      // open window and set window title
    
    
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
    
    
    
    // Install callback functions:
    glutDisplayFunc(Window::displayCallback);
    glutReshapeFunc(Window::reshapeCallback);
    glutIdleFunc(Window::idleCallback);
    glutKeyboardFunc(Window::keyboardCallback);
    glutSpecialFunc(Window::specialCallback);
    glutMouseFunc(Window::mouseFunc);
    glutMotionFunc(Window::motionFunc);
	
	// force to full screen for generateShadowFBO which requires screen width/height
	glutFullScreen();
	Window::width = glutGet(GLUT_SCREEN_WIDTH);
	Window::height = glutGet(GLUT_SCREEN_HEIGHT);
	Window::generateShadowFBO();
	Window::loadShadowShader();

    glutMainLoop();

    return 0;
}
