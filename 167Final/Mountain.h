//
//  Mountain.h
//  167Final
//
//  Created by Noah Martin on 12/12/14.
//  Copyright (c) 2014 Noah Martin. All rights reserved.
//

#ifndef ___67Final__Mountain__
#define ___67Final__Mountain__

#include <stdio.h>
#include "Vector3.h"

class Mountain {
public:
    Mountain();
    Mountain(double startX, double width, double startY);
    void generate();
    void draw();
    double endY;
    double endX;
    void translate(double dy);
private:
    void genMountain(int start, int end, int depth);
    Vector3 mountains[200];
};


#endif /* defined(___67Final__Mountain__) */
