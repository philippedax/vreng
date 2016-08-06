//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)       http://vreng.enst.fr/
//
// Copyright (C) 1997-2011 Philippe Dax
// Telecom-ParisTech (Ecole Nationale Superieure des Telecommunications)
//
// VREng is a free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public Licence as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// VREng is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "vector3.hpp"


#if 0
  // Inline implementation.
  inline float operator*(const Vector3 &a, const Vector3 &b) {
    return a.DotProduct(b);
  }
  inline Vector3 operator*(const Vector3 &a, float b) {
    return Vector3(a.x*b, a.y*b, a.z*b);
  }
  inline Vector3 operator*(float b, const Vector3 &a) {
    return Vector3(a.x*b, a.y*b, a.z*b);
  }
  inline Vector3 &operator*=(Vector3 &a, float b) {
    a.Scale(b);
    return a;
  }
  inline Vector3 operator^(const Vector3 &a, const Vector3 &b) {
    Vector3 c; c.CrossProduct(a, b);
    return c;
  }

  // Math opperations.
  inline Vector3 operator+(const Vector3& ptSrc1, const Vector3& ptSrc2) {
    Vector3 ptTmp(ptSrc1);
    ptTmp.Add(ptSrc2);
    return ptTmp;
  }
  inline Vector3 operator-(const Vector3& ptSrc1, const Vector3& ptSrc2) {
    Vector3 ptTmp(ptSrc1);
    ptTmp.Subtract(ptSrc2);
    return ptTmp;
  }
#endif

// DotProduct of this vector into the other vector.
float Vector3::DotProduct(const Vector3 &other) const
{
  return (x * other.x + y * other.y + z * other.z);
}

// Calculates the cross product of "a" and "b", then storing it in this vector.
void Vector3::CrossProduct(const Vector3 &a, const Vector3 &b)
{
  x = a.y * b.z - b.y * a.z;
  y = b.x * a.z - a.x * b.z;
  z = a.x * b.y - b.x * a.y;
}

// Returns the length of this vector.
float Vector3::Length()
{
  return (float) sqrt(sqr(x) + sqr(y) + sqr(z));
}

// Returns the squared length of this vector.
float Vector3::Length2()
{
  return (float) (sqr(x) + sqr(y) + sqr(z));
}

// Normalizes this vector, and returns the scalar value used to normalize the vector.
float Vector3::Normalize()
{
  float nn = sqr(x) + sqr(y) + sqr(z);
  if (nn < EPSILON) return 0.;
  nn = (float) sqrt(nn);

  float n = 1. / nn;
  x *= n;
  y *= n;
  z *= n;
  return nn;
}

void Vector3::Zero( )
{
  x = y = z = 0.; w = 1.;
}

// Scale this vector by "s".
void Vector3::Scale(float s)
{
  x *= s; y *= s; z *= s;
}

// Add the other vector to this vector.
void Vector3::Add(const Vector3 &other)
{
  x += other.x; y += other.y; z += other.z;
}

// Subtract the other vector from this vector.
void Vector3::Subtract(const Vector3 &other)
{
  x -= other.x; y -= other.y; z -= other.z;
}

// Linear Interpolate
void Vector3::Lerp( const Vector3 &a, const Vector3 &b, float percent )
{
  x = a.x*(1.f-percent) + b.x*percent;
  y = a.y*(1.f-percent) + b.y*percent;
  z = a.z*(1.f-percent) + b.z*percent;
}

// Retrieve the angles of this vector.
void Vector3::GetAngles(float &pan, float &pitch)
{
  float dx = (float)x;
  float dy = (float)y;
  float dz = (float)z;

  if (dz > 0.0001) {
    pan = (float) -atan(dx/dz);
    dz /= cos((float)pan);
    pitch = (float) atan(dy/dz);
  }
  else if (dz < -0.0001) {
    pan = (float) (M_PI-atan(dx/dz));
    dz /= cos((float)pan-M_PI);
    pitch = (float) -atan(dy/dz);
  }
  else {
    pan = 0.0;
    pitch = (float)-M_PI_2;
  }
}

//
// Matrix3
//

void Matrix3::FromAngle(float ax, float ay, float az)
{
  float cx = cos(ax); float sx = sin(ax);
  float cy = cos(ay); float sy = sin(ay);
  float cz = cos(az); float sz = sin(az);

  // Calcul de la matrice de transformation 3D
  x.x = cy*cz; x.y = cx*sz*cy+sx*sy; x.z = sx*sz*cy-cx*sy;
  y.x = -sz;   y.y = cx*cz;          y.z = sx*cz;
  z.x = cz*sy; z.y = cx*sz*sy-sx*cy; z.z = sx*sz*sy+cx*cy;
}

void Matrix3::Apply(const Vector3 &v, Vector3 *out)
{
  out->x = v*x;
  out->y = v*y;
  out->z = v*z;
}

void Matrix3::RApply(Vector3 const &v, Vector3 *out)
{
  out->x = v.x*x.x+v.y*y.x+v.z*z.x;
  out->y = v.x*x.y+v.y*y.y+v.z*z.y;
  out->z = v.x*x.z+v.y*y.z+v.z*z.z;
}

void Matrix3::Apply(Vector3 *v)
{
  Vector3 i(*v);
  Apply(i, v);
}

void Matrix3::RApply(Vector3 *v)
{
  Vector3 i(*v);
  RApply(i, v);
}
