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
#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include "wobject.hpp"

#define PROJECTILE_TYPE	14
#define PROJECTILE_NAME	"Projectile"


/**
 * Projectile class
 */
class Projectile: public WObject {
  friend class User; // access to member "hit"

 protected:
  float lspeed;		///< linear speed
  bool hit;		///< hit: no=0, yes=1

 public:
  /* properties */
  enum {
    PROPHNAME,
    PROPXY,
    PROPHIT,
    PROPS
  };

  /* actions */
  enum {
    CREATE
  };

  static const OClass oclass;   ///< class variable

  static void funcs();	///< init funclist

  void changePosition(float lasting);

  void updateTime(time_t s, time_t us, float *plasting);

  bool whenIntersect(WObject *pcur, WObject *pold);

  void whenWallsIntersect(WObject *pold, V3 *norm);

  bool updateToNetwork(const Pos &oldpos);

  void quit() {};
  /**< Quits. */
};

#endif
