//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine) 	http://vreng.enst.fr/
//
// Copyright (C) 1997-2008 Philippe Dax
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
//***************************************//
// Author (10/06/01) :                   //
// Ben Humphrey, Game Programmer         //
// DigiBen@GameTutorials.com             //
// Co-Web Host of www.GameTutorials.com  //
//***************************************//
#ifndef VEC3_HPP
#define VEC3_HPP

/**
 * used to store the vertices
 */
class Vec3 {
public:
  float x, y, z;
};

/**
 * used to store the UV coordinates
 */
class Vec2 {
public:
  float x, y;
};

// computes the magnitude of a normal. (magnitude = sqrt(x^2 + y^2 + z^2)
#define Mag(v) (sqrt(v.x*v.x + v.y*v.y + v.z*v.z))

Vec3 Vector(Vec3 p1, Vec3 p2);
/**< calculates a vector between 2 points and returns the result */

Vec3 AddVector(Vec3 v1, Vec3 v2);
/**< adds 2 vectors together and returns the result */

Vec3 DivideVectorByScaler(Vec3 v1, float val);
/**< divides a vector by a single number (scalar) and returns the result */

Vec3 Cross(Vec3 v1, Vec3 v2);
/**< returns the cross product between 2 vectors */

Vec3 Normalize(Vec3 v);
/**< returns the normal of a vector */


/**
 * Vect3D class
 */
class Vect3D {
 public:
  float x,y,z;

  // Constructeurs / destructeur
  inline Vect3D(float ox=0, float oy=0, float oz=0) {
    x = ox;
    y = oy;
    z = oz;
  }

  inline Vect3D(Vect3D * v) {
    x = v->x;
    y = v->y;
    z = v->z;
  }

  virtual ~Vect3D() {}

  // Actions sur les champs
  inline void set(float ox, float oy, float oz) {
    x = ox;
    y = oy;
    z = oz;
  }

  inline void reset() {
    x = 0;
    y = 0;
    z = 0;
  }

  inline float length() {
    return (float) sqrt(x*x + y*y + z*z);
  }

  inline void normalize() {
    float len = length();
    if (len != 0.) {
      float invLen = 1. / len;
      x *= invLen;
      y *= invLen;
      z *= invLen;
    }
  }

  //------------
  // Operateurs
  // -> Addition
  inline friend Vect3D operator + (Vect3D & v1, Vect3D & v2) {
    Vect3D result;
    result.x = v1.x + v2.x;
    result.y = v1.y + v2.y;
    result.z = v1.z + v2.z;
    return result;
  }
  inline void operator += (Vect3D & v) {
    x += v.x;
    y += v.y;
    z += v.z;
  }
  // -> Soustraction
  inline friend Vect3D operator - (Vect3D & v1, Vect3D & v2) {
    Vect3D result;
    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;
    result.z = v1.z - v2.z;
    return result;
  }
  inline void operator -= (Vect3D & v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
  }
  // -> Mise a l'echelle
  friend Vect3D operator * (Vect3D & v, float f) {
    Vect3D result;
    result.x = v.x * f;
    result.y = v.y * f;
    result.z = v.z * f;
    return result;
  }
  inline void operator *= (float v) {
    x *= v;
    y *= v;
    z *= v;
  }
  // -> Mise a l'echelle
  inline friend Vect3D operator * (float f, Vect3D & v) {
    Vect3D result;
    result.x = f * v.x;
    result.y = f * v.y;
    result.z = f * v.z;
    return result;
  }
  inline void operator *= (Vect3D & v) {
    x *= v.x;
    y *= v.y;
    z *= v.z;
  }
  // -> Multiplication matricielle
  inline friend Vect3D operator * (float * m, Vect3D & v) {
    Vect3D result;
    result.x = v.x * m[ 0] + v.y * m[ 4] + v.z * m[ 8] + m[12];
    result.y = v.x * m[ 1] + v.y * m[ 5] + v.z * m[ 9] + m[13];
    result.z = v.x * m[ 2] + v.y * m[ 6] + v.z * m[10] + m[14];
    return result;
  }
  inline void operator *= (float * m) {
    float tx = x;
    float ty = y;
    float tz = z;
    x = tx * m[ 0] + ty * m[ 4] + tz * m[ 8] + m[12];
    y = tx * m[ 1] + ty * m[ 5] + tz * m[ 9] + m[13];
    z = tx * m[ 2] + ty * m[ 6] + tz * m[10] + m[14];
  }
  // -> Mise a l'echelle par division
  inline friend Vect3D operator / (Vect3D & v, float f) {
    Vect3D result;
    float inv = 1. / f;
    result.x = v.x * inv;
    result.y = v.y * inv;
    result.z = v.z * inv;
    return result;
  }
  inline void operator / (float v) {
    float inv = 1. / v;
    x *= inv;
    y *= inv;
    z *= inv;
  }

  //-------------------
  // -> Produit scalaire
  static inline float dotProduct(Vect3D & v1, Vect3D & v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
  }

  static inline float dotProduct(Vect3D * v1, Vect3D * v2) {
    return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
  }
  // -> Produit vectoriel
  inline void crossProduct(Vect3D & v1, Vect3D & v2) {
    x = v1.y * v2.z - v1.z * v2.y;
    y = v1.z * v2.x - v1.x * v2.z;
    z = v1.x * v2.y - v1.y * v2.x;
  }

  inline void crossProduct(Vect3D * v1, Vect3D * v2) {
    x = v1->y * v2->z - v1->z * v2->y;
    y = v1->z * v2->x - v1->x * v2->z;
    z = v1->x * v2->y - v1->y * v2->x;
  }
};

#endif
