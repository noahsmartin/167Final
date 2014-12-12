#ifndef _MATRIX4_H_
#define _MATRIX4_H_

#define M_PI 3.14159265358979323846
#include "Vector4.h"
#include "Vector3.h"

class Matrix4
{
  public:
    double m[4][4];   // matrix elements; first index is for rows, second for columns (row-major)
    
  public:
    Matrix4();
    Matrix4 operator*(const Matrix4&);
    Vector4 operator*(const Vector4&);
    Matrix4 operator*(double);
    Matrix4 operator+(const Matrix4&);
    Matrix4 operator-(const Matrix4&);
    Matrix4& operator=(const Matrix4&);
    void setVectorColumn(Vector4* vector, int col);
    double* getPointer();
    void identity(); 
    void transpose();
    void makeRotateX(double);
    void makeRotateY(double);
    void makeRotateZ(double);
    void makeRotate(double, const Vector3&);
    void makeScale(double, double, double);
    void makeTranslate(double, double, double);
    void print(std::string);
    double xTrans();
    double yTrans();
    double zTrans();
    double xScale() const;
    Vector3 position();
    
public:
    Vector4 rowVector(int) const;
    Vector4 columnVector(int) const;
};

#endif