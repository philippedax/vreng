//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2008 Philippe Dax
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
#ifndef TEAPOT_HPP
#define TEAPOT_HPP

typedef GLfloat point[3];


/**
 * Teapot class
 */
class Teapot {
 private:
  static const point vertices[306];
  static const int indices[32][4][4];

  uint8_t level; 
  point data[32][4][4];

 public:
  Teapot();		///< constructor
  virtual ~Teapot() {}	///< destructor

  void draw(GLfloat width, GLfloat depth, GLfloat height, const int textures[], GLfloat rtex[6][2], uint8_t slices, uint8_t style);
  /**< Draws a teapot */

 private:
  void transpose(point p[4][4]);
  void draw_patch(point p[4][4]);
  void divide_patch(point p[4][4], uint8_t n);
  void divide_curve(point *c, point *r, point *l);
  void normal(point n, point p, point q, point r);
};

#endif
