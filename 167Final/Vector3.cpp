//
//  Vector3.cpp
//  Cube
//
//  Created by Noah Martin on 10/6/14.
//  Copyright (c) 2014 Noah Martin. All rights reserved.
//

#include "Vector3.h"
#include <math.h>

Vector3::Vector3() {
    v[0] = 0;
    v[1] = 0;
    v[2] = 0;
}

Vector3::Vector3(double x, double y, double z)
{
  v[0] = x;
  v[1] = y;
  v[2] = z;
}

Vector3::Vector3(const Vector3& vector) {
    v[0] = vector.v[0];
    v[1] = vector.v[1];
    v[2] = vector.v[2];
}

void Vector3::setz(double d) {
    v[2] = d;
}

void Vector3::setx(double d) {
    v[0] = d;
}

void Vector3::sety(double d) {
    v[1] = d;
}

Vector3 Vector3::operator+(const Vector3 & v2)
{
  Vector3 sum(0, 0, 0);
  for(int i = 0; i < 3; i++)
  {
    sum.v[i] = v[i] + v2.v[i];
  }
  return sum;
}

Vector3 Vector3::operator-(const Vector3 & v2)
{
    Vector3 v(v2.v[0], v2.v[1], v2.v[2]);
    v.negate();
    return *this + v;
}

void Vector3::negate()
{
    scale(-1);
}

void Vector3::scalex(double s)
{
    v[0] *= s;
}

void Vector3::scaley(double s)
{
    v[1] *= s;
}

void Vector3::scalez(double s)
{
    v[2] *= s;
}

void Vector3::scale(double s)
{
  for(int i = 0; i < 3; i++)
  {
    v[i] *= s;
  }
}

double Vector3::dot(const Vector3& v1, const Vector3& v2)
{
  double dot = 0;
    for(int i = 0; i < 3; i++)
    {
        dot += v1.v[i] * v2.v[i];
    }
    return dot;
}

 Vector3 Vector3::cross(const Vector3& v1, const Vector3& v2)
{
    return Vector3(v1.v[1] * v2.v[2] - v1.v[2] * v2.v[1], -v1.v[0]*v2.v[2] + v1.v[2] * v2.v[0], v1.v[0] * v2.v[1] - v1.v[1] * v2.v[0]);
}

double Vector3::length()
{
    Vector3 v = *this;
    return sqrt(dot(v, v));
}

void Vector3::normalize()
{
  if(length())
  {
    scale(1 / length());
  }
}

double Vector3::x() const
{
    return v[0];
}

double Vector3::y() const
{
    return v[1];
}

double Vector3::z() const
{
    return v[2];
}

void Vector3::print(std::string comment) const
{
    printf("%s x:%f y:%f z: %f\n", comment.c_str(), v[0], v[1], v[2]);
}