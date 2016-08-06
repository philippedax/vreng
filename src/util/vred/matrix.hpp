#ifndef MATRIX_H
#define MATRIX_H

#include "vect.hpp"

#define PI 3.14159
#define COS(x) (cos(PI * (x) / 180))
#define SIN(x) (sin(PI * (x) / 180))
#define ACOS(x) (180 * acos((x)) / PI)

class Matrix {

public:
  Matrix();
  Matrix(const Vect& translation, const Vect& rotation, const Vect& scaling);

  operator const GLdouble*() const {return a;}
  operator GLdouble*() {return a;}

  double& operator[](const int n) {return a[n];}
  double operator[](const int n) const {return a[n];}

  Matrix operator*(const Matrix&) const;
  Matrix& operator*=(const Matrix&);
  
  Matrix Invert() const;
  void Print() const;

private:
  GLdouble a[16];

};

#endif
