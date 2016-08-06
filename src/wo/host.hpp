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
#ifndef HOST_HPP
#define HOST_HPP

#include "wobject.hpp"

#define HOST_TYPE	17
#define HOST_NAME	"Host"


/**
 * Host class
 */
class Host: public WObject {

public:
  /* properties */
  enum {
    PROPHNAME,
    PROPXY,
    PROPZ,
    PROPAZ,
    PROPAX,
    PROPAY,
    PROPS	///< last item = properties number
  };

  static const OClass oclass;	///< class variable

  virtual const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funlist

  Host(char *l);	///< Constructor

  static WObject * (creator)(char *l);
  /**< Creates from fileline */

  virtual bool whenIntersect(WObject *pcur, WObject *pold);
  /**< When an other object intersects */

  virtual bool updateToNetwork(const Pos &oldpos);
  /**< Publishes new position */

  virtual void quit();
  /**< Quits */

private:
  virtual void parser(char *l);
  /**< Parses */

  // GUI callbacks
  static void connect(Host *o, void *d, time_t s, time_t u);
  static void disconnect(Host *o, void *d, time_t s, time_t u);
};

#endif
