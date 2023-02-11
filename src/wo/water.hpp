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
#ifndef WATER_HPP
#define WATER_HPP

#define WATER_TYPE 	32
#define WATER_NAME 	"Water"

#include "wobject.hpp"


/**
 * Water class
 */
class Water: public WObject {

private:
  //static const uint16_t RATE;
  static const float MAX_OFF;
  static const uint8_t MESH;

  float amplitude;	///< wave amplitude
  float freq;		///< number of waves
  float phase;		///< wave speed
  float transparency;	///< water transparency
  float off;		///< offset
  GLuint texid;		///< texture id

public:
  static const float DEF_TRANSP;
  static const float DEF_AMPLITUDE;
  static const float DEF_FREQ;
  static const float DEF_PHASE;
  static const float INCR_AMPLITUDE;
  static const float INCR_FREQ;
  static const float INCR_PHASE;
  static const float INCR_TRANSP;

  static const OClass oclass;	///< class variable.
  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist.

  Water(char *l);	///< Constructor.

  static WObject * (creator)(char *l);
  ///< Creates from fileline.

  virtual void changePermanent(float lasting);
  virtual bool whenIntersect(WObject *pcur, WObject *pold);
  virtual bool whenIntersectOut(WObject *pcur, WObject *pold);
  virtual void render();

private:
  void parser(char *l);
  /**< Parses file line. */

  void defaults();
  /**< Sets default values. */

  void behavior();
  /**< Sets behaviors. */

  void inits();
  /**< Makes initializations. */

  void draw();
  /**< Draws geometry. */

  static void sound_continue(int sig);
  void sound();

  // GUI callbacks
  static void moreAmpl(Water *o, void *d, time_t s, time_t u);
  static void lessAmpl(Water *o, void *d, time_t s, time_t u);
  static void moreFreq(Water *o, void *d, time_t s, time_t u);
  static void lessFreq(Water *o, void *d, time_t s, time_t u);
  static void morePhase(Water *o, void *d, time_t s, time_t u);
  static void lessPhase(Water *o, void *d, time_t s, time_t u);
  static void moreTransp(Water *o, void *d, time_t s, time_t u);
  static void lessTransp(Water *o, void *d, time_t s, time_t u);
  static void reset(Water *o, void *d, time_t s, time_t u);
};

#endif
