//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2024 Philippe Dax
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
// world.hpp
//---------------------------------------------------------------------------
#ifndef WORLD_HPP
#define WORLD_HPP

#include "object.hpp"	// Object
#include "matvec.hpp"   // V3 M4

#include <list>


/**
 * World class
 */
class World {
 private:
  static class World * worldcurr;

  // world states
  enum world_state {
    TOLOAD,
    LOADING,
    LOADED,
    SIMULATION,
    STOPPED
  };

 public:
  static const uint8_t GRIDX;
  static const uint8_t GRIDY;
  static const uint8_t GRIDZ;
  static const float   DISTX;
  static const float   DISTY;
  static const float   DISTZ;
  static const uint8_t WORLD_LEN;
  static class OList *gridArray[4][4][2];

  char *url;			///< url of world description.
  char *name;			///< world name.
  char *chan;			///< channel string addr/port/ttl.
  uint8_t state;		///< current state.
  uint16_t num;			///< world number.
  uint32_t group;		///< current group addr.
  uint32_t ssrc;		///< current ssrc.
  bool linked;			///< linked world or not.
  bool persistent;		///< persistent world or not.
  float ground;			///< ground level
  V3 bbmin;			///< bb min of the world.
  V3 bbmax;			///< bb max of the world.
  V3 bbslice;			///< slice units of the grid.
  V3 bbcent;			///< bb center of the world.
  V3 bbsize;			///< bb size of the world.
  uint8_t dimgrid[3];		///< grid dimensions.
  World *prev;			///< prev world.
  World *next;			///< next world.

  class Universe *universe;	///< universe pointer.
  class Bgcolor *bgcolor;	///< background color.
  class User *user;		///< localuser.
  class OList ****grid;		///< matrix grid pointer.
  struct GuiItem *guip;		///< gui handle.
  class Clock *clock;		///< internal clock.
  class Vjc *vjc;		///< vjc.
  class Wind *wind;		///< wind.

  World();	///< Constructor.
  ~World() {};	///< Destructor.

  void compute(time_t sec, time_t usec);
  /**< Processes the world. */

  void quit();
  /**< Quits current world. */

 private:
  static void reader(void *urlvre, class Http *http);
  /**< World reader. */

  bool call(World *wprev);
  /**<
   * Switches channels.
   * deleteChannel -> enter() -> join()
   */

  void checkIcons();
  /**< Checks whether Icon objects are locally presents. */

  void checkPersist();
  /**< Checks whether objects are persistents. */

 public:
  enum {
    OLD_WORLD,
    NEW_WORLD
  };

  //
  // Accessors
  //
  void setName(const char* name);	///< Builds world name from url.
  void setUrl(const char* _url);	///< Sets the world url.
  void setGroup(uint32_t _group);	///< Sets the world group.
  bool setChan(const char *chan);	///< Sets the channel name.
  void joinChan(char *chan);		///< Joins the channel.
  void setGround(float level);		///< Sets the world level.

  //
  // Grid
  //
 private:
  void initGrid();
  /**< Inits the grid by default. */

  void localGrid();
  /**< Sets local grid for the current world. */

  //
  // World methods - static methods
  //
 public:
  static World* current();
  /**< Returns the current world. */

  static void init(const char *urlvre);
  /**< General World initialization. */

  static World* enter(const char* url, const char* chan, bool isnew);
  /**< New World initialization. */

  static World* goPrev();
  /**< Go to the previous world and return it. */

  static World* goNext();
  /**< Go to the next world and return it. */

  static World* swap(World *w);
  /**< Exchanges Worlds in the list. */

  static World* getWorld(const char *url);
  /**< Gets world by url. */

  static void initFunc();
  /**< Initializes table of general functions. */

  void clearObjects();
  /**< Clears all object lists. */

  void deleteObjects();
  /**< Deletes all objects dropped in the todeletelist. */

  void joinManager(const char *chan);
  /**< Sets the manager channel name and joins it. */
};

#endif
