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
// mech.hpp
//---------------------------------------------------------------------------
#ifndef MECH_HPP
#define MECH_HPP

#include "object.hpp"

#define MECH_TYPE 33
#define MECH_NAME "Mech"


/**
 * Mech class
 */
class Mech: public Object {
 private:
  /* display list definitions */
  enum {
    TORSO,
    HIP,
    SHOULDER,
    UPPER_ARM,
    FOREARM,
    UPPER_LEG,
    LOWER_LEG,
    FOOT,
    ROCKET,
    VULCAN,
    BODY_PARTS
  };

  float rotx;	///< angle /x
  float roty;	///< angle /y
  float rotz;	///< angle /z
  bool anim;	///< flag animation
  bool walking;	///< flag walking

 public:
  static const OClass oclass;	///< class variable

  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist

  Mech(char *l);	///< Constructor

  static Object * (creator)(char *l);
  /**< Creates from fileline */

  void changePermanent(float lasting);
  /**< Permanent motion */

  void render();
  /**< Specific rendering */

  void quit();
  /**< Quits properly */

 private:
  void parser(char *l);
  /**< Parses */

  void defaults();
  /**< Default values */

  void geometry();
  /**< Builds geometry */

  void behaviors();
  /**< Sets behaviors */

  void inits();
  /**< Initializations */

  void draw();
  void display();
  void drawTorso();
  void drawHip();
  void drawShoulder();
  void drawRocketPod();
  void drawUpperArm();
  void drawForeArm();
  void drawUpperLeg();
  void drawLowerLeg();
  void drawFoot();
  void drawVulcanGun();

  static void setMaterial(GLfloat spec[], GLfloat amb[], GLfloat diff[], GLfloat shin[]);

  // GUI callbacks
  static void still(Mech *o, void *d, time_t s, time_t u);
  static void animate(Mech *o, void *d, time_t s, time_t u);
  static void walk(Mech *o, void *d, time_t s, time_t u);
};

#endif
