//
//  Vector4.cpp
//  Cube
//
//  Created by Noah Martin on 10/6/14.
//  Copyright (c) 2014 Noah Martin. All rights reserved.
//

#include "Vector4.h"

Vector4::Vector4() {
    v[0] = 0;
    v[1] = 0;
    v[2] = 0;
    v[3] = 0;
}

Vector4::Vector4(double x, double y, double z, double w)
{
    v[0] = x;
    v[1] = y;
    v[2] = z;
    v[3] = w;
}

Vector4 Vector4::operator+(const Vector4& v2)
{ 
    Vector4 sum(0, 0, 0, 0);
    for(int i = 0; i < 4; i++)
    {
        sum.v[i] = v[i] + v2.v[i];
    }
    return sum;
}

Vector4 Vector4::operator-(const Vector4& v2)
{
    Vector4 v(-1*v2.v[0], -1*v2.v[1], -1*v2.v[2], -1*v2.v[3]);
    return *this + v;
}

double Vector4::dot(const Vector4& v1, const Vector4& v2)
{
    double result = 0;
    for(int i = 0; i < 4; i++)
    {
        result += v1.v[i] * v2.v[i];
    }
    return result;
}

void Vector4::dehomogenize()
{
    for (int i = 0; i < 4; i++) {
        v[i] /= v[3];
    }
}

void Vector4::print(std::string comment)
{
    printf("%s x:%f y:%f z:%f w:%f", comment.c_str(), v[0], v[1], v[2], v[3]);
}