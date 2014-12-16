//
//  Mountain.cpp
//  167Final
//
//  Created by Noah Martin on 12/12/14.
//  Copyright (c) 2014 Noah Martin. All rights reserved.
//

#include "Mountain.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdio.h>
#include <cstdlib>

Mountain::Mountain() {
    
}

Mountain::Mountain(double startX, double width, double startY, double z, Vector3 color) {
    mountains[0] = Vector3(startX, startY, z);
    double endY = (rand() % 100 - 50) / ((double)60);
    mountains[199] = Vector3(startX+width, 2+endY, z);
    this->endY = endY;
    endX = startX + width;
    this->color = color;
    this->z = z;
}

void Mountain::generate() {
    genMountain(0, 199, 1);
}

void Mountain::genMountain(int start, int end, int depth) {
    if(end <= start || end - start <= 1) {
        return;
    }
    int middle = (end-start)/2;
    middle += start;
    if(middle == start || middle == end) {
        return;
    } else {
        double rise = (mountains[end].y() - mountains[start].y());
        double run =( mountains[end].x() - mountains[start].x());
        Vector3 tan(-1*rise, run, 0);
        double random = (rand() % 100 - 50) / ((double)60);
        random = random / (depth * 2.5);
        tan.scale(random);
        mountains[middle] = Vector3((mountains[end].x() + mountains[start].x())/2, (mountains[end].y() + mountains[start].y())/2, 0);
        mountains[middle] = mountains[middle] + tan;
        depth++;
        genMountain(start, middle, depth);
        genMountain(middle, end, depth);
    }
}

void Mountain::translate(double dx) {
    for (int i = 0; i < 200; i++) {
        mountains[i].v[0] = mountains[i].x() + dx;
    }
    endX += dx;
}

void Mountain::draw() {
    glBegin(GL_QUADS);
    //glMatrixMode(GL_MODELVIEW);
    for (int i = 0; i < 199; i++) {
        //glScalef(10, 10, 10);
        glNormal3f(0, 0, 1);
        glColor3f(color.x(), color.y(), color.z());
        Vector3 start = mountains[i];
        start.scale(8);
        Vector3 end = mountains[i+1];
        end.scale(8);
        glVertex3f(start.x(), start.y(), z);
        glVertex3f(start.x(), -20, z);
        glVertex3f(end.x(), -20, z);
        glVertex3f(end.x(), end.y(), z);
    }
    glEnd();
}
