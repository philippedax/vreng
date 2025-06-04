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
// projectile.hpp
//---------------------------------------------------------------------------
#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include "object.hpp"

#define PROJECTILE_TYPE	14
#define PROJECTILE_NAME	"Projectile"


/**
 * Projectile class
 */
class Projectile: public Object {
 friend class User;	// access to member "hit"

 protected:
  float lspeed;		///< linear speed
  bool hit;		///< hit: no=0, yes=1

 public:
  // properties
  enum {
    PROPHNAME,
    PROPXY,
    PROPHIT,
    PROPS
  };
  // actions
  enum {
    CREATE
  };

  static const OClass oclass;   ///< class variable

  static void funcs();	///< init funclist

  void imposed(float lasting);

  void timing(time_t s, time_t us, float *plasting);

  bool intersect(Object *pcur, Object *pold);

  void wallsIntersect(Object *pold, V3 *norm);

  bool publish(const Pos &oldpos);
};

#endif
