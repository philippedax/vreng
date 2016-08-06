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
  static const uint16_t RATE;
  static const float MAX_OFF;
  static const uint8_t MESH;

  float rotx, roty, rotz;	///< rotations x y z
  float width, depth, height;	///< dimensions
  float off;			///< offset
  bool play;			///< flag play

  float amplitude;	///< wave amplitude
  float freq;		///< number of waves
  float phase;		///< wave speed
  float transparency;	///< water transparency

public:
  static const float DEF_TRANSP;
  static const float DEF_HEIGHT;
  static const float DEF_AMPLITUDE;
  static const float DEF_FREQ;
  static const float DEF_PHASE;
  static const float INCR_AMPLITUDE;
  static const float INCR_FREQ;
  static const float INCR_PHASE;
  static const float INCR_TRANSP;

  static const OClass oclass;	///< class variable.
  virtual const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist.

  Water(char *l);	///< Constructor.

  static WObject * (creator)(char *l);
  ///< Creates from fileline.

  virtual void changePermanent(float lasting);
  virtual bool whenIntersect(WObject *pcur, WObject *pold);
  virtual bool whenIntersectOut(WObject *pcur, WObject *pold);
  virtual void render();

private:
  virtual void parser(char *l);
  /**< Parses file line. */

  virtual void defaults();
  /**< Sets default values. */

  virtual void behavior();
  /**< Sets behaviors. */

  virtual void init();
  /**< Makes initializations. */

  virtual void draw();
  /**< Draws geometry. */

  static void sound_continue(int sig);
  virtual void sound();

  // GUI callbacks
  static void moreAmplitude(Water *o, void *d, time_t s, time_t u);
  static void lessAmplitude(Water *o, void *d, time_t s, time_t u);
  static void moreFrequence(Water *o, void *d, time_t s, time_t u);
  static void lessFrequence(Water *o, void *d, time_t s, time_t u);
  static void morePhase(Water *o, void *d, time_t s, time_t u);
  static void lessPhase(Water *o, void *d, time_t s, time_t u);
  static void moreTransp(Water *o, void *d, time_t s, time_t u);
  static void lessTransp(Water *o, void *d, time_t s, time_t u);
  static void reset(Water *o, void *d, time_t s, time_t u);
};

#endif
