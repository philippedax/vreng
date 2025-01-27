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
// doc.hpp
//---------------------------------------------------------------------------
#ifndef DOC_HPP
#define DOC_HPP

#include "wobject.hpp"

#define DOC_TYPE	19
#define DOC_NAME	"Doc"


/**
 * Doc class
 */
class Doc: public Object {

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

  static const OClass oclass;   ///< class variable.

  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist.

  Doc(char *l);		///< Constructor.

  static Object * (creator)(char *l);
  /** Creates from fileline. */

  bool whenIntersect(Object *pcur, Object *pold);
  /**< When an intersect occurs. */

  bool publish(const Pos &oldpos);
  /**< Publishes new position. */

  void quit();
  /**< Quits. */

 private:
  void parser(char *l);
  /**< Parses fileline. */

  // Callbacks
  static void view_cb(Doc *o, void *d, time_t s, time_t u);
  static void print_cb(Doc *o, void *d, time_t s, time_t u);
  static void cancel_cb(Doc *o, void *d, time_t s, time_t u);
};

#endif
