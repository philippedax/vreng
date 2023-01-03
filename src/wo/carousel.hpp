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
#ifndef CAROUSEL_HPP
#define CAROUSEL_HPP

#include "wobject.hpp"
#include "x3d.hpp"

#define CAROUSEL_TYPE	53
#define CAROUSEL_NAME	"carousel"


/**
 * Carousel class
 */
class Carousel: public WObject {

private:
  X3d * x3d;
  float dimx, dimy, dimz;
  float scale;

public:

  static const OClass oclass;	///< class variable

  const OClass* getOClass() {return &oclass;}

  Carousel(char *l);	///< Constructor
  ~Carousel() {};	///< Destructor

  static void funcs(); //makes links with the GUI

  static WObject * creator(char *l);

  void render();

  bool whenIntersect(WObject *pcur, WObject *pold);

  static X3d * getx3d();

private:

  void defaults(); //default values for the object

  void parser(char *l);
  /**< Parses */

  void makeSolid();
  /**< Makes a solid */

  // GUI callbacks
  static void start(Carousel *po, void *d, time_t s, time_t u);
  static void pause(Carousel *po, void *d, time_t s, time_t u);
  static void stop(Carousel *po, void *d, time_t s, time_t u);
  static void setFlashy(Carousel *po, void *d, time_t s, time_t u);
  static void resetFlashy(Carousel *po, void *d, time_t s, time_t u);
};

#endif
