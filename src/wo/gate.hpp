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
#ifndef GATE_HPP
#define GATE_HPP

#include "wobject.hpp"

#define GATE_TYPE	8
#define GATE_NAME	"Gate"


/**
 * Gate class
 */
class Gate: public WObject {

private:
  bool automatic;	///< flag automatic enter
  bool flagentry;	///< flag entry
  bool link;		///< flag link
  uint8_t cntcol;	///< collide count
  V3 entry;		///< position after passing gate

public:
  /* properties */
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

  static void funcs();	///< init funclist.

  Gate(char *l);	///< Constructor.

  Gate(WObject *user, char *geom);	///< Constructor from user.

  static WObject * (creator)(char *l);
  /**< Creates from file line. */

  bool whenIntersect(WObject *pcur, WObject *pold);
  /**< When an other object intersects. */

  bool whenIntersectOut(WObject *pcur, WObject *pold);
  /**< When an other object leave intersection. */

  bool updateToNetwork(const Pos &oldpos);
  /**< Publishes new position. */

  void quit();
  /**< Quits properly. */

private:
  void parser(char *l);
  /**< Parses fileline. */

  void defaults();
  /**< Sets defaults. */

  void behavior();
  /**< Sets behaviors. */

  void inits();
  /**< Makes initializations. */

  // Actions
  void enter();

  // GUI callbacks
  static void enter_cb(Gate *o, void *d, time_t s, time_t u);
};

#endif
