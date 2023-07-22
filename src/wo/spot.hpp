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
#ifndef SPOT_HPP
#define SPOT_HPP

#define SPOT_TYPE	88
#define SPOT_NAME	"Spot"

#include "wobject.hpp"

/**
 * Spot class
 */
class Spot: public WObject {

private:
  bool on;
  float alpha;
  float dist;
  float base;
  float color[3];

public:
  uint8_t state;

  static const OClass oclass;   ///< class variable

  const OClass* getOClass() {return &oclass;}

  Spot(char *l);	///< Constructor

  static void funcs();	///< init funclist

  static WObject * (creator)(char *l);
  /**< Creates from file line */

  void render();
  /**< Specific rendering */

  void quit();
  /**< Quits properly */

private:
  void behaviors();
  /**< Sets behaviors */

  void parser(char *l);
  /**< Parses */

  void defaults();
  /**< Sets default values */

  void geometry();
  /**< Makes a pseudo solid */

};

#endif
