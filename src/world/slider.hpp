//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2012 Philippe Dax
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
// slider.hpp
//---------------------------------------------------------------------------
#ifndef SLIDER_HPP
#define SLIDER_HPP

#include "wobject.hpp"
#include "step.hpp"

#define SLIDER_TYPE	78
#define SLIDER_NAME	"Slider"

/**
 * Slider class
 */
class Slider: public Step {
 public:
  static const OClass oclass;	///< class variable.

  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist.

  Slider(char *l);	///< Constructor.

  static Object * (creator)(char *l);
  /**< Create from fileline. */

  bool isMoving();
  /**< Checks if object is moving. */

  void changePosition(float lasting);
  /**< Changes the position after a triggered movement. */

  void updateTime(time_t sec, time_t usec, float *lasting);
  /**< Updates remaining times of the movement. */

  bool whenIntersect(Object *pcur, Object *pold);
  /**< When an other object intersects. */

  void quit();
  /**< Quits. */

 private:
  float incrx;
  float incry;
  float incrz;
  Object *capted;	///< captured object.

  void parser(char *l);
  /**< Parses fileline. */

  void defaults();
  /**< Default values. */

  void behaviors();
  /**< Sets behaviors. */
};

#endif
