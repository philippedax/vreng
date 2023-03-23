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
#ifndef WALLS_HPP
#define WALLS_HPP

#define WALLS_TYPE 20
#define WALLS_NAME "walls"

#include "wobject.hpp"


/**
 * Walls class
 */
class Walls: public WObject {

 public:
  float xs, xe;		///< pos x start, end
  float ys, ye;		///< pos y start, end
  float zs, ze;		///< pos z start, end
  float r, g, b;	///< wall color
  GLint dlist;		///< walls display list
  class Walls *next;	///< next walls

  static const OClass oclass;	///< class variable

  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist

  Walls(char *l);	///< Constructor

  Walls(float sx, float ex, float sy, float ey, float sz, float ez, float r, float g, float b);
  /**< Constructor for one wall */

  static WObject * (creator)(char *l);	///< Creates from fileline

  void draw();	///< Draws walls

  void render(); ///< Renders the walls

  void quit();	///< Quits

  static int whenIntersect(const V3& center, const V3& size, V3& norm);

private:
  void parser(char *l);
  /**< Parses */

  void defaults();
  /**< Sets default values */

  void makeSolid();
  /**< Sets solid values */

  const char * getUrl() const;

  static void reader(void *va, class Http *http);
  /**< download walls */

};

#endif
