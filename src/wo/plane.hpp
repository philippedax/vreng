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
#ifndef PLANE_HPP
#define PLANE_HPP

#include "wobject.hpp"

#define PLANE_TYPE	2
#define PLANE_NAME	"Plane"


/**
 * Plane class
 */
class Plane: public WObject {

public:
 /* properties */
  enum {
    PROPHNAME,
    PROPXY,
    PROPZ,
    PROPAZ,
    PROPAX,
    PROPAY,
    PROPS       ///< last item = properties number.
  };

  static const OClass oclass;	///< class variable.

  const OClass* getOClass() {return &oclass;}

  static WObject * (creator)(char *l);
  /**< Creates from fileline. */

  Plane(char *l);
  /**< Contructor */

  static void funcs();	///< init funclist.

  bool whenIntersect(WObject *pcur, WObject *pold);
  /**< When an other object intersects. */

  void quit();
  /**< Quits. */

private:
  char *tex;	///< texture string.

  void parser(char *l);
  /**< Parses file line. */

  void makeSolid();
  /**< Makes a new solid. */

};

#endif
