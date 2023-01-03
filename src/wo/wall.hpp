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
#ifndef WALL_HPP
#define WALL_HPP

#define WALL_TYPE	9
#define WALL_NAME	"Wall"

#include "wobject.hpp"


/**
 * Wall class
 */
class Wall: public WObject {

 public:
  static const uint8_t props;

  static const OClass oclass;	///< class variable

  static void funcs();	///< init funclist

  const OClass* getOClass() {return &oclass;}

  static WObject * (creator)(char *l);
  /**< Creates from fileline */

  Wall(char *l);
  /**< Contructor */

  Wall() {}
  /**< Contructor for sub-classes */

  Wall(WObject *user, char *forme);
  /**< Contructor from user */

  virtual bool whenIntersect(WObject *pcur, WObject *pold);
  /**< When an other object intersects */

 private:
  void parser(char *l);
  /**< Parses fileline */

  // GUI callbacks
  static void destroy(Wall *po, void *d, time_t s, time_t u);
};

#endif
