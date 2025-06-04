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
// bird.hpp
//---------------------------------------------------------------------------
#ifndef BIRD_HPP
#define BIRD_HPP

#define BIRD_TYPE	85
#define BIRD_NAME	"Bird"

#include "object.hpp"

/**
 * Bird class
 */
class Bird : public Object {
 private:
  class Wings *wings;
  bool flying;
  Pos posorig;
  float zone;
  float scale;
  uint8_t model;
  char modelname[16];

 public:
  static const float BIRD_ZONE;
  static const float BIRD_SCALE;
  static const float BIRD_DELTA;

  static const OClass oclass;   ///< class variable

  const OClass* getOClass() {return &oclass;}

  static void funcs();		///< init funclist

  Bird(char *l);	///< Constructor
  ~Bird() {};		///< Destructor

  static Object * (creator)(char *l);
  /**< Creates from fileline */

  void permanent(float lasting);
  /**< Called each frame - check if ths texture has to be updated */

  void render();
  /**< Render the object */

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
  void pause();
  void reset();

  // GUI callbacks
  static void fly_cb(Bird *o, void *d, time_t s, time_t u);
  static void pause_cb(Bird *o, void *d, time_t s, time_t u);
  static void reset_cb(Bird *o, void *d, time_t s, time_t u);
};

#endif
