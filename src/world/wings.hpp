//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2011 Philippe Dax
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
// wings.hpp
//---------------------------------------------------------------------------
#ifndef WINGS_HPP
#define WINGS_HPP

#include "object.hpp"
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
class Wings: public Object {
 private:
  bool taken;                           ///< bool taken by avatar
  float ttl;                            ///< time to live with avatar
  float dx, dy, dz, dax, day, daz;      ///< difference with avatar positions
  float ox, oy, oz, oax, oay, oaz;      ///< original positions
  float aspeed;				///< angular speed

  GLfloat angle;	///< angle
  GLint dlist_center;	///< display-list center
  GLint dlist_right;	///< display-list right wing
  GLint dlist_left;	///< display-list left wing
  GLfloat scale;	///< scale
  float color[3];	///< color
  bool active;		///< is in movement ?
  uint8_t article;                      ///< types of article

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

  /* net properties */  
  enum {
    PROPHNAME,
    PROPXY,
    PROPZ,
    PROPAZ,
    PROPAX,
    PROPAY,
    PROPS       ///< last item = properties number
  };

  static const OClass oclass;	///< class variable.

  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist.

  static Object * (creator)(char *l);
  /**< Creation from a file. */

  Wings(char *l);		///< Constructor file.
  Wings(class User *user, void *d, time_t s, time_t u);
  /**< Constructor from User */
  Wings();			///< Constructor from bird.
  Wings(uint8_t model, float scale, float aspeed);			///< Constructor from bird.
  Wings(uint8_t model, float scale, float aspeed, float *color);	///< Constructor from drone.

  void changePermanent(float lasting);
  /**< Changes. */

  void draw();
  void draw(uint8_t model);
  /**< Draws. */

  void render();
  void render(uint8_t model);
  /**< Renders. */

  void quit();
  /**< Quits. */

  void start();
  /**< Starts movement. */

  void stop();
  /**< Stops movement. */
  
 private:
  void defaults();
  /**< Sets default values. */

  void parser(char *l);
  /**< Parses file line. */

  void behaviors();
  /**< Sets behaviors. */

  void inits();
  /**< Sets initializations. */

  void geometry();
  /**< Builds solid geometry. */

  uint8_t getModel(const char *name);
  /**< Gets model id. */

  void reset();
  /**< Restores original position */

  void wear();
  void takeoff();

  // callbacks
  static void wear_cb(Wings *wings, void *d, time_t s, time_t u);
  static void takeoff_cb(Wings *wings, void *d, time_t s, time_t u);

  static void recreate_cb(class User *user, void *d, time_t s, time_t u);

};

#endif
