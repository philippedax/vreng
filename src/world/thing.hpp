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
// thing.hpp
//---------------------------------------------------------------------------
#ifndef THING_HPP
#define THING_HPP

#include "object.hpp"

#define THING_TYPE	4
#define THING_NAME	"Thing"


/**
 * Thing class
 */
class Thing: public Object {
 private:
  static const float LSPEED;
  float lspeed;			///< linear speed
  float aspeed;			///< angular speed

 public:
  class Carrier *carrier;	///< carrier instance

  // properties
  enum {
    PROPHNAME,
    PROPXY,
    PROPZ,
    PROPAZ,
    PROPAX,
    PROPAY,
    PROPS	///< last item = properties number
  };
  // actions
  enum {
    MOVE,
    BASKET,
    APPROACH,
    DESTROY,
    RECREATE,
    NONE
  };

  static const OClass oclass;	///< class variable

  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist

  Thing(char *l);	///< Constructor

  Thing(Object *user, char *form);
  /**< Constructor from GUI */

  Thing(class World *pw, void *d, time_t s, time_t u);
  /**< Constructor from World */

  static Object * (creator)(char *l);
  /**< Creates from a file */

  void permanent(float lasting);
  /**< Permanent movement */

  void timing(time_t s, time_t us, float *lasting);
  /**< Updates lasting time */

  bool publish(const Pos &oldpos);
  /**< Publishes to network */

  bool intersect(Object *pcur, Object *pold);
  /**< Intersects with a mobile object */

  void quit();
  /**< Quits properly */

 private:
  void parser(char *l);
  /**< Parses */

  void defaults();
  /**< Sets default values */

  void setName();
  /**< Sets name */

  void behaviors();
  /**< Sets behaviors flags */

  void moveobject(Thing *o);

  // Funcs callbacks
  static void dropIntoBasket(Thing *o, void *d, time_t s, time_t u);
  static void move_cb(Thing *o, void *d, time_t s, time_t u);
  static void destroy(Thing *o, void *d, time_t s, time_t u);
  static void recreate(class World *w, void *d, time_t s, time_t u);
};

#endif
