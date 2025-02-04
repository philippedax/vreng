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
// ball.hpp
//---------------------------------------------------------------------------
#ifndef BALL_HPP
#define BALL_HPP

#include "object.hpp"

#define BALL_TYPE	5
#define BALL_NAME	"Ball"

#define DEF_URL_BALL	"/gif/ball.gif"		// ball by default


/**
 * Ball class
 */
class Ball: public Object {
 private:
  float lspeed;		///< horizontal speed in xy
  float zspeed;		///< vertical speed
  float aspeed;		///< angular speed
  float gravity;	///< gravity in z
  bool taken;		///< taken or not by an user
  float ttl;		///< time to live

 public:
  static const float LSPEED;
  static const float ZSPEED;
  static const float ASPEED;
  static const float SHIFT;
  static const float RADIUS;
  static const float DELTAZ;
  static const float TTL;

  /* properties */
  enum {
    PROPHNAME,
    PROPXY,
    PROPZ,
    PROPAZ,
    PROPAX,
    PROPAY,
    PROPS
  };

  /* actions */
  enum {
    PUSH,	///< forward
    PULL,	///< backward
    SHOOT,	///< shoot
    UP,		///< move up
    TAKE,	///< take
    DROP,	///< drop
    TURN,	///< turn
    KILL,	///< destroy
    CREATE,	///< create
    RECREATE,	///< create
    NONE
  };

  static const OClass oclass;	///< class variable

  static void funcs();	///< init funclist

  const OClass* getOClass() {return &oclass;}

  Ball(class Object *cauldron, void *d, time_t s, time_t u);
  /**< constructor: called by cauldron. */

  Ball(class World *world, void *d, time_t s, time_t u);
  /**< constructor: called by world. */

  Ball(uint8_t type_id, Noid noid, Payload *pp);
  /**< constructor: network replication. */

  Ball(Object *user, char *solid);
  /**< constructor: created by user. */

  Ball(char *l);
  /**< constructor: fileline. */

  static Object * replicator(uint8_t type_id, Noid noid, Payload *pp);
  /**< Replicates a ball coming from the Network. */

  static Object * (creator)(char *l);
  /**< Creates a ball defined in a file. */

  bool isMoving();
  /**< Checks if object is moving. */

  void imposed(float lasting);
  /**< Does any position changes. */

  void permanent(float lasting);
  /**< Does permanent position changes. */

  void timing(time_t s, time_t u, float *lasting);
  /**< Updates times. */

  bool publish(const Pos &oldpos);
  /**< Publishes to network. */

  bool intersect(Object *pcur, Object *pold);
  /**< Handles collisions. */

  void wallsIntersect(Object *pold, V3 *norm);
  /**< Handles collisions with a wall. */

  void quit();
  /**< Quits. */

 private:
  void defaults();
  /**< Sets defaults values. */

  void parser(char *l);
  /**< Parses fileline. */

  void behaviors();
  /**< Sets behaviors. */

  void inits();
  /**< Sets initializations. */

  void geometry();
  /**< Builds solid geometry. */

  void setName();
  /**< Sets an implicited name. */

  // Actions
  void push();
  void pull();
  void shoot();
  void up();
  void take();
  void drop();
  void turn();
  void destroy();

  // GUI callbacks
  static void push_cb(Ball *o, void *d, time_t s, time_t u);
  static void pull_cb(Ball *o, void *d, time_t s, time_t u);
  static void shoot_cb(Ball *o, void *d, time_t s, time_t u);
  static void up_cb(Ball *o, void *d, time_t s, time_t u);
  static void take_cb(Ball *o, void *d, time_t s, time_t u);
  static void drop_cb(Ball *o, void *d, time_t s, time_t u);
  static void turn_cb(Ball *o, void *d, time_t s, time_t u);
  static void destroy_cb(Ball *o, void *d, time_t s, time_t u);
  static void create_cb(class Cauldron *c, void *d, time_t s, time_t u);
  static void recreate_cb(class World *w, void *d, time_t s, time_t u);
};

#endif
