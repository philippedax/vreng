#include "vred.hpp"
#include "matrix.hpp"


Matrix::Matrix()
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glGetDoublev(GL_MODELVIEW_MATRIX, a);
  glPopMatrix();
}

Matrix::Matrix(const Vect& translation,
	       const Vect& rotation,
	       const Vect& scaling)
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glTranslated(translation[0], translation[1], translation[2]);
  glRotated(rotation[0], 1.0, 0.0, 0.0);
  glRotated(rotation[1], 0.0, 1.0, 0.0);
  glRotated(rotation[2], 0.0, 0.0, 1.0);
  glScaled(scaling[0], scaling[1], scaling[2]);
  glGetDoublev(GL_MODELVIEW_MATRIX, a);
  glPopMatrix();
}

Matrix Matrix::operator*(const Matrix& m) const
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadMatrixd(a);
  glMultMatrixd(m);
  Matrix tmp;
  glGetDoublev(GL_MODELVIEW_MATRIX, tmp);
  glPopMatrix();
  return tmp;  
}

Matrix& Matrix::operator*=(const Matrix& m)
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadMatrixd(a);
  glMultMatrixd(m);
  glGetDoublev(GL_MODELVIEW_MATRIX, a);
  glPopMatrix();
  return *this;
}

Matrix Matrix::invert() const
{
  double sx = sqrt(Vect(a[0], a[1], a[2]).norm());
  double sy = sqrt(Vect(a[4], a[5], a[6]).norm());
  double sz = a[10];
  double cos = a[0] / sx, sin = a[1] / sy, alpha = 0;
  if (sin >= 0)
    alpha = ACOS(cos);
  else
    alpha = - ACOS(cos);
  double tx = a[12], ty = a[13], tz = a[14];
  Matrix m;
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glScaled(1/sx, 1/sy, 1/sz);
  glRotated(-alpha, 0.0, 0.0, 1.0);
  glTranslated(-tx, -ty, -tz);
  glGetDoublev(GL_MODELVIEW_MATRIX, m);
  glPopMatrix();
  return m;
}

void Matrix::print() const
{
  cout << "(" << a[0] << " " << a[1] << " " << a[2] << " " << a[3] << ")" << endl;
  cout << "(" << a[4] << " " << a[5] << " " << a[6] << " " << a[7] << ")" << endl;
  cout << "(" << a[8] << " " << a[9] << " " << a[10] << " " << a[11] << ")" << endl;
  cout << "(" << a[12] << " " << a[13] << " " << a[14] << " " << a[15] << ")" << endl;
}
