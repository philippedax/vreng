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
#ifndef MIRROR_HPP
#define MIRROR_HPP

#include "wobject.hpp"

#define MIRROR_TYPE	29
#define MIRROR_NAME	"Mirror"

/**
 * Mirror class
 */
class Mirror: public WObject {

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

  bool state;	///< on or off

  static const OClass oclass;	///< class variable

  virtual const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist

  Mirror(char *l);	///< Constructor

  static WObject * (creator)(char *l);

  virtual bool whenIntersect(WObject *pcur, WObject *pold);
  /**< When an intersection occurs */

  virtual bool updateToNetwork(const Pos &oldpos);
  /**< Publishes new position */

  virtual void render();
  /**< Renders mirrored scene. */

  virtual void quit();
  /**< Quits properly */

private:
  virtual void defaults();
  virtual void behavior();

  virtual void parser(char *l);
  /**< Parses file line */

  virtual void mirroredScene();

  // GUI callbacks
  static void mirrorOn(Mirror *po, void *d, time_t s, time_t u);
  static void mirrorOff(Mirror *po, void *d, time_t s, time_t u);
};

#endif
