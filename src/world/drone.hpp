//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2021 Philippe Dax
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
#ifndef DRONE_HPP
#define DRONE_HPP

#define DRONE_TYPE	86
#define DRONE_NAME	"Drone"

#include "wobject.hpp"

/**
 * Drone class
 */
class Drone : public WO {

 private:
  class Wings *wings;
  bool flying;		///< the drone is flying
  bool following;	///< following from the drone
  bool viewing;		///< the drone is viewing
  bool driven;		///< the drone is under control of user
  float zone;		///< zone to explore
  float scale;		///< scaling factor
  float color[3];	///< color
  uint8_t model;	///< model of wings
  Pos posorig;		///< initial position

 public:
  static const float DRONE_ZONE;
  static const float DRONE_SCALE;
  static const float DRONE_DELTA;

  static const OClass oclass;   ///< class variable

  const OClass* getOClass() {return &oclass;}

  static void funcs();		///< init funclist

  Drone(char *l);
  /**< Constructor */

  static WO * (creator)(char *l);
  /**< Creates from fileline */

  void changePermanent(float lasting);
  /**< Called each frame - check if ths texture has to be updated */

  void render();
  /**< Render the object */

  void quit();
  /**< Called when the object is destroy */

 private:
  void parser(char *l);
  /**< Parses */

  void defaults();
  /**< Sets defaults values */

  void behaviors();
  /**< Sets behaviors */

  void inits();
  /**< Do specific inits */

  void fly();
  void follow();
  void drive();
  void pause();
  void view();
  void reset();

  // GUI callbacks
  static void fly_cb(Drone *o, void *d, time_t s, time_t u);
  static void follow_cb(Drone *o, void *d, time_t s, time_t u);
  static void drive_cb(Drone *o, void *d, time_t s, time_t u);
  static void pause_cb(Drone *o, void *d, time_t s, time_t u);
  static void view_cb(Drone *o, void *d, time_t s, time_t u);
  static void reset_cb(Drone *o, void *d, time_t s, time_t u);

};

#endif
