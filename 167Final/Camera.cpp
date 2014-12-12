//
//  Camera.cpp
//  Cube
//
//  Created by Noah Martin on 10/20/14.
//  Copyright (c) 2014 Noah Martin. All rights reserved.
//

#include "Camera.h"
#include "Vector3.h"


Camera::Camera(const Vector3& e, const Vector3& d, const Vector3& up) : e(e), d(d), up(up), z(0, 0, 0) {
    z = this->e - this->d;
    z.normalize();
    Vector3 x = Vector3::cross(this->up, z);
    x.normalize();
    Vector3 y = Vector3::cross(z, x);
    c = Matrix4();
    c.identity();
    c.makeTranslate(e.x(), e.y(), e.z());
    c.getPointer()[0] = x.x();
    c.getPointer()[1] = y.x();
    c.getPointer()[2] = z.x();
    c.getPointer()[4] = x.y();
    c.getPointer()[5] = y.y();
    c.getPointer()[6] = z.y();
    c.getPointer()[8] = x.z();
    c.getPointer()[9] = y.z();
    c.getPointer()[10] = z.z();
        
    // Now take the inverse
    Matrix4 translate = Matrix4();
    translate.identity();
    translate.makeTranslate(-1*e.x(), -1*e.y(), -1*e.z());
    Matrix4 mask = Matrix4();
    mask.identity();
    mask.getPointer()[15] = 0;
    Matrix4 rotate = c * mask;
    rotate.getPointer()[15] = 1;
    rotate.transpose();
    c = rotate * translate;
}

Matrix4 Camera::getMatrix() {
    return c;
}

double* Camera::getglmatrix() {
    Matrix4 m = c;
    m.transpose();
    return m.getPointer();
}