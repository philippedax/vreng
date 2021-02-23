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
#ifndef WORLD_HPP
#define WORLD_HPP

#include <list>

using namespace std;

#define MAX_WORLDS	256	// uint8_t

#define WL 0	// STL

/**
 * World class
 */
class World {

 private:
#if WL
#include <list>
  static std::list<World*> worldList;
#else
  static class World * worldList;
#endif

  /* states */
  enum world_state {
    TOLOAD,
    LOADING,
    LOADED,
    SIMULATION,
    STOPPED
  };

 protected:
  static const uint8_t GRIDX;
  static const uint8_t GRIDY;
  static const uint8_t GRIDZ;
  static const float   DISTX;
  static const float   DISTY;
  static const float   DISTZ;

  char *url;			///< url of world description.
  char *name;			///< world name.
  char *chan;			///< channel string addr/port/ttl.
  uint8_t state;		///< current state.
  uint32_t group;		///< current group addr.
  uint32_t ssrc;		///< current ssrc.
  bool islinked;		///< linked world or not.
  bool persistent;		///< persistent world or not.
  uint16_t num;			///< world number.
  V3 bbmin;			///< bb min of the world.
  V3 bbmax;			///< bb max of the world.
  V3 bbslice;			///< slice units of the grid.
  float ground;			///< ground level
  uint8_t dimgrid[3];		///< grid dimensions.
  class Universe *universe;	///< universe pointer.
  class Bgcolor *bgcolor;	///< background color.
  World *prev;			///< prev world.
  World *next;			///< next world.
  class User *user;		///< localuser.

 public:
  static const uint8_t WORLD_LEN;

  static class OList *gridArray[4][4][2];
  class OList ****grid;	///< matrix grid pointer.

  int namecnt;			///< name counter.
  V3 bbcenter;			///< bb center of the world.
  V3 bbsize;			///< bb size of the world.

  struct GuiItem *guip;		///< gui handle.
  class Clock *clock;		///< internal clock.
  class Vjc *vjc;		///< vjc.
  class Wind *wind;		///< wind.

  World();
  /**< Constructor. */

  virtual ~World() {};
  /**< Destructor. */

  virtual void compute(time_t sec, time_t usec);
  /**< Processes the world. */

  virtual void quit();
  /**< Quits current world. */

 private:
  virtual void addToList();
  /**< Adds world into world list. */

  virtual bool call(World *wprev);
  /**<
   * Switches channels.
   * deleteChannel -> enter() -> join()
   */

  virtual void checkIcons();
  /**< Checks whether Icon objects are locally presents. */

  virtual void checkPersist();
  /**< Checks whether objects are persistents. */

  //
  // Accessors
  //

 public:

  enum {
    OLD,
    NEW
  };

  virtual const char* getName() const;		///< Gets current world name.
  virtual void setName(const char* name);	///< Builds world name from url.

  virtual uint8_t getState() const;		///< Gets the world state.
  virtual void setState(int _state);		///< Sets a world state.
  virtual bool isDead() const;			///< Checks if world is dead.

  virtual const char* getChan() const;		///< Current channel string.
  virtual bool setChan(const char *chanstr);	///< Sets the channel name.
  virtual void setChanAndJoin(char *chanstr);	///< Sets and joins the channel.

  virtual const char* getUrl() const;		///< Gets the world url.
  virtual void setUrl(const char* _url);	///< Sets the world url.

  virtual void setGround(float level);		///< Sets the world level.
  virtual float getGround() const;		///< Gets the world level.

  // Accessors
  virtual bool isPersistent() const;
  virtual void setPersistent(bool persistent);
  virtual bool isLinked() const;
  virtual void linked();
  virtual struct GuiItem * getGui() const;
  virtual bool isGui() const;
  virtual void resetGui();
  virtual uint16_t number() const;
  virtual User* localUser() const;
  virtual Bgcolor* backgroundColor() const;
  virtual uint32_t getGroupAdr() const;
  virtual uint32_t getSsrc() const;
  virtual void setGroupAdr(uint32_t _group);
  virtual void setSsrc(uint32_t _ssrc);

  void localGrid();
  /**< Sets locals for the current world. */

  void initGrid();
  /**< Inits the grid by default. */

  void initGrid(const uint8_t dim[3], const V3 &sl);
  /**< Inits the grid. */

  class OList **** allocGrid();
  /**< Allocs memory for the grid. */

  void clearGrid();
  /**< Clears the grid. */

  void freeGrid();
  /**< Frees the grid. */

  //
  // World methods
  // static methods
  //
  static World* current();		///< Return the current world.

  static World* goBack();		///< Go to the previous world and return it.
  static World* goForward();		///< Go to the next world and return it.

  static World* swap(World *w);		///< Exchanges Worlds in the list.

  static World* worldByUrl(const char *_url);	///< Gets world by url.
  static World* worldByGroup(uint32_t group);	///< Gets world by group addr.

  static World* enter(const char* _url, const char* _chanstr, bool _new_one);
  /**< New World initialization. */

  static void init(const char *urlvre);
  /**< General World initialization. */

  static void httpReader(void *urlvre, class Http *http);
  /**< World reader. */

  static void deleteObjects();
  /**< Deletes all objects dropped in the todeletelist. */

  static void initNames();
  /**< Initializes hashcode table of names. */

  static void initGeneralFuncList();
  /**< Initializes table of general functions. */

  static void setManagerChanAndJoin(const char *chanstr);
  /**< Sets the manager channel name and joins it. */

  static const char * getManagerChan();
  /**< Gets the channel name of the manager. */

  static void clearLists();
  /**< Clears all lists. */

};


#endif
