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
#ifndef ANIMX3D_HPP
#define ANIMX3D_HPP

#include "wobject.hpp"
#include "x3d.hpp"

#define ANIMX3D_TYPE	53
#define ANIMX3D_NAME	"animx3d"


/**
 * AnimX3d class
 */
class AnimX3d: public WObject {

protected:
  X3d *x3dmodel;
  float dimx,dimy,dimz;

public:

  static const OClass oclass;	///< class variable

  virtual const OClass* getOClass() {return &oclass;}

  AnimX3d(char *l);	///< Constructor

  static void funcs(); //makes links with the GUI

  static WObject * creator(char *l);

  void render();

  bool whenIntersect(WObject *pcur, WObject *pold);

  static X3d * current();

private:

  void defaults(); //default values for the object

  virtual void parser(char *l);
  /**< Parses */

  // GUI callbacks
  static void start(AnimX3d *po, void *d, time_t s, time_t u);
  static void pause(AnimX3d *po, void *d, time_t s, time_t u);
  static void stop(AnimX3d *po, void *d, time_t s, time_t u);
  static void setFlashy(AnimX3d *po, void *d, time_t s, time_t u);
  static void resetFlashy(AnimX3d *po, void *d, time_t s, time_t u);
};

#endif
