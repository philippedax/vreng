//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2009 Philippe Dax
// Telecom-Paris (Ecole Nationale Superieure des Telecommunications)
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
#include "matvec.hpp"


V3 setV3(float x, float y, float z)
{
  V3 v;

  v.v[0] = x;
  v.v[1] = y;
  v.v[2] = z;
  return v;
}

/** Clear a vector */
void clearV3(V3 vec)
{
  vec.v[0] = 0;
  vec.v[1] = 0;
  vec.v[2] = 0;
}

/** Copies a vector into another */
void copyV3(V3 *to, V3 from)
{
  to->v[0] = from.v[0];
  to->v[1] = from.v[1];
  to->v[2] = from.v[2];
}

/** Computes a determinant for a 3x3 matrix */
float detV3(V3 d, V3 v, V3 w)
{
  return (  d.v[0] * (v.v[2] * w.v[1] - v.v[1] * w.v[2])
          + d.v[1] * (v.v[0] * w.v[2] - v.v[2] * w.v[0])
          + d.v[2] * (v.v[1] * w.v[0] - v.v[0] * w.v[1])
         );
}

/** Compute an quantity that's used in the line/surface intersection */
float calcV3(V3 c, V3 e, V3 w, V3 v)
{
  return (  (c.v[0] - e.v[0]) * (v.v[2] * w.v[1] - v.v[1] * w.v[2])
          + (c.v[1] - e.v[1]) * (v.v[0] * w.v[2] - v.v[2] * w.v[0])
          + (c.v[2] - e.v[2]) * (v.v[1] * w.v[0] - v.v[0] * w.v[1])
         );
}

/** identity matrix */
void idM4(M4 *a)
{
  for (int i=0; i<4; i++) {
    for (int j=0; j<4; j++) {
      a->m[i][j] = (i==j) ? 1 : 0;
    }
  }
}

M4 mulM4(M4 a, M4 b)
{
  M4 c;

  for (int i=0; i<4; i++) {
    for (int j=0; j<4; j++) {
      float s = 0;
      for (int k=0; k<4; k++) {
        s += a.m[i][k] * b.m[k][j];
      }
      c.m[i][j] = s;
    }
  }
  return c;
}

M4 scaleM4(float sx, float sy, float sz)
{
  M4 a;

  idM4(&a);
  a.m[0][0] = sx;
  a.m[1][1] = sy;
  a.m[2][2] = sz;
  return a;
}

M4 rotM4(float t, int u)
{
  float s,c;
  int v,w;
  M4 a;

  if ((v = u+1) > 2) v = 0;
  if ((w = v+1) > 2) w = 0;
  s = sin(t);
  c = cos(t);
  idM4(&a);
  a.m[v][v] = c;  a.m[v][w] = -s;
  a.m[w][v] = s;  a.m[w][w] = c;
  return a;
}

M4 transM4(float x, float y, float z)
{
  M4 a;

  idM4(&a);
  a.m[0][3] = x;
  a.m[1][3] = y;
  a.m[2][3] = z;
  return a;
}

/** Returns a = b x c */
void mulM4V3(V3 *a, const M4 *b, const V3 *c)
{
  a->v[0] = b->m[0][0]*c->v[0] + b->m[0][1]*c->v[1] + b->m[0][2]*c->v[2] + b->m[0][3];
  a->v[1] = b->m[1][0]*c->v[0] + b->m[1][1]*c->v[1] + b->m[1][2]*c->v[2] + b->m[1][3];
  a->v[2] = b->m[2][0]*c->v[0] + b->m[2][1]*c->v[1] + b->m[2][2]*c->v[2] + b->m[2][3];
}

/** Matrix times a vector, dest = m*v */
void mulM3V4(float m[3][4], float v[4], float dest[3])
{
  for (int i=0; i<3; i++) {
    dest[i] = 0;
    for (int j=0; j<4; j++) {
      dest[i] += m[i][j] * v[j];
    }
  }
}

/** Matrix multiplication, dest = m1*m2 */
void mulM3M4(float m1[3][4], float m2[4][4], float dest[3][4])
{
  for (int i=0; i<3; i++) {
    for (int j=0; j<4; j++) {
      dest[i][j] = 0;
      for (int k=0; k<4; k++) {
        dest[i][j] += (m1[i][k] * m2[k][j]);
      }
    }
  }
}

void M4toV16(const M4 *mat, float *_vec)
{
  float *vec = _vec;

  for (int i=0; i<4; i++) {
    for (int j=0; j<4; j++) {
      *vec++ = mat->m[j][i];
    }
  }
}

#if 0 //notused ----------------------------------------------------------------------

V4 setV4(float x, float y, float z, float w)
{
  V4 v;

  v.v[0] = x;
  v.v[1] = y;
  v.v[2] = z;
  v.v[3] = w;
  return v;
}

void moveM4(M4 *a, const M4 *b)
{
  memcpy(a, b, sizeof(M4));
}

void moveV3(V3 *a, const V3 *b)
{
  memcpy(a, b, sizeof(V3));
}

void mulM3V3(V3 *a, const M4 *b, const V3 *c)
{
  a->v[0] = b->m[0][0]*c->v[0]+b->m[0][1]*c->v[1]+b->m[0][2]*c->v[2];
  a->v[1] = b->m[1][0]*c->v[0]+b->m[1][1]*c->v[1]+b->m[1][2]*c->v[2];
  a->v[2] = b->m[2][0]*c->v[0]+b->m[2][1]*c->v[1]+b->m[2][2]*c->v[2];
}

void mulM4V4(V4 *a, const M4 *b, const V4 *c)
{
 a->v[0] = b->m[0][0]*c->v[0]+b->m[0][1]*c->v[1]+b->m[0][2]*c->v[2]+b->m[0][3]*c->v[3];
 a->v[1] = b->m[1][0]*c->v[0]+b->m[1][1]*c->v[1]+b->m[1][2]*c->v[2]+b->m[1][3]*c->v[3];
 a->v[2] = b->m[2][0]*c->v[0]+b->m[2][1]*c->v[1]+b->m[2][2]*c->v[2]+b->m[2][3]*c->v[3];
 a->v[3] = b->m[3][0]*c->v[0]+b->m[3][1]*c->v[1]+b->m[3][2]*c->v[2]+b->m[3][3]*c->v[3];
}

float * getGLMatrix(const M4 *vrmat)
{
  float glmat[16], *gl_mat = glmat;

  M4toV16(vrmat, gl_mat);
  return gl_mat;
}

/** Inverse of a 3x3 matrix */
M3 M3_Inv(M3 m)
{
  float det;
  M3 a;

  det = m.m[0][0]*m.m[1][1]*m.m[2][2]-m.m[0][0]*m.m[1][2]*m.m[2][1]-
        m.m[1][0]*m.m[0][1]*m.m[2][2]+m.m[1][0]*m.m[0][2]*m.m[2][1]+
        m.m[2][0]*m.m[0][1]*m.m[1][2]-m.m[2][0]*m.m[0][2]*m.m[1][1];

  a.m[0][0] = (m.m[1][1]*m.m[2][2]-m.m[1][2]*m.m[2][1])/det;
  a.m[0][1] = -(m.m[0][1]*m.m[2][2]-m.m[0][2]*m.m[2][1])/det;
  a.m[0][2] = -(-m.m[0][1]*m.m[1][2]+m.m[0][2]*m.m[1][1])/det;
  a.m[1][0] = -(m.m[1][0]*m.m[2][2]-m.m[1][2]*m.m[2][0])/det;
  a.m[1][1] = (m.m[0][0]*m.m[2][2]-m.m[0][2]*m.m[2][0])/det;
  a.m[1][2] = -(m.m[0][0]*m.m[1][2]-m.m[0][2]*m.m[1][0])/det;
  a.m[2][0] = (m.m[1][0]*m.m[2][1]-m.m[1][1]*m.m[2][0])/det;
  a.m[2][1] = -(m.m[0][0]*m.m[2][1]-m.m[0][1]*m.m[2][0])/det;
  a.m[2][2] = (m.m[0][0]*m.m[1][1]-m.m[0][1]*m.m[1][0])/det;
  return a;
}

/** normalisation d'un vecteur */
int normV3(V3 *v)
{
  float n = sqrt(v->v[0]*v->v[0] + v->v[1]*v->v[1] + v->v[2]*v->v[2]);
  if (n == 0) return 1;
  v->v[0] /= n;
  v->v[1] /= n;
  v->v[2] /= n;
  return 0;
}

#endif //notused
