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
#include <GL/glut.h>
#endif

using namespace std;

int Window::width  = 512;   // set window width in pixels here
int Window::height = 512;   // set window height in pixels here

Camera camera(Vector3(0, 0, 10), Vector3(0, 0, -1), Vector3(0, 1, 0));

Matrix4 model;

Vector3 mountains[200];
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


void Window::displayCallback()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // clear color and depth buffers
    glMatrixMode(GL_MODELVIEW);  // make sure we're in Modelview mode
    glDisable(GL_LIGHTING);
    
    Matrix4 modelToWorld = camera.getMatrix() * model;
    modelToWorld.transpose();
    glLoadIdentity();
    glLoadMatrixd(modelToWorld.getPointer());
    
    if(genMountains) {
        double startY = (rand() % 100 - 50) / ((double)60);
        double endY = (rand() % 100 - 50) / ((double)60);
        mountains[199] = Vector3(2, 2+startY, 0);
        mountains[0] = Vector3(-2, 2+endY, 0);
        genMountain(mountains, 0, 199, 1);
        
        genMountains = false;
    }
    
    glBegin(GL_QUADS);
    
    for(int i = 0; i < 199; i++) {
        glColor3f(0.0, 1.0, 0.0);
        glNormal3f(0, 0, 1);
        glVertex3f(mountains[i].x(), mountains[i].y(), mountains[i].z());
        glVertex3f(mountains[i].x(), 0, 0);
        glVertex3f(mountains[i+1].x(), 0, 0);
        glVertex3f(mountains[i+1].x(), mountains[i+1].y(), mountains[i].z());
    }
    glEnd();

    glFlush();
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