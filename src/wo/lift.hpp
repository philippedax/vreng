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
#ifndef LIFT_HPP
#define LIFT_HPP

#include "wobject.hpp"

#define LIFT_TYPE	50
#define LIFT_NAME	"Lift"


/**
 * Lift class
 */
class Lift: public WObject {

private:
  static const int DEF_TOP;
  static const int DEF_BOT;
  static const float DEF_HEIGHT;
  static const float DEF_SPEED;
  static const float DEF_STEP;
  static const float DENSITY;

  int bot;		///< bottom level
  int top;		///< top level
  float height;		///< level height
  float speed;		///< vertical speed
  float dest;		///< destination

public:
  /* properties */
  enum {
    PROPZ,
    PROPHNAME,
    PROPS
  };

  static const OClass oclass;	///< class variable

  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist

  Lift(char *l);	///< Constructor

  static WObject * (creator)(char *l);

  virtual void updateTime(time_t sec, time_t usec, float *lasting);

  virtual void changePosition(float lasting);

  virtual bool updateToNetwork(const Pos &oldpos);

  virtual bool whenIntersect(WObject *pcur, WObject *pold);

  virtual bool whenIntersectOut(WObject *pcur, WObject *pold);

  virtual void quit();

private:
  void parser(char *l);
  /**< Parses */

  void defaults();
  /**< set default values */

  void up();
  void down();
  void call();

  // GUI callbacks
  static void up_cb(Lift *o, void *d, time_t s, time_t u);
  static void down_cb(Lift *o, void *d, time_t s, time_t u);
  static void call_cb(Lift *o, void *d, time_t s, time_t u);
};

#endif
