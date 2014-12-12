//
//  Camera.h
//  Cube
//
//  Created by Noah Martin on 10/20/14.
//  Copyright (c) 2014 Noah Martin. All rights reserved.
//

#ifndef __Cube__Camera__
#define __Cube__Camera__

#include "Vector3.h"
#include "Matrix4.h"

class Camera {
  public:
    Vector3 e;
    Vector3 d;
    Vector3 up;
    Vector3 z;
    Matrix4 c;
    
  public:
    Camera(const Vector3&, const Vector3&, const Vector3&);
    Matrix4 getMatrix();
    double* getglmatrix();
};

#endif /* defined(__Cube__Camera__) */
