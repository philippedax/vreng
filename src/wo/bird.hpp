//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://www.vreng.enst.fr/
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
#ifndef BIRD_HPP
#define BIRD_HPP

#define BIRD_TYPE	85
#define BIRD_NAME	"Bird"

#include "wobject.hpp"

/**
 * Bird class
 */
class Bird : public WObject {

 private:
  bool flying;
  class Wings *wings;
  Pos posinit;
  float radius;
  float scale;
  uint8_t model;
  char modelname[16];

 public:
  static const float BIRD_ZONE;
  static const float BIRD_SCALE;

  static const OClass oclass;   ///< class variable

  virtual const OClass* getOClass() {return &oclass;}

  static void funcs();		///< init funclist

  Bird(char *l);
  /**< Constructor */

  static WObject * (creator)(char *l);
  /**< Creates from fileline */

  virtual void changePermanent(float lasting);
  /**< Called each frame - check if ths texture has to be updated */

  virtual void render();
  /**< Render the object */

  virtual void quit();
  /**< Called when the object is destroy */

 private:
  virtual void parser(char *l);
  /**< Parses */

  virtual void defaults();
  /**< Sets defaults values */

  virtual void behavior();
  /**< Sets behavior */

  virtual void inits();
  /**< Do specific inits */

  virtual void fly();
  virtual void pause();
  virtual void reset();

  // GUI callbacks
  static void fly_cb(Bird *o, void *d, time_t s, time_t u);
  static void pause_cb(Bird *o, void *d, time_t s, time_t u);
  static void reset_cb(Bird *o, void *d, time_t s, time_t u);

};

#endif
