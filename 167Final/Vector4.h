#ifndef __Cube__Vector4__
#define __Cube__Vector4__

#include <string>

class Vector4
{
  public:
    double v[4];
  public:
    Vector4();
    Vector4(double, double, double, double);
    Vector4 operator+(const Vector4&);
    Vector4 operator-(const Vector4&);
    static double dot(const Vector4&, const Vector4&);
    void dehomogenize();
    void print(std::string);
};

#endif