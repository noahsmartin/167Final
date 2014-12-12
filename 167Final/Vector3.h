#ifndef _Vector3_H_
#define _Vector3_H_

#include <string>

class Vector3
{
  protected:
    double v[3];
    
  public:
    Vector3();
    Vector3(double, double, double);
    Vector3(const Vector3&);
    Vector3 operator+(const Vector3&);
    Vector3 operator-(const Vector3&);
    void negate();
    void scale(double);
    void scalex(double);
    void scaley(double);
    void scalez(double);
    void setz(double);
    void setx(double);
    void sety(double);
    double x() const;
    double y() const;
    double z() const;
    static double dot(const Vector3&, const Vector3&);
    static Vector3 cross(const Vector3&, const Vector3&);
    double length();
    void normalize();
    void print(std::string) const;
};

#endif
