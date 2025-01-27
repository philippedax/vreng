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
// off.hpp
//---------------------------------------------------------------------------
#ifndef OFF_HPP
#define OFF_HPP

#include "object.hpp"	// Pos

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

  void render(float *color);
  void render(const Pos &pos, float *color);
  /**< renders model Off */

  static void reader(void *aoff, class Http *http);
  /**< Off reader */

 private:
  int vn;	///< vertices number
  int nn;	///< normals number
  int pn;	///< polygons number
  float *v;	///< vertices
  float *n;	///< normals
  int   *p;	///< polygons
  float scale;	///< scale
  GLint	dlist;	///< displaylist
  char *url;	///< url .off

  void get3i(char *s, int *a, int *b, int *c);
  void get3f(char *s, float *a, float *b, float *c, float scale);

  void defaults();
  /** sets default values */
};

#endif
