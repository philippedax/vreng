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
#ifndef WOBJECT_HPP
#define WOBJECT_HPP

#include "oclass.hpp"	// OClass
#include "olist.hpp"	// OList
#include "parse.hpp"	// Parse
#include "vrsql.hpp"	// VRSql


/* special behavior masks */
#define NO_BEHAVIOR		0	// null
#define NO_ELEMENTARY_MOVE	1	// bit 0
#define NO_BBABLE		2	// bit 1
#define UNSELECTABLE		4	// bit 2
#define UNVISIBLE		8	// bit 3
#define SPECIFIC_RENDER		16	// bit 4
#define PERSISTENT		32	// bit 5
#define DYNAMIC			64	// bit 6
#define TAKABLE			128	// bit 7
#define REMOVABLE		256	// bit 8
#define PARTICLE		512	// bit 9
#define LIQUID			1024	// bit 10
/* collide behaviors */
#define COLLIDE_EVER		0	// bits 29,30,31
#define COLLIDE_MASK		3<<29	// bits 29,30,31
#define COLLIDE_ONCE		1<<29	// bit 29
#define COLLIDE_NEVER		1<<30	// bit 30
#define COLLIDE_GHOST		1<<31	// bit 31

/**
 * external variables
 * objects lists
 */
extern std::list<WObject*> objectList;
extern std::list<WObject*> mobileList;
extern std::list<WObject*> stillList;
extern std::list<WObject*> invisList;
extern std::list<WObject*> fluidList;
extern std::list<WObject*> deleteList;
extern std::list<WObject*> lightList;
extern std::list<WObject*> renderList;


/**
 * WObjectId Class
 *
 * WObjectId identifies a distributed object.
 */
class WObjectId {
 public:
  uint32_t src_id;	///< IPaddr src
  uint16_t port_id;	///< port
  uint16_t obj_id;	///< object id -- network format
};

/**
 * Names struct
 *
 * Container for all names.
 */
struct Names {
  char type[HNAME_LEN];		///< name of an object class.
  char given[OBJNAME_LEN];	///< given name.
  char *instance;		///< name of an instancied object.
  char *implicit;		///< implicit name.
  char *category;		///< category name.
  char *infos;			///< infos of an instancied object.
  const char *world;		///< name of world where is this object.
  char url[URL_LEN];		///< url.
  char owner[USER_LEN];		///< owner.
};

/**
 * Pos struct
 *
 * Spatial position and bounding-box.
 */
struct Pos {
 public:
  float x;		///< x absolute position.
  float y;		///< y absolute position.
  float z;		///< z absolute position.
  float az;		///< angle plan xy axis z.
  float ay;		///< angle plan xz axis y.
  float ax;		///< angle plan yz axis x.
  V3 bbcenter;		///< Bounding Box's center.
  V3 bbsize;		///< Bounding Box's dimension.
  bool moved;		///< has moved or not.
  uint8_t state;	///< button state.
};

/**
 * Move struct
 *
 * Motion parameters of the object.
 */
struct Move {
  V3 lspeed;		///< linear speed.
  V3 aspeed;		///< angular speed.
  time_t sec;		///< timestamp.
  time_t usec;		///< timestamp.
  time_t perm_sec;	///< time in sec of last permanent movement.
  time_t perm_usec;	///< time in usec of last permanent movement.
  float ttl;		///< time to live.
  bool without_col;	///< flag without collision.
  struct Move *next;	///< next motion.
};

/**
 * WObject Class
 *
 * Common class for all the objects.
 */
class WObject {
public:
  typedef std::list<class Solid*> solidList;
  solidList _solids;		///< list of solids.

protected:
  bool objectbar;		///< true if object bar is active.

public:
  class NetObject *noh;		///< reserved field for network.
  struct GuiItem *guip;		///< reserved field for GUI.
  uint8_t type;			///< object type.
  uint8_t mode;			///< object mode.
  uint16_t num;			///< object sequence number.
  uint32_t behavior;		///< behavior flags.
  Names names;			///< names.
  Pos pos;			///< position in the space.
  Move move;			///< movement specific.
  class WObjectId noid;		///< WObject Id.
  bool is_in_a_list;		///< true if it is already in an OList.
  bool removed;			///< flag removed or not.
  int16_t state;		///< current state.
  int8_t prior;			///< render priority.
  char *geometry;		///< geometry string.
  class Solid *solid;		///< solid pointer
  char chan[CHAN_LEN];		///< channel.
#if VRSQL 			///< HAVE_SQLITE | HAVE_MYSQL
  class VRSql *psql;		///< VRSql pointer.
#endif
  class Flare *flare;		///< flare instance.

  /* object's list modes */
  enum object_mode {
    STILL,
    MOBILE,
    EPHEMERAL,
    INVISIBLE,
    FLUID,
    MOBILEINVISIBLE,
    MODE_MAX
  };

  /* object's states */
  enum object_state {
    DELETED = -1,
    NONE = 0,
    INACTIVE = 0,
    ACTIVE,
    PLAYING,
    TAKEN,
    DROPPED,
    MOVED,
    CARRYING,
    IN_CART,
    ENDSTATE
  };

  /* render priorities */
  enum object_prior {
    PRIOR_LOW,
    PRIOR_MEDIUM,
    PRIOR_HIGH
  };

  WObject();
  /**< Constructor. */

  virtual ~WObject();
  /**< Destructor. */

  virtual const OClass* getOClass()	{return NULL;};
  /**< Abstrct class. */

  virtual uint8_t typeId()		{return getOClass()->type_id;};
  virtual const char* typeName()	{return getOClass()->type_name;};
  WCreator* getCreator()		{return getOClass()->creator;};
  WReplicator* getReplicator()  	{return getOClass()->replicator;};

  //
  // Methods of Instances of general object handlers
  //
  virtual bool isMoving() { return testMoving(); }
  /**< Checks weither object is moving. */

  virtual void changePosition(float lasting) {}
  /**< Changes the position after a triggered movement. */

  virtual void changePermanent(float lasting) {}
  /**< Changes the position during a permanent movement. */

  virtual void updateTime(time_t sec, time_t usec, float *lasting) {}
  /**< Updates remaining times of the movement. */

  virtual bool updateToNetwork(const Pos &oldpos) { return false; }
  /**< Publishes changes to the network. */

  virtual bool updatePosToNetwork(const Pos &oldpos, int propxy, int propz, int propaz, int propax, int propay);
  /**< Publishes position changes to the network. */

  virtual bool whenIntersect(WObject *pcur, WObject *pold) { return false; }
  /**< Handles an ingoing collision with another object. */

  virtual bool whenIntersectOut(WObject *pcur, WObject *pold) {return false;}
  /**< Handles an outgoing collision with another object. */

  virtual void whenWallsIntersect(WObject *pold, V3 *norm) {}
  /**< Handles collisions with walls. */

  virtual void render() {}
  /**< Makes special rendering. */

  virtual void lighting() {}
  /**< Makes special lighting. */

  virtual void quit() {}
  /**< Makes all thing when leaving the object. */

  //
  // Accessors to Solid
  //
  const solidList& solids() const;
  /**< solidlist address. */

  uint32_t countOfSolids() const;
  //*< Count of solids. */

  virtual class Solid* getSolid() const;
  /**< Returns first solid. */

  virtual void addSolid(class Solid* solid);
  /**< Adds to solidList. */

  virtual void deleteSolids();
  /**< Deletes solids from solidList. */

  //
  // Intersections
  //
  virtual int interAABB(WObject *o1, WObject *o2);
  static int interAABB(V3 center1, V3 size1, V3 center2, V3 size2);
  /**< Computes intersection between 2 AABB (Axis Aligned Bounding Box). */

  static int interAABBVertical(V3 center1, V3 size1, V3 center2, V3 size2);
  static int interAABBHorizontal(V3 center1, V3 size1, V3 center2, V3 size2);

  //
  // Set, Get, Have, Is
  //
  virtual bool haveAction();
  /**< Checks weither have actions. */

  virtual uint8_t numberAction();
  /**< Number of actions. */

  virtual bool runAction(const char *action);
  /**< Runs action everwhere if available. */

  virtual void setType(int type);
  /**< Sets object type. */

  virtual int getType() {return type;}
  /**< Gets object type. */

  virtual int getNumber() {return num;}
  /**< Gets object number. */

  virtual uint16_t getNum();
  /**< Gets a new object number. */

  virtual void getPosition(M4& mpos);
  /**< Gives solid's position. */

  virtual void getRelBB(V3 &center, V3 &size);
  virtual void getDimBB(V3 &dim);
  virtual void getCentBB(V3 &center);
  virtual void getAbsBB(V3 &center, V3 &size);
  /**< Gets relative or absolute center and size of solid. */

  virtual uint8_t getFrames();
  /**< Gets number of frames of this solid. */

  virtual uint8_t getFrame();
  /**< Gets index of current frame of this solid. */

  virtual void setFrame(uint8_t _frame);
  /**< Sets current frame. */

  virtual bool isValid() const;
  /**< Checks if valid object type. */

  virtual bool isPermanent() const;
  /**< Checks if permanent object. */

  virtual const char * named() const;
  /**< Gets names.given. */

  virtual const char * getInstance() const;
  /**< Gets names.instance. */

  virtual const char * urlName() const;
  /**< Gets names.url. */

  virtual const char * ownerName() const;
  /**< Gets names.owner. */

  virtual const char * worldName() const;
  /**< Gets names.owner. */

  virtual bool givenName() const;
  /**< Checks whether the object is explicitly named. */

  virtual void toDelete();
  /**< Delete this object latter. */

  virtual bool removeFromScene();
  /**< Removes an object. */

  virtual bool isOwner() const;
  /**< Returns true if owner. */

  virtual void setOwner(const char *_owner);
  /**< Initialize owner. */

  virtual void setOwner();
  /**< Initialize local user as owner. */

  static void show(const char *name);
  /**< Snaps position (debug). */

  virtual void click(GLint x, GLint y);
  virtual void click(V3 norm) {}
  /**< Intercepts a click. */

  //
  // Parse
  //
  virtual char * tokenize(char *l);
  /**< Tokenizes the line <object ... >. */

  virtual Parse * parse();
  /**< Gets current parse. */


  //
  // Noid (Network Object Identifier)
  //
  void setWObjectId();
  /**< Sets the WObjectid.
   * Assigns a unique identifier to each Vreng object
   * whether if be a networked object or not.
   */

  void copyNoid(class Noid _noid);
  /**< Copies the NetObjectid in WObjectId. */

  void setSrcId(uint32_t src_id);
  /**< Sets the SrcId. */

  void setPortId(uint16_t port_id);
  /**< Sets the PortId. */

  void setObjId(uint16_t obj_id);
  /**< Sets the ObjId. */

  uint32_t getSrcId() const;
  /**< Gets the SrcId. */

  uint16_t getPortId() const;
  /**< Gets the PortId. */

  uint16_t getObjId() const;
  /**< Gets the ObjId. */

  //
  // 3D
  //
#if 0 //notused
  virtual void setBB();
  /**< Sets solid's Bounding Box. */
#endif //notused

  virtual void update3D(Pos &pos);
  /**< Updates object in the 3D. */

  virtual void updateAll3D(Pos &pos);
  /**< Updates object in the 3D. */

  virtual void updateCamera(Pos &pos);
  /**< Updates camera in the 3D. */

  virtual void getSurfVecs(Pos &pos, V3 *v, V3 *w, V3 *norm);
  /**< Returns two vectors that describe the object's surface
   * and a normal vector to that surface
   */

  virtual void updateBB();
  /**< Updates object's Bounding Box. */

  virtual void getMaterials(GLfloat *dif, GLfloat *amb, GLfloat *spe, GLfloat *emi, GLint *shi, GLfloat *alpha);

  //
  // Grid
  //
  virtual void insertIntoGrid();
  /**< Adds an object into the grid. */

  virtual void deleteFromGrid();
  /**< Deletes an object from the grid. */

  virtual void updateGrid(const float *bbminnew, const float *bbmaxnew, const float *bbminold, const float *bbmaxold);
  virtual void updateGrid(const Pos &oldpos);
  virtual void updateGrid(const WObject *pold);
  /**< Updates an object into the grid. */

  //
  // List
  //
  virtual void    addToList(std::list<WObject*> &olist);
  virtual OList * addToList(OList * olist);
  /**< Adds an object pointer into a olist. */

  virtual void addToList();
  /**< Adds an object pointer into lists. */

  virtual void addToObject();
  /**< Adds an object pointer into objectList. */

  virtual void addToStill();
  /**< Adds an object pointer into stillList. */

  virtual void addToMobile();
  /**< Adds an object pointer into mobileList. */

  virtual void addToInvisible();
  /**< Adds an object pointer into invisList. */

  virtual void addToFluid();
  /**< Adds an object pointer into fluidList. */

  virtual void addToRender();
  /**< Adds an object pointer into renderList. */

  virtual void delFromMobile();
  /**< Deletes an object pointer from mobileList. */

  virtual void clearList();
  /**< Clears an olist. */

  virtual void clearList(std::list<WObject*> &olist);
  /**< Clears an olist. */

  virtual void    addToListOnce(std::list<WObject*> &olist);
  virtual OList * addToListOnce(OList * olist);
  /**< Adds an object pointer into a list only once time. */

  virtual void    delFromList(std::list<WObject*> &olist);
  virtual OList * delFromList(OList * olist);
  /**< Deletes an object pointer from a olist. */

  virtual void delFromList();
  /**< Deletes an object pointer from a olist. */

#if 0 //STL
  virtual list<WObject*>::iterator getVicinityList(const WObject *pold);
#else
  virtual OList * getVicinityList(const WObject *pold);
#endif
  /**< Returns list of pointers on objects touching cell where is the object. */

#if 0 //STL
  virtual list<WObject*>::iterator addListToList(list<WObject*> &list1, list<WObject> &list2);
#else
  virtual OList * addListToList(OList * list1, OList * list2);
#endif
  /**< Concatenation (test of "ispointed") of list pointers on an object. */

  virtual bool isStill();
  /**< Checks an object exists in the stilllist. */

  virtual bool isMobile();
  virtual bool isFluid();
  virtual bool isEphemeral();
  /**< Checks an object exists in the mobilelist. */

  virtual WObject * byWObject(WObject *po);
  /**< Gets a WObject from the lists. */

  static WObject * byNum(uint16_t num); // to become virtual !
  /**< Gets an object by its num. */

  //
  // Movements
  //
  virtual void checkVicinity(WObject *pold);
  /**< Checks weither vicinity. */

  virtual void imposedMovement(time_t sec, time_t usec);
  /**< Handles an object movement. */

  virtual void elemImposedMovement(float lasting);
  /**< Handles an elementary object movement. */

  virtual void enableImposedMovement();
  /**< Enables movement on an object. */

  virtual void enablePermanentMovement();
  /**< Enables permanent movement on an object. */

  virtual void disablePermanentMovement();
  /**< Disables movement on an object. */

  virtual void enablePermanentMovement(float speed);
  /**< Enables permanent movement on an object. */

  virtual void initImposedMovement(float lasting);
  /**< Initializes movement on an object. */

  virtual float diffTime(time_t sec, time_t usec);
  /**< Updates times on an object. */

  virtual void stopImposedMovement();
  /**< Stops a movement on an object. */

  virtual void permanentMovement(time_t sec, time_t usec);
  /**< Handles a permanent object movement. */

  virtual void elemPermanentMovement(float lasting);
  /**< Handles an elementary permanent object movement. */

  virtual void setLasting(float maxlast);
  /**< Sets the max lasting time of an object. */

  virtual float getLasting() const;
  /**< Gets the lasting time of an object. */

  virtual bool testMoving();
  /**< Tests if object is moving. */

  virtual bool updateLasting(time_t sec, time_t usec, float *lasting);
  /**< Updates remaining times of the movement. */

  virtual void moveUserToObject(float val, float _lttl, float _attl);
  /**< Moves the user to the object. */

  static void moveObject(WObject *po, void *d, time_t s, time_t u);
  /**< User moves the object. */

  //
  // Names
  //
  virtual void setObjectName(const char *str);
  /**< Sets an object name. */

  virtual WObject *getObjectByName(const char *str);
  /**< Gets an object by its name. */

  virtual void updateNames();
  /**< Updates names. */

  static void getObjectNameById(uint8_t type_id, char *name);
  /**< Gets a name by its id. */

  //
  // Initializations
  //
  virtual void initObject(uint8_t mode);
  /**< Initializes object. */

  virtual void initMobileObject(float last);
  virtual void initEphemeralObject(float last);
  /**< Initializes mobile object. */

  virtual void initStillObject();
  /**< Initializes still object. */

  virtual void initFluidObject(float last);
  /**< Initializes fluid object. */

  void createPermanentNetObject(uint8_t props, uint16_t oid);
  /**< Creates local permanent NetObject. */

  void createVolatileNetObject(uint8_t props);
  /**< Creates local volatile NetObject. */

  void replicateVolatileNetObject(uint8_t props, class Noid _noid);
  /**< Replicate distant volatile NetObject. */

  virtual void enableBehavior(uint32_t flag);
  /**< Enables behavior. */

  virtual void disableBehavior(uint32_t flag);
  /**< Disables behavior. */

  virtual bool isBehavior(uint32_t flag) const;
  /**< Checks weither this behavior is on. */

  virtual bool bbBehavior() const;
  /**< Checks weither BBox behavior is on. */

  virtual bool isSeen();
  /**< Checks weither the object is in the sight view of the user. */

  virtual void setVisible(bool flag);
  /**< Sets visible or not the 3D object. */

  virtual bool isVisible() const;
  /**< Checks weither the object is visible. */

  virtual bool isOpaque() const;
  /**< Checks weither the object is opaque. */

  virtual void setFlashy(float *color);
  /**< Sets flashy the 3D object. */

  virtual void setFlashy();
  /**< Sets flashy the 3D object. */

  virtual void resetFlashy();
  /**< Resets flashy the 3D object. */

  virtual void setRay(GLint wx, GLint wy);
  virtual void resetRay();
  /**< Resets Ray on the 3D object. */

  virtual void setReflexive(bool flag);

  virtual bool isSelectable() const;
  /**< Checks weither 3D selectable behavior is on. */

  virtual uint32_t collideBehavior() const;
  /**< Returns collide behavior. */

  virtual void setRenderPrior(uint8_t _prior);
  /**< Sets render priority. */

  virtual uint8_t getRenderPrior() const;
  /**< Gets render priority. */

  //
  // Collisions
  //
  void generalIntersect(WObject *pold, OList *vicinityList);
  /**< General intersection of objects. */

  bool ingoingNeighbor(WObject *pold, WObject *neighbor);
  /** Checks ingoing intersection with a neighbor. */

  bool outgoingNeighbor(WObject *pold, WObject *neighbor);
  /** Checks outgoing intersection with a neighbor. */

  void ingoingWalls(WObject *pold);
  /** Checks ingoing intersection with walls. */

  virtual void copyPosAndBB(Pos &newpos);
  /**< Copy object position and Bounding Box. */

  virtual void copyPositionAndBB(WObject *o);
  /**< Copy object position and Bounding Box. */

  virtual bool projectPosition(WObject *pcur, WObject *pold);
  /**< Projects object position. */

  virtual int projectPositionOnObstacle(Pos &pcur, Pos &pold, Pos &obstacle);
  /**< Projects object position on an obstacle. */

  virtual void initPosition();
  /**< Init 3D and grid position. */

  virtual void updatePosition();
  /**< Updating 3D. */

  virtual void updatePositionAndGrid(Pos &oldpos);
  /**< Updating 3D and grid position. */

  virtual void updatePositionAndGrid(WObject *pold);
  /**< Updating 3D and grid position. */

  virtual void computeNormal(WObject *mobil, V3 *normal);
  /**< Computes the normal of an object. */

  virtual void computeNormal(Pos &mobil, Pos &stil, V3 *normal);
  /**< Computes the normal of still object. */

  virtual void bounceTrajectory(WObject *pold, V3 *norm);
  /**< Intersects with wall. */

  //
  // Properties
  //
  void getProperty(uint8_t prop_id, class Payload *pp);
  /**<
   * Gets the local copy property from the payload (pp->data).
   * Il faut renvoyer la position de lecture de la suite,
   * (utiliser getPayload et sa valeur de retour).
   * Typically called after a Delta reception.
   */

  void putProperty(uint8_t prop_id, class Payload *pp);
  /**<
   * Puts the local copy property in the payload.
   * Il faut ecrire pp->data et renvoyer la longueur ecrite
   * Typically called before a Delta emission.
   */

  virtual void deleteReplica();
  /**< Deletes a replicated object. */

  static void resetObjectsNumber();
  /**< Resets object seq. number. */

  static uint32_t getObjectsNumber();
  /**< Gets current object sequence number. */


  //
  // GUI
  //
  virtual struct GuiItem * getGui() const;
  /**< Gets the GUI handle. */

  virtual bool isGui() const;
  /**< Checks the GUI handle. */

  virtual void resetGui();
  /**< Resets the GUI handle. */

  virtual void clearObjectBar();
  /**< Clears the ObjectBar in the GUI. */

  virtual void specialAction(int action, void *data, time_t sec, time_t usec);
  /**< Calls methods dedicated to each object.
   * Called by GUI. */

  void getObjectHumanName(char **classname, char **instancename, char **actionnames);
  /**< Gives object's class_name & action names.
   * Called by GUI. */

  //
  // MySql
  //
  virtual void getPersistency();
  /**< Checks if position is managed by MySql and get it. */

  virtual void getPersistency(int16_t state);
  /**< Gets state from MySql. */

  virtual void updatePersistency();
  /**< Checks if position is managed by MySql and update it. */

  virtual void updatePersistency(int16_t state);
  /**< Updates state for MySql. */

  virtual void savePersistency();
  /**< Flushes position for MySql. */

  virtual void quitPersistency();
  /**< Quits MySql. */

protected:
  // GUI and network change callbacks
  static void get_xy(WObject *po, class Payload *pp);
  static void get_z(WObject *po, class Payload *pp);
  static void get_az(WObject *po, class Payload *pp);
  static void get_ay(WObject *po, class Payload *pp);
  static void get_ax(WObject *po, class Payload *pp);
  static void get_hname(WObject *po, class Payload *pp);

  static void put_xy(WObject *po, class Payload *pp);
  static void put_z(WObject *po, class Payload *pp);
  static void put_az(WObject *po, class Payload *pp);
  static void put_ay(WObject *po, class Payload *pp);
  static void put_ax(WObject *po, class Payload *pp);
  static void put_hname(WObject *po, class Payload *pp);
};


#include "initobj.hpp"


#endif
