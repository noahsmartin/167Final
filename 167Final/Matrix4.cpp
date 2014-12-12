#include <math.h>

#include "Matrix4.h"

Matrix4::Matrix4()
{
  for (int i=0; i<4; ++i)
  {
    for (int j=0; j<4; ++j)
    {
      m[i][j] = 0;
    }
  }
}

Matrix4 Matrix4::operator*(const Matrix4& m)
{
    Matrix4 result;
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            result.m[i][j] = Vector4::dot(rowVector(i), m.columnVector(j));
        }
    }
    return result;
}

Vector4 Matrix4::operator*(const Vector4& v)
{
    Vector4 result(Vector4::dot(v, rowVector(0)), Vector4::dot(v, rowVector(1)), Vector4::dot(v, rowVector(2)), Vector4::dot(v, rowVector(3)));
    return result;
}

Matrix4 Matrix4::operator*(double d)
{
    Matrix4 result = *this;
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            result.m[i][j] *= d;
        }
    }
    return result;
}

Matrix4 Matrix4::operator+(const Matrix4& m)
{
    Matrix4 result;
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            result.m[i][j] = this->m[i][j] + m.m[i][j];
        }
    }
    return result;
}

Matrix4 Matrix4::operator-(const Matrix4& m)
{
    Matrix4 mCopy = m;
    return *this + (mCopy * -1.0);
}

Matrix4& Matrix4::operator=(const Matrix4& m2)
{
  if (this != &m2)
  {
    for (int i=0; i<4; ++i)
    {
      for (int j=0; j<4; ++j)
      {
        m[i][j] = m2.m[i][j];
      }
    }
  }
  return *this;
}

// return pointer to matrix elements
double* Matrix4::getPointer()
{
  return &m[0][0];
}

// set matrix to identity matrix
void Matrix4::identity()
{
  for (int i=0; i<4; ++i)
  {
    for (int j=0; j<4; ++j)
    {
      if (i==j) 
        m[i][j] = 1.0; 
      else 
        m[i][j] = 0.0;
    }
  }
}

// transpose the matrix (mirror at diagonal)
void Matrix4::transpose()
{
  Matrix4 temp;
  for (int i=0; i<4; ++i)
  {
    for (int j=0; j<4; ++j)
    {
      temp.m[j][i] = m[i][j];
    }
  }
  *this = temp;  // copy temporary values to this matrix
}

// Creates a rotation matrix which rotates about the y axis.
// angle is expected in degrees
void Matrix4::makeRotateY(double angle)
{
  angle = angle / 180.0 * M_PI;  // convert from degrees to radians
  identity();
	m[0][0] = cos(angle);
	m[0][2] = sin(angle);
	m[2][0] = -sin(angle);
	m[2][2] = cos(angle);
}

void Matrix4::makeRotateX(double angle)
{
    angle = angle / 180.0 * M_PI;  // convert from degrees to radians
    identity();
    m[1][1] = cos(angle);
    m[1][2] = sin(angle);
    m[2][1] = -sin(angle);
    m[2][2] = cos(angle);
}

void Matrix4::makeRotateZ(double angle)
{
    angle = angle / 180.0 * M_PI;  // convert from degrees to radians
    identity();
    m[0][0] = cos(angle);
    m[1][0] = sin(angle);
    m[0][1] = -sin(angle);
    m[1][1] = cos(angle);
}

double Matrix4::xTrans()
{
    return m[0][3];
}

double Matrix4::yTrans()
{
    return m[1][3];
}

double Matrix4::zTrans()
{
    return m[2][3];
}

void Matrix4::makeRotate(double angle, const Vector3& axis)
{
    angle = angle / 180.0 * M_PI;
    identity();
    double c = cos(angle);
    double s = sin(angle);
    double x2 = axis.x() * axis.x();
    double y2 = axis.y() * axis.y();
    double z2 = axis.z() * axis.z();
    m[0][0] = x2 + c * ( 1- x2);
    m[0][1] = axis.x() * axis.y() * (1 - c) - axis.z()*s;
    m[0][2] = axis.x() * axis.z() * (1 - c) + axis.y()*s;
    m[1][0] = axis.x() * axis.y() * (1 - c) + axis.z() * s;
    m[1][1] = y2 + c * (1 - y2);
    m[1][2] = axis.y() * axis.z() * (1 - c) - axis.x() * s;
    m[2][0] = axis.x() * axis.z() * (1 - c) - axis.y() * s;
    m[2][1] = axis.y() * axis.z() * (1 - c) + axis.x() * s;
    m[2][2] = z2 + c * (1 - z2);
}

void Matrix4::makeScale(double sx, double sy, double sz)
{
    identity();
    m[0][0] *= sx;
    m[1][1] *= sy;
    m[2][2] *= sz;
}

void Matrix4::makeTranslate(double tx, double ty, double tz)
{
    identity();
    m[0][3] = tx;
    m[1][3] = ty;
    m[2][3] = tz;
}

void Matrix4::setVectorColumn(Vector4* vector, int col) {
    m[0][col] = vector->v[0];
    m[1][col] = vector->v[1];
    m[2][col] = vector->v[2];
    m[3][col] = vector->v[3];
}

void Matrix4::print(std::string string)
{
    printf("%s:\n", string.c_str());
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            printf("%f ", m[i][j]);
        }
        printf("\n");
    }
}

Vector3 Matrix4::position()
{
    return Vector3(m[0][3], m[1][3], m[2][3]);
}

Vector4 Matrix4::rowVector(int row) const
{
    Vector4 vector(m[row][0], m[row][1], m[row][2], m[row][3]);
    return vector;
}

Vector4 Matrix4::columnVector(int column) const
{
    Vector4 vector(m[0][column], m[1][column], m[2][column], m[3][column]);
    return vector;
}

double Matrix4::xScale() const
{
    return m[0][0];
}
