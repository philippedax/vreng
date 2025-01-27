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
// cauldron.hpp
//---------------------------------------------------------------------------
#ifndef CAULDRON_HPP
#define CAULDRON_HPP

#include "object.hpp"

#define CAULDRON_TYPE	13
#define CAULDRON_NAME	"Cauldron"


/**
 * Cauldron class
 */
class Cauldron: public Object {

 private:
  static const float DELTAZ;

 public:
  static const OClass oclass;	///< class variable

  static void funcs();	///< init funclist

  const OClass* getOClass() {return &oclass;} ///< inst. method

  Cauldron(char *l);	///< Constructor

  static Object * (creator)(char *l);
  /**< Create from fileline */

  bool whenIntersect(Object *pcur, Object *pold);
  /**< When an other object intersects */

  void changePermanent(float lasting);
  /**< Change permanent */

 private:
  void parser(char *l);
  /**< Parses */

  // Callbacks
  static void create_cb(Cauldron *o, void *d, time_t s, time_t u);
};

#endif
