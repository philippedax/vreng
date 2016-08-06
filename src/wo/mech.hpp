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
#ifndef MECH_HPP
#define MECH_HPP

#include "wobject.hpp"

#define MECH_TYPE 33
#define MECH_NAME "Mech"


/**
 * Mech class
 */
class Mech: public WObject {

 private:
  /* display list definitions */
  enum {
    TORSO,
    HIP,
    SHOULDER,
    UPPER_ARM,
    FOREARM,
    UPPER_LEG,
    FOOT,
    ROCKET,
    VULCAN,
    BODY_PARTS
  };

  float rotx;	///< angle /x
  float roty;	///< angle /y
  float rotz;	///< angle /z
  uint8_t anim;	///< flag animation
  uint8_t walking;	///< flag walking

 public:
  static const OClass oclass;	///< class variable

  virtual const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist

  Mech(char *l);	///< Constructor

  static WObject * (creator)(char *l);
  /**< Creates from fileline */

  virtual void changePermanent(float lasting);
  /**< Permanent motion */

  virtual void render();
  /**< Specific rendering */

  virtual void quit();
  /**< Quits properly */

private:
  virtual void parser(char *l);
  /**< Parses */

  virtual void defaults();
  /**< Default values */

  virtual void makeSolid();
  /**< Builds geometry */

  virtual void draw();
  virtual void display();
  virtual void drawTorso();
  virtual void drawHip();
  virtual void drawShoulder();
  virtual void drawRocketPod();
  virtual void drawUpperArm();
  virtual void drawForeArm();
  virtual void drawUpperLeg();
  virtual void drawLowerLeg();
  virtual void drawFoot();
  virtual void drawVulcanGun();

  static void setMaterial(GLfloat spec[], GLfloat amb[], GLfloat diff[], GLfloat shin[]);

  // GUI callbacks
  static void still(Mech *o, void *d, time_t s, time_t u);
  static void animate(Mech *o, void *d, time_t s, time_t u);
  static void walk(Mech *o, void *d, time_t s, time_t u);
};

#endif
