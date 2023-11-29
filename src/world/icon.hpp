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
#ifndef ICON_HPP
#define ICON_HPP

#include "wobject.hpp"

class Wall;

#define ICON_TYPE	45
#define ICON_NAME	"Icon"


/**
 * Icon class
 */
class Icon: public WO {

private:
  static const float DEPTH;
  static const float LSPEED;
  static const float ZSPEED;
  static const float ASPEED;

  char *ifile;		///< local input file
  char *ofile;		///< public output file
  char *vref;		///< vref entry to restore

  static const float TTL;
  static const float WIDTH;
  static const float HEIGHT;

  float lspeed;		///< linear speed in xy
  float aspeed;		///< angular speed
  float zspeed;		///< linear speed in z
  bool remove;		///< flag to remove
  float ttl;		///< time to live
  bool taken;		///< taken or not by an user
  char *tex;		///< texture of the icon

 public:
  /* properties */
  enum {
    PROPHNAME,
    PROPXY,
    PROPZ,
    PROPAZ,
    PROPAX,
    PROPAY,
    PROPURL,
    PROPTEX,
    PROPGNAME,
    PROPS		///< last item = properties number
  };

  /* actions */
  enum {
    OPEN,		///< open document
    SAVE,		///< save document
    PIN,		///< throw icon
    LEAVE,		///< leave here
    CARRY,		///< carry icon
    KILL,		///< remove icon
    PUSH,		///< push icon
    PULL,		///< pull icon
    DROP,		///< drop icon
    TURN,		///< turn
    MOVE,		///< position manipulation
    CREATE,		///< create icon
    STICK,		///< stick icon
    NONE
  } actions;

  static const OClass oclass;	///< class variable

  static void funcs();		///< init funclist

  const OClass* getOClass() {return &oclass;}

  Icon(class User *pu, void *data);
  /**< constructor: called by GUI */

  Icon(uint8_t type_id, Noid noid, Payload *pp);
  /**< constructor: network replication */

  static WO * replicator(uint8_t type_id, Noid noid, Payload *pp);
  /**< Replicates a icon coming from the Network */

  bool isMoving();
  /**< Checks if object is moving */

  void changePosition(float lasting);
  /**< Does any position changes */

  void changePermanent(float lasting);
  /**< Does permanent position changes */

  void updateTime(time_t, time_t, float *);
  /**< Updates times */

  bool publish(const Pos &oldpos);
  /**< Publishes to network */

  bool whenIntersect(WO *pcur, WO *pold);
  /**< Handles collisions */

  void quit();
  /**< Quits */

private:
  void parser(char *l);
  /**< Parses */

  void defaults();
  /**< icon defaults */

  void geometry();
  /**< Builds solid geometry */

  char * getParam(char *ptok);
  /**< gets parameter */

  // GUI and network change callbacks
  static void create(User *pu, void *d, time_t s, time_t u);
  static void stick(Wall *pwall, void *_picon, time_t s, time_t u);
  static void pin(Icon *o, void *d, time_t s, time_t u);
  static void push(Icon *o, void *d, time_t s, time_t u);
  static void pull(Icon *o, void *d, time_t s, time_t u);
  static void carry(Icon *o, void *d, time_t s, time_t u);
  static void leave(Icon *o, void *d, time_t s, time_t u);
  static void drop(Icon *o, void *d, time_t s, time_t u);
  static void open(Icon *o, void *d, time_t s, time_t u);
  static void save(Icon *o, void *d, time_t s, time_t u);
  static void turn(Icon *o, void *d, time_t s, time_t u);
  static void destroy(Icon *o, void *d, time_t s, time_t u);

  static void get_url(Icon *o, Payload *pp);
  static void put_url(Icon *o, Payload *pp);
  static void get_tex(Icon *o, Payload *pp);
  static void put_tex(Icon *o, Payload *pp);
  static void get_gname(Icon *o, Payload *pp);
  static void put_gname(Icon *o, Payload *pp);
};

#endif
