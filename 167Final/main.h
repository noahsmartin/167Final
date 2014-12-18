//
//  main.h
//  167Final
//
//  Created by Noah Martin on 12/13/14.
//  Copyright (c) 2014 Noah Martin. All rights reserved.
//

#ifndef _67Final_main_h
#define _67Final_main_h

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "shader.h"

namespace Globals {
    extern GLuint textures[4];
    extern Shader* shader;
}

#endif
