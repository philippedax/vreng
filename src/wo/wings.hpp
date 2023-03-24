//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2011 Philippe Dax
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
#ifndef WINGS_HPP
#define WINGS_HPP

#include "wobject.hpp"
#include "cloth.hpp"

#define WINGS_TYPE	74
#define WINGS_NAME	"Wings"

struct sWings {
  uint8_t wings_id;
  const char wings_str[16];
};


/**
 * Wings class
 */
class Wings: public Cloth {

private:
  GLfloat angle;	///< angle
  GLint dlist_center;	///< display-list center
  GLint dlist_right;	///< display-list right wing
  GLint dlist_left;	///< display-list left wing
  GLfloat scale;	///< scale
  float color[3];	///< color
  bool active;		///< is in movement ?

protected:
  uint8_t model;
  char modelname[16];

public:
  enum {
    NOWINGS,
    BIRD,
    BUTTERFLY,
    LIBELLULE,
    ANGEL,
    EAGLE,
    HELICOPTER,
    UNKNOWN
  };

  enum {
    WEAR,
    TAKEOFF,
    MOVE,
    CREATE_WEAR,
    RECREATE,
    DESTROY
  };

  static const OClass oclass;	///< class variable.

  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist.

  static WObject * (creator)(char *l);
  /**< Creation from a file. */

  Wings(char *l);		///< Constructor file.
  Wings(class User *user, void *d, time_t s, time_t u);
  /**< Constructor from User */
  Wings();			///< Constructor from bird.
  Wings(uint8_t model, float scale);		///< Constructor from bird.
  Wings(uint8_t model, float scale, float *color);	///< Constructor from drone.

  void changePermanent(float lasting);
  /**< Changes. */

  void start();
  /**< Starts movement. */

  void stop();
  /**< Stops movement. */

  void draw();
  void draw(uint8_t model);
  /**< Draws. */

  void render();
  void render(uint8_t model);
  /**< Renders. */

  void quit();
  /**< Quits. */
  
private:
  void defaults();
  /**< Sets default values. */

  void parser(char *l);
  /**< Parses file line. */

  void makeSolid();
  /**< Builds solid geometry. */

  uint8_t getModel(const char *name);
  /**< Gets model id. */

  void wear();
  void takeoff();

  // callbacks
  static void wear_cb(Wings *wings, void *d, time_t s, time_t u);
  static void takeoff_cb(Wings *wings, void *d, time_t s, time_t u);

  static void recreate_cb(class User *user, void *d, time_t s, time_t u);

};

#endif
