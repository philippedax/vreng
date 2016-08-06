//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2009 Philippe Dax
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
#ifndef MATVEC_HPP
#define MATVEC_HPP


#define V_X v[0]
#define V_Y v[1]
#define V_Z v[2]
#define V_W v[3]

// Rotation axis
enum {
  UX,
  UY,
  UZ
};

/* Matrix & Vector */
typedef struct { float m[4][4]; } M4;
typedef struct { float m[3][3]; } M3;
typedef struct { float v[3]; } V3;
typedef struct { float v[4]; } V4;


void IdM4(M4 *a);
M4 MulM4(M4 a, M4 b);
M4 ScaleM4(float sx, float sy, float sz);
M4 RotateM4(float t, int u);
M4 TranslateM4(float x, float y, float z);
void MulM4V3(V3 *a, const M4 *b, const V3 *c);
void MulM3V4(float m[3][4], float v[4], float dest[3]);
void MulM3M4(float m1[3][4], float m2[4][4], float dest[3][4]);

V3 newV3(float x, float y, float z);
/**< Create a new vector. */

void clearV3(V3 vec);
/**< Clear a vector. */

void copyV3(V3 *to, V3 from);
/**< Copies a vector into another. */

float detV3(V3 d, V3 v, V3 w);
/**< Compute a determinant for a 3x3 matrix that's useful from time to time. */

void M4toV16(const M4 *mat, float *vec);
/**< Transforms a 4x4 matrix to a vec[16]. */

float calcV3(V3 c, V3 e, V3 w, V3 v);
/**< Compute an quantity that's used in the line/surface intersection. */

#if 0 //notused
float * getGLMatrix(const M4 *m4);
/**< Gets a vec[16] from a 4x4 matrix */
V4 newV4(float x, float y, float z, float w);
void MulM4V4(V4 *a, const M4 *b, const V4 * c);
void MulM3V3(V3 *a, const M4 *b, const V3 *c);
void moveM4(M4 *a, const M4 *b);
void moveV3(V3 *a, const V3 *b);
int normV3(V3 *a);
M3 M3_Inv(M3 b);
#endif //notused

#endif
