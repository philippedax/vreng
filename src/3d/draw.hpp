//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2012 Philippe Dax
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
// draw.hpp
//---------------------------------------------------------------------------
#ifndef DRAW_HPP
#define DRAW_HPP


/**
 * Draw simple 3D shapes : points, lines, cube, sphere, cylinder...
 */
class Draw {
 public:
  // draw styles
  enum {
    STYLE_FILL,
    STYLE_LINES,
    STYLE_POINTS
  };

  static void vertex3fv(const GLfloat *v);
  static void vertex3f(GLfloat x, GLfloat y, GLfloat z);

  static void point(GLfloat x, GLfloat y, GLfloat z);
  /**< Draws a point. */

  static void line(GLfloat l, GLfloat w);
  /**< Draws a line.*/

  static void rect(GLfloat w, GLfloat h, uint8_t style, GLfloat rTxs=1, GLfloat rTxt=1);
  /**< Draws a rectangle. */

  static void ellipse(GLfloat rx, GLfloat ry, uint8_t style);
  /**< Draws an ellipse. */

  static void triangle(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat x3, GLfloat y3, GLfloat z3);
  /**< Draws a triangle. */

  static void bbox(GLfloat width, GLfloat depth, GLfloat height);
  /**< Draws a wired bounding box. */

  static void box(GLfloat width, GLfloat depth, GLfloat height, const int textures[], GLfloat rTx[6][2], uint8_t style);
  /**< Draws a textured box. */

  static void box(GLfloat width, GLfloat depth, GLfloat height);
  /**< Draws a box. */

  static void pentagon(GLfloat side, GLfloat height, uint8_t style);
  /**< Draws a pentagon. */

  static void pyramid(GLfloat side, uint8_t style);
  /**< Draws a pyramid. */

  static void octagon(GLfloat side, GLfloat height, uint8_t style);
  /**< Draws a octagon. */

  static void torus(GLfloat rc, uint8_t numc, GLfloat rt, uint8_t numt, uint8_t style);
  /**< Draws a torus. */

  static void sphere(GLfloat radius, uint8_t slices, uint8_t stacks, uint8_t style);
  /**< Draws a sphere. */

  static void cylinder(GLfloat baseR, GLfloat topR, GLfloat height, uint8_t slices, uint8_t stacks, uint8_t style);
  /**< Draws a cylinder or cone. */

  static void disk(GLfloat inR, GLfloat outR, uint8_t slices, uint8_t loops, uint8_t style);
  /**< Draws a disk. */

  static void ray(GLint *dl, GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, const GLfloat *color, GLint stipple);
  /**< Draws a stipple beam in displaylist. */

  static void ray(GLint *dl, GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2);
  /**< Draws a full beam in displaylist. */

  static void helix(GLfloat r, GLfloat length, GLfloat height, GLfloat slices, GLfloat thick, GLfloat *color);

};


#endif
