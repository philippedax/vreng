//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
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
#ifndef OFF_HPP
#define OFF_HPP

#include "wobject.hpp"

/**
 * Off class
 */
class Off {

 public:
  Off(float _scale);	///< constructor
  Off(const char *url);	///< constructor
  virtual ~Off();	///< destructor

  GLint displaylist();
  /**< Draws model Off into display list*/

  void draw();
  /**< Draws model Off */

  void setScale(float _scale);
  /**< Sets scale */

  virtual void render(float *color);
  virtual void render(const Pos &pos, float *color);
  /**< renders model Off */

  static void reader(void *aoff, class Http *http);
  /**< Off reader */

 private:
  int vn;	///< Vertice number
  int nn;	///< Normal number
  int pn;	///< Polygon number
  float *v;	///< Vertices
  float *n;	///< Normals
  int   *p;	///< Polygons
  float scale;	///< Scale
  GLint	dlist;	///< displaylist
  char *url;	///< url .off

  static void getOff3i(char *s, int *a, int *b, int *c);
  static void getOff3f(char *s, float *a, float *b, float *c, float scale);

  void defaults();
  /** sets default values */
};

#endif
