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
#ifndef VECTOR3_HPP
#define VECTOR3_HPP


/**
 * Vector3 class.
 */
class Vector3 {

public:

  union { float x; float nx; };
  union { float y; float ny; };
  union { float z; float nz; };
  ///< Coordinates and normals.

  Vector3() {}
  Vector3(float tx, float ty, float tz) : x(tx), y(ty), z(tz) {}
  Vector3(float tx, float ty, float tz, float tw) : x(tx), y(ty), z(tz) {}
  ///< Constructors.

  virtual ~Vector3() {}
  ///< Destructor.

  //
  // General utility functions.
  //
  inline void set(float tx, float ty, float tz);
  ///< Set this vector.

  inline float dotProduct(const Vector3 &other) const;
  ///< DotProduct of this vector into the other vector.

  inline void  crossProduct(const Vector3 &a, const Vector3 &b);
  ///< Calculates the cross product of "a" and "b", then storing it in this vector.

  inline float length();
  ///< Returns the length of this vector.

  inline float length2();
  ///< Returns the squared length of this vector.

  inline float normalize();
  ///< Normalizes this vector, and returns the scalar value used to normalize the vector.

  inline void  scale(float s);
  ///< Scale this vector by "s".

  inline void  add(const Vector3 &other);
  ///< Add the other vector to this vector.

  inline void  subtract(const Vector3 &other);
  ///< Subtract the other vector from this vector. (result is contained in this vector.)

  inline void  lerp(const Vector3 &a, const Vector3 &b, float percent);
  ///< Linear Interpolate

  inline void  getAngles(float &pan, float &pitch);
  ///< Retrieve the angles of this vector.

  inline void  reset();
  ///< Reset variables to a normal zero constant.

  static inline Vector3 Vector(Vector3 p1, Vector3 p2);
  ///< Calculates a vector between 2 points and returns the result.

  static inline Vector3 addVector(Vector3 v1, Vector3 v2);
  ///< Adds 2 vectors together and returns the result.

  static inline Vector3 divideVectorByScaler(Vector3 v1, float val);
  ///< Divides a vector by a single number (scalar) and returns the result.

  static inline Vector3 cross(Vector3 v1, Vector3 v2);
  ///< Returns the cross product between 2 vectors.

  static inline Vector3 normalize(Vector3 v);
  ///< Returns the normal of a vector.

  //
  // Operators.
  //
  const Vector3& operator = (const Vector3 &pt)
  { x = pt.x; y = pt.y; z = pt.z; return *this; }

  friend Vector3 operator + (const Vector3 &pt1, const Vector3 &pt2);
  friend Vector3 operator - (const Vector3 &pt1, const Vector3 &pt2);
  friend Vector3 operator * (const Vector3 &pt, float f); 
  friend Vector3 operator * (float f, const Vector3 &pt);
  friend Vector3 operator * (float * m, Vector3 & v);

  const Vector3& operator += (const Vector3 &pt) { add(pt); return *this; }
  const Vector3& operator -= (const Vector3 &pt) { subtract(pt); return *this; }
  //const Vector3& operator *= (float f) { scale(f); return *this; }

};

// Inline implementation.
inline float operator * (const Vector3 &a, const Vector3 &b) {
  return a.dotProduct(b);
}
inline Vector3 operator * (const Vector3 &a, float b) {
  return Vector3(a.x*b, a.y*b, a.z*b);
}
inline Vector3 operator * (float b, const Vector3 &a) {
  return Vector3(a.x*b, a.y*b, a.z*b);
}
inline Vector3& operator *= (Vector3 &a, float b) {
  a.scale(b);
  return a;
}
inline Vector3 operator ^ (const Vector3 &a, const Vector3 &b) {
  Vector3 c; c.crossProduct(a, b);
  return c;
}

// Sets this vector
void Vector3::set(float tx, float ty, float tz)
{
  x = tx;
  y = ty;
  z = tz;
}

// DotProduct of this vector into the other vector.
float Vector3::dotProduct(const Vector3 &other) const
{
  return (x * other.x + y * other.y + z * other.z);
}

// Calculates the cross product of "a" and "b", then storing it in this vector.
void Vector3::crossProduct(const Vector3 &a, const Vector3 &b)
{
  x = a.y * b.z - b.y * a.z;
  y = b.x * a.z - a.x * b.z;
  z = a.x * b.y - b.x * a.y;
}

// Returns the length of this vector.
float Vector3::length()
{
  return (float) sqrt(SQR(x) + SQR(y) + SQR(z));
}

// Returns the squared length of this vector.
float Vector3::length2()
{
  return (float) (SQR(x) + SQR(y) + SQR(z));
}

// Normalizes this vector, and returns the scalar value used to normalize the vector.
float Vector3::normalize()
{
  float nn = SQR(x) + SQR(y) + SQR(z);
  if (nn < EPSILON) return 0.;
  nn = (float) sqrt(nn);

  float n = 1. / nn;
  x *= n;
  y *= n;
  z *= n;
  return nn;
}

void Vector3::reset()
{
  x = y = z = 0;
}

// Scale this vector by "s".
void Vector3::scale(float s)
{
  x *= s; y *= s; z *= s;
}

// Add the other vector to this vector.
void Vector3::add(const Vector3 &other)
{
  x += other.x; y += other.y; z += other.z;
}

// Subtract the other vector from this vector.
void Vector3::subtract(const Vector3 &other)
{
  x -= other.x; y -= other.y; z -= other.z;
}

// calculates a vector between 2 points and returns the result
Vector3 Vector3::Vector(Vector3 p1, Vector3 p2)
{       
  Vector3 v;

  v.x = p1.x - p2.x;
  v.y = p1.y - p2.y;
  v.z = p1.z - p2.z;            
  return v;
}

// adds 2 vectors together and returns the result
Vector3 Vector3::addVector(Vector3 v1, Vector3 v2)
{
  Vector3 v;

  v.x = v2.x + v1.x;
  v.y = v2.y + v1.y;
  v.z = v2.z + v1.z;
  return v;
}   
  
// divides a vector by a single number (scalar) and returns the result
Vector3 Vector3::divideVectorByScaler(Vector3 v1, float val)
{
  Vector3 v;

  v.x = v1.x / val;
  v.y = v1.y / val;
  v.z = v1.z / val;
  return v;
}

// returns the cross product between 2 vectors
Vector3 Vector3::cross(Vector3 v1, Vector3 v2)
{
  Vector3 v;

  v.x = ((v1.y * v2.z) - (v1.z * v2.y));
  v.y = ((v1.z * v2.x) - (v1.x * v2.z));
  v.z = ((v1.x * v2.y) - (v1.y * v2.x));
  return v;
}

// returns the normal of a vector
Vector3 Vector3::normalize(Vector3 v)
{
  float magnitude = (float) sqrt(v.x*v.x + v.y*v.y + v.z*v.z);

  v.x /= magnitude;
  v.y /= magnitude;
  v.z /= magnitude;
  return v;
}

// Linear Interpolate
void Vector3::lerp( const Vector3 &a, const Vector3 &b, float percent )
{
  x = a.x*(1.f-percent) + b.x*percent;
  y = a.y*(1.f-percent) + b.y*percent;
  z = a.z*(1.f-percent) + b.z*percent;
}

// Retrieve the angles of this vector.
void Vector3::getAngles(float &pan, float &pitch)
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
    pan = (float) (M_PI - atan(dx/dz));
    pitch = (float) -atan(dy/dz);
  }
  else {
    pan = 0;
    pitch = (float) -M_PI_2;
  }
}

// Math opperations.
inline Vector3 operator + (const Vector3 &pt1, const Vector3 &pt2)
{
  Vector3 pt(pt1);
  pt.add(pt2);
  return pt;
}

inline Vector3 operator - (const Vector3 &pt1, const Vector3 &pt2)
{
  Vector3 pt(pt1);
  pt.subtract(pt2);
  return pt;
}

// -> Multiplication matricielle
inline Vector3 operator * (float * m, Vector3 & v) {
  Vector3 result;
  result.x = v.x * m[ 0] + v.y * m[ 4] + v.z * m[ 8] + m[12];
  result.y = v.x * m[ 1] + v.y * m[ 5] + v.z * m[ 9] + m[13];
  result.z = v.x * m[ 2] + v.y * m[ 6] + v.z * m[10] + m[14];
  return result;
}

#if 0
inline void operator *= (float * m) {
  float tx = x;
  float ty = y;
  float tz = z;
  x = tx * m[ 0] + ty * m[ 4] + tz * m[ 8] + m[12];
  y = tx * m[ 1] + ty * m[ 5] + tz * m[ 9] + m[13];
  z = tx * m[ 2] + ty * m[ 6] + tz * m[10] + m[14];
}
#endif


/**
 * Matrix3 class.
 */
class Matrix3 {
public:
  Vector3 vx, vy, vz;

  Matrix3() : vx(1,0,0), vy(0,1,0), vz(0,0,1) {}  /// constructor.

  void fromAngle(float ax, float ay, float az)
  {
    float cx = cos(ax); float sx = sin(ax);
    float cy = cos(ay); float sy = sin(ay);
    float cz = cos(az); float sz = sin(az);

    // Calcul de la matrice de transformation 3D
    vx.x = cy*cz; vx.y = cx*sz*cy+sx*sy; vx.z = sx*sz*cy-cx*sy;
    vy.x = -sz;   vy.y = cx*cz;          vy.z = sx*cz;
    vz.x = cz*sy; vz.y = cx*sz*sy-sx*cy; vz.z = sx*sz*sy+cx*cy;
  }
  void apply(const Vector3 &v, Vector3 *out)
  {
    out->x = v*vx;
    out->y = v*vy;
    out->z = v*vz;
  }
  void rapply(Vector3 const &v, Vector3 *out)
  {
    out->x = v.x*vx.x + v.y*vy.x + v.z*vz.x;
    out->y = v.x*vx.y + v.y*vy.y + v.z*vz.y;
    out->z = v.x*vx.z + v.y*vy.z + v.z*vz.z;
  }
  void apply(Vector3 *v)
  {
    Vector3 i(*v);
    apply(i, v);
  }
  void rapply(Vector3 *v)
  {
    Vector3 i(*v);
    rapply(i, v);
  }

};

#endif
