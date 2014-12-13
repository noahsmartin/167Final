//
//  Window.h
//  167Final
//
//  Created by Noah Martin on 12/12/14.
//  Copyright (c) 2014 Noah Martin. All rights reserved.
//

#ifndef ___67Final__Window__
#define ___67Final__Window__

#include <stdio.h>

#include "Vector3.h"

class Window	  // OpenGL output window related routines
{
public:
    static int width, height; 	            // window size
    
    static void idleCallback(void);
    static void reshapeCallback(int, int);
    static void displayCallback(void);
    static void keyboardCallback(unsigned char, int, int);
    static void specialCallback(int, int, int);
    static void mouseFunc(int button, int state, int x, int y);
    static void motionFunc(int x, int y);
    static Vector3 getVector(int x, int y);
	static void generateShadowFBO();
	static void loadShadowShader();
};

#endif /* defined(___67Final__Window__) */
