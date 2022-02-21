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
#ifndef MAN_HPP
#define MAN_HPP

#include "wobject.hpp"	// Pos

/**
 * Man class
 */
class Man {

public:
  Pos pos;		///< position and BB

  Man();		///< constructor

  virtual ~Man();	///< destructor

  virtual void draw();

private:
  float width;
  float depth;
  float height;
  bool usercontrol;
  GLint dlist;

  static void myMaterial(GLenum mode, float *f, float alpha);

  static void setMaterial(int i);
};

#endif
