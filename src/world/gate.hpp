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
// gate.hpp
//---------------------------------------------------------------------------
#ifndef GATE_HPP
#define GATE_HPP

#include "object.hpp"

#define GATE_TYPE	8
#define GATE_NAME	"Gate"


/**
 * Gate class
 */
class Gate: public Object {
 private:
  bool automatic;	///< flag automatic enter
  bool flagentry;	///< flag entry
  bool link;		///< flag link
  char chan[CHAN_LEN];	///< channel
  V3 entry;		///< position after passing gate

 public:
  // properties
  enum {
    PROPHNAME,
    PROPXY,
    PROPZ,
    PROPAZ,
    PROPAX,
    PROPAY,
    PROPS	///< last item = properties number.
  };

  static const OClass oclass;	///< class variable.

  const OClass* getOClass() {return &oclass;}

  static void funcs();		///< init funclist.

  Gate(char *l);		///< Constructor.
  Gate(Object *user, char *geom);	///< Constructor from user.

  static Object * (creator)(char *l);
  /**< Creates from file line. */

  bool intersect(Object *pcur, Object *pold);
  /**< When an other object intersects. */

  bool intersectOut(Object *pcur, Object *pold);
  /**< When an other object leave intersection. */

  bool publish(const Pos &oldpos);
  /**< Publishes new position. */

  void quit();
  /**< Quits properly. */

 private:
  void parser(char *l);
  /**< Parses fileline. */

  void defaults();
  /**< Sets defaults. */

  void behaviors();
  /**< Sets behaviors. */

  void inits();
  /**< Makes initializations. */

  // Actions
  void enter();

  // GUI callbacks
  static void enter_cb(Gate *o, void *d, time_t s, time_t u);
};

#endif
