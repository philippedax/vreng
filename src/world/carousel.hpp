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
// carousel.hpp
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
class Carousel: public WO {

 private:
  X3d * x3d;
  V3 dim;
  float scale;

 public:
  static const OClass oclass;	///< class variable

  const OClass* getOClass() {return &oclass;}

  Carousel(char *l);	///< Constructor
  ~Carousel() {};	///< Destructor

  static void funcs(); //makes links with the GUI

  static WO * creator(char *l);

  void render();

  bool whenIntersect(WO *pcur, WO *pold);

 private:
  void defaults(); //default values for the object

  void parser(char *l);
  /**< Parses */

  void geometry();
  /**< Makes a solid */

  void behaviors();
  /**< Sets behaviors. */

  void inits();
  /**< Sets initializations. */

  // GUI callbacks
  static void start(Carousel *po, void *d, time_t s, time_t u);
  static void pause(Carousel *po, void *d, time_t s, time_t u);
  static void stop(Carousel *po, void *d, time_t s, time_t u);
  static void setFlashy(Carousel *po, void *d, time_t s, time_t u);
  static void resetFlashy(Carousel *po, void *d, time_t s, time_t u);
};

#endif
