//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2012 Philippe Dax
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

  virtual const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist.

  Slider(char *l);	///< Constructor.

  static WObject * (creator)(char *l);
  /**< Create from fileline. */

  virtual bool isMoving();
  /**< Checks if object is moving. */

  virtual void changePosition(float lasting);
  /**< Changes the position after a triggered movement. */

  virtual void updateTime(time_t sec, time_t usec, float *lasting);
  /**< Updates remaining times of the movement. */

  virtual bool whenIntersect(WObject *pcur, WObject *pold);
  /**< When an other object intersects. */

  virtual void quit();
  /**< Quits. */

private:
  float incrx;
  float incry;
  float incrz;
  WObject *capted;	///< captured object.

  virtual void parser(char *l);
  /**< Parses fileline. */

  virtual void defaults();
  /**< Default values. */

  virtual void behavior();
  /**< Sets behavior. */
};

#endif
