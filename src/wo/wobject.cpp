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
#include "vreng.hpp"
#include "wobject.hpp"
#include "world.hpp"	// World::current
#include "user.hpp"	// localuser
#include "netobj.hpp"	// NetObject
#include "vrsql.hpp"	// VRSql
#include "solid.hpp"	// Solid
#include "olist.hpp"	// OList
#include "gui.hpp"	// clearInfoBar, removeUser
#include "payload.hpp"	// Payload
#include "carrier.hpp"	// carrier->take
#include "render.hpp"	// getVisiblePosition
#include "stat.hpp"	// new_wobject
#include "timer.hpp"	// ::g.timer

#include <list>
#include <vector>
using namespace std;

// global
vector<WObject*> objectList;
vector<WObject*> stillList;
list<WObject*> mobileList;
vector<WObject*> fluidList;
vector<WObject*> clothList;
vector<WObject*> invisList;
vector<WObject*> deleteList;
vector<WObject*> lightList;

// local
static uint32_t objectNum = 0;
static struct hash_elt hashtable[NAME_HASH_SIZE];


/* WObject constructor */
WObject::WObject()
{
  new_wobject++;

  netop = NULL;
  guip = NULL;
  inlist = false;
  removed = false;
  deleted = false;
  visible = true;

  state = NONE;
  type = 0;
  mode = 0;
  behavior = 0;

  memset(names.type, 0, sizeof(names.type));
  memset(names.given, 0, sizeof(names.given));
  memset(names.url, 0, sizeof(names.url));
  memset(names.owner, 0, sizeof(names.owner));

  names.implicit = NULL;
  names.instance = NULL;
  names.world = NULL;
  names.category = NULL;
  names.infos = NULL;
  geomsolid = NULL;

  pos.x = 0;
  pos.y = 0;
  pos.z = 0;
  pos.ax = 0;
  pos.ay = 0;
  pos.az = 0;
  pos.st = 0;
  pos.alter = false;
  pos.bbc = setV3(0, 0, 0);
  pos.bbs = setV3(0, 0, 0);

  move.perm_sec = 0;
  move.perm_usec = 0;
  move.ttl = 0.;
  move.manip = false;

  noid.src_id = 0;
  noid.port_id = 0;
  noid.obj_id = 0;

  solid = NULL;
  //dax solidList = ::g.render.getSolidList();
  ::g.render.relsolidList.clear();

  flare = NULL;
  carrier = NULL;
  vrsql = NULL;
}

/* WObject destructor */
WObject::~WObject()
{
  if (! isValid()) return;
  if (! isBehavior(COLLIDE_NEVER)) {
    delFromGrid();
  }

  delSolids();	// delete all solids

  // delete NetObject
  if (netop && (mode == MOBILE)) {
    if (! isPermanent()) {
      netop->declareDeletion();
    }
    delete netop;
    netop = NULL;
  }
  if (names.implicit && isalpha(names.implicit[0])) delete[] names.implicit;
  if (names.category && isalpha(names.category[0])) delete[] names.category;
  if (names.infos && isalpha(names.infos[0])) delete[] names.infos;
  if (geomsolid && isalpha(*geomsolid)) delete[] geomsolid;
  del_wobject++;
}

/* Initializes Object. */
void WObject::initObject(uint8_t _mode)
{
  mode = _mode;
  type = typeId();
  num = objectNum++;

  setWObjectId();
  updateNames();
  initPosition();
  objectList.push_back(this);	// add to objectList

  //echo("num=%d mode=%d type=%d", num, mode, type);
  switch (mode) {

    case STILL:
      stillList.push_back(this);	// add to stillList
      break;

    case MOBILE:
      if (isBehavior(PERSISTENT)) {
        if (checkPersist()) {
          getPersist();	// gets persistency object
        }
        else {
          setPersist();	// sets persistency object
        }
      }
      mobileList.push_back(this);	// add to mobileList
      if (isBehavior(PERMANENT_MOVEMENT)) {
        enablePermanentMovement();
      }
      break;

    case FLUID:
      enableBehavior(NO_ELEMENTARY_MOVE);
      fluidList.push_back(this);	// add to fluidList
      break;

    case CLOTH:
      enableBehavior(NO_ELEMENTARY_MOVE);
      clothList.push_back(this);	// add to clothList
      break;

    case INVISIBLE:
      enableBehavior(NO_BBABLE);
      enableBehavior(UNSELECTABLE);
      enableBehavior(NO_ELEMENTARY_MOVE);
      visible = false;
      invisList.push_back(this);	// add to invisList
      break;

    case EPHEMERAL:
      enableBehavior(NO_BBABLE);
      enableBehavior(UNSELECTABLE);
      enableBehavior(NO_ELEMENTARY_MOVE);
      mobileList.push_back(this);	// add to mobileList
      break;

    case MOBILEINVISIBLE:
      enableBehavior(NO_BBABLE);
      enableBehavior(UNSELECTABLE);
      enableBehavior(NO_ELEMENTARY_MOVE);
      visible = false;
      mobileList.push_back(this);	// add to mobileList
      invisList.push_back(this);	// add to invisList
      break;
  }
  if (isBehavior(NO_BBABLE)) {
    enableBehavior(COLLIDE_NEVER);
  }

  update3D(pos);
  if (bbBehavior()) {
    updateBB();
    insertIntoGrid();
  }
}

/* Initializes still object */
void WObject::initStillObject()
{
  initObject(STILL);
}

/* Initializes mobile object */
void WObject::initMobileObject(float last)
{
  initObject(MOBILE);
  if (last) {
    setLasting(last);
  }
}

/* Initializes fluid object */
void WObject::initFluidObject(float last)
{
  initObject(FLUID);
  if (last) {
    setLasting(last);
  }
}

/* Initializes cloth object */
void WObject::initClothObject(float last)
{
  initObject(CLOTH);
  if (last) {
    setLasting(last);
  }
}

/* Initializes ephemeral object */
void WObject::initEphemeralObject(float last)
{
  initObject(MOBILE);
  if (last) {
    setLasting(last);
  }
}

void WObject::enableBehavior(uint32_t flag)
{
  switch (flag) {
    case UNVISIBLE:
      setVisible(false);
      break;
    case COLLIDE_ONCE:
      disableBehavior(COLLIDE_NEVER);
      disableBehavior(COLLIDE_GHOST);
      break;
    case COLLIDE_NEVER:
      disableBehavior(COLLIDE_ONCE);
      disableBehavior(COLLIDE_GHOST);
      break;
    case COLLIDE_GHOST:
      disableBehavior(COLLIDE_ONCE);
      disableBehavior(COLLIDE_NEVER);
      break;
  }
  behavior |= flag;
}

void WObject::disableBehavior(uint32_t flag)
{
  behavior &= ~flag;

  switch (flag) {
    case UNVISIBLE: setVisible(true); break;
  }
}

bool WObject::isBehavior(uint32_t flag) const
{
  return (behavior & flag);
}

bool WObject::bbBehavior() const
{
  return (! isBehavior(NO_BBABLE));
}

bool WObject::isSelectable() const
{
  return (! isBehavior(UNSELECTABLE));
}

bool WObject::isValid() const
{
  return OClass::isValidType(type);
}

void WObject::setType(uint8_t _type)
{
  type = _type;
}

bool WObject::isRemoved() const
{
  return removed;
}

// static
uint32_t WObject::getObjectsNumber()
{
  return objectNum;
}

void WObject::resetObjectsNumber()
{
  objectNum = 0;
}

bool WObject::isPermanent() const
{
  return (netop) ? netop->isPermanent() : false;
}

bool WObject::isSeen()
{
  V3 vseen = ::g.render.getVisiblePosition(this);
  return vseen.v[2];  // seen = v[2]
}

bool WObject::isOwner() const
{
  if (! strcmp(names.owner, localuser->names.instance)) {
    return true;
  }
  return false;
}

void WObject::setOwner(const char *_owner)
{
  strcpy(names.owner, _owner);
}

void WObject::setOwner()
{
  if (localuser)
    setOwner(localuser->names.instance);
  else
    setOwner("me");
}

// Returns name
void WObject::getObjectNameById(uint8_t type, char *name)
{
  const OClass *oclass = OClass::getOClass(type);
  if (oclass) {
    strcpy(name, oclass->type_name);
  }
  else
    error("getObjectNameById: no name found for type=%d", type);
}

uint16_t WObject::getNum()
{
  num = ++objectNum;
  return num;
}

const char * WObject::named() const
{
  if (names.given)
    return names.given;
  else
    return NULL;
}

const char * WObject::getInstance() const
{
  if (names.instance)
    return names.instance;
  else
    return names.given;
}

const char * WObject::urlName() const
{
  return names.url;
}

const char * WObject::ownerName() const
{
  return names.owner;
}

const char * WObject::worldName() const
{
  return names.world;
}

bool WObject::givenName() const
{
  return *names.given;
}
 

//
// Solids
//

/* returns the first solid of the object - accessor */
Solid* WObject::getSolid() const
{
  return solid;
}

void WObject::setVisible(bool flag)
{
  visible = flag;
  if (solid) solid->setVisible(flag);
}

bool WObject::isVisible() const
{
  if (solid)
    return solid->isVisible();
  else
    return false;
}

void WObject::setRendered(bool flag)
{
  if (solid) solid->setRendered(flag);
}

bool WObject::isRendered() const
{
  if (solid)
    return solid->isRendered();
  else
    return false;
}

bool WObject::isOpaque() const
{
  if (solid)
    return solid->isOpaque();
  else
    return false;
}

void WObject::setRay(GLint wx, GLint wy)
{
  if (solid) solid->setRay(wx, wy);
}

void WObject::resetRay()
{
  if (! isValid()) return;
  if (solid) solid->resetRay();
}

void WObject::setReflexive(bool flag)
{
  if (solid) solid->setReflexive(flag);
}

/* Sets a WObject pointer to this object in the Solid (friend) */
/* adds solid to the list of solids for this object */
void WObject::addSolid(Solid* psolid)
{
  _solidList.push_back(psolid);	// add solid to solidList
  psolid->wobject = this;	// Solid is friend of WObject
  solid = psolid;		// keep solid pointer in WObject
}

/* Deletes all solids of this object */
void WObject::delSolids()
{
  list<Solid*> solList = ::g.render.getSolidList();
  for (list<Solid*>::iterator it = solList.begin(); it != solList.end(); ++it) {
    if ((*it)->wobject == this) {
      delete *it;
    }
  }
}

void WObject::getPosition(M4& mpos)
{
  if (solid) solid->getPosition(mpos);
}

void WObject::getMaterials(GLfloat *dif, GLfloat *amb, GLfloat *spe, GLfloat *emi, GLint *shi, GLfloat *alpha)
{
  if (solid) solid->getMaterials(dif, amb, spe, emi, shi, alpha);
}

/* Gets size of solid */
void WObject::getDim(V3 &dim)
{
  if (solid) solid->getDimBB(dim);
}

/* Gets relative center and size of solid */
void WObject::getRelBB(V3 &center, V3 &size)
{
  if (solid) solid->getRelBB(center, size);
}

#if 0 //notused
/* Gets absolute center and size of solid */
void WObject::getAbsBB(V3 &center, V3 &size)
{
  if (solid) solid->getAbsBB(center, size);
}

/* Gets relative center of solid */
void WObject::getCent(V3 &center)
{
  if (solid) solid->getCentBB(center);
}
#endif //notused

/* Gets number of frames of this solid */
uint8_t WObject::getFrames()
{
  if (solid) return solid->getFrames();
  else return 0;
}

/* Gets index of current frame of this solid */
uint8_t WObject::getFrame()
{
  if (solid) return solid->getFrame();
  else return 0;
}

void WObject::setFrame(uint8_t _frame)
{
  if (solid) solid->setFrame(_frame);      // set frame
}

void WObject::setFlashy(float *color)
{
  if (solid) solid->setFlashyEdges(color);
}

void WObject::setFlashy()
{
  if (solid) solid->setFlashyEdges(true);
}

void WObject::resetFlashy()
{
  if (! isValid()) return;
  if (solid) solid->resetFlashyEdges();
}

//
// Network
//

uint32_t WObject::getSrc() const
{
  return noid.src_id;
}

uint16_t WObject::getPort() const
{
  return noid.port_id;
}

uint16_t WObject::getObj() const
{
  return noid.obj_id;
}

/* Assigns a unique identifier to each Vreng object */
void WObject::setWObjectId()
{
  noid.src_id = NetObject::getSsrc();		// Application's identifier
  noid.port_id = NetObject::getPort();		// Comm port identifier
  NetObject::setObj(NetObject::getObj()+1);	// myObjId++
  noid.obj_id = htons(NetObject::getObj());	// Application wide unique number
}

/* Creates local permanent NetObject */
NetObject * WObject::createPermanentNetObject(uint8_t props, uint16_t oid)
{
  netop = new NetObject(this, props, oid);
  return netop;
}

/* Creates local volatile NetObject */
NetObject * WObject::createVolatileNetObject(uint8_t props)
{
  netop = new NetObject(this, props);
  return netop;
}

/* Replicates distant volatile NetObject */
NetObject * WObject::replicateNetObject(uint8_t props, Noid _noid)
{
  noid.src_id = _noid.src_id;
  noid.port_id = _noid.port_id;
  noid.obj_id = _noid.obj_id;
  netop = new NetObject(this, props, _noid);
  return netop;
}

//
// Gui
//
struct GuiItem* WObject::getGui() const
{
  return guip;
}

bool WObject::isGui() const
{
  return (guip) ? true : false;
}

void WObject::resetGui()
{
  guip = NULL;
}

bool WObject::removeFromScene()
{
  if (isOwner()) {
    if (vrsql) vrsql->deleteRow(this);
    toDelete();
    clearObjectBar();
    return true;
  }
  else {
    echo("Permission denied, owner is %s", names.owner);
    removed = false;
    return false;
  }
}

//
// names
//
void WObject::initNames()  
{ 
  for (int i=0; i < NAME_HASH_SIZE; i++) {
    memset(hashtable[i].name, 0, OBJNAME_LEN);
  }
}

static uint32_t hash_name(const char *s)
{ 
  uint32_t h;

  for (h=0; *s ; s++) {
    h = ((h << 8) + *s) % NAME_HASH_SIZE;
  }
  return h;
} 

void WObject::setObjectName(const char *name)
{ 
  if (! name) return;

  char fullname[OBJNAME_LEN];
  sprintf(fullname, "%s@%s", name, World::current()->getName());
  uint32_t hval = hash_name(fullname);
  while (hval) {
    if ((*(hashtable[hval].name) == '\0') ||
        (! strcmp(hashtable[hval].name, NAME_DELETED))) {
      strcpy(hashtable[hval].name, fullname);
      hashtable[hval].po = this;
      return;
    }
    hval = (hval + 1) % NAME_HASH_SIZE;
  }
}

WObject * WObject::getObjectByName(const char *name)
{
  if (! name) return (WObject *) NULL;

  char fullname[OBJNAME_LEN];
  sprintf(fullname, "%s@%s", name, World::current()->getName());
  uint32_t hval = hash_name(fullname);
  trace(DBG_WO, "getObjectByName: hval=%d name=%s", hval, fullname);
  while (hval) {
    if (*(hashtable[hval].name) == '\0') {
      return (WObject *) NULL;          // not found
    }
    if (! strcmp(hashtable[hval].name, fullname)) {
      return hashtable[hval].po;        // found
    }
    hval = (hval + 1) % NAME_HASH_SIZE;
  }
  return (WObject *) NULL;              // not found
}

/* Sets Object names */
void WObject::forceNames(const char *name)
{
  strcpy(names.type, name);
  names.implicit = new char[OBJNAME_LEN];
  sprintf(names.implicit, "%s%d", name, num);
  if (isupper(*(names.implicit))) {
    *names.implicit = tolower(*(names.implicit)); // names.implicit in lowercase
  }
  names.instance = names.implicit;
}

/* Updates Object names */
void WObject::updateNames()
{
  if (! isValid()) return;

  getObjectNameById(type, names.type);

  if (! givenName()) {	// no given name
    names.implicit = new char[OBJNAME_LEN];
    sprintf(names.implicit, "%s%d", names.type, num);
    if (isupper(*(names.implicit))) {
      *names.implicit = tolower(*(names.implicit)); // names.implicit in lowercase
    }
    names.instance = names.implicit;
  }
  else {
    names.instance = names.given;
  }

  setObjectName(names.instance);
  names.world = World::current()->getName();

  if (*names.owner == 0) {
    setOwner("public");  // public by default
  }
}

/* Updates the Bounding Box */
void WObject::updateBB()
{
  if (! solid || removed) return;
  solid->updateBB(pos.az);
  solid->getAbsBB(pos.bbc, pos.bbs);
}

/* Inits 3D and grid position */
void WObject::initPosition()
{
  update3D(pos);
  if (bbBehavior()) {
    updateBB();
    insertIntoGrid();
  }
}

/* Updates 3D position */
void WObject::updatePosition()
{
  updateAll3D(pos);
  if (bbBehavior()) {
    updateBB();
  }
  pos.alter = true;	// has changed
  updateDist();
}

/* Updates 3D and grid position */
void WObject::updatePositionAndGrid(Pos &oldpos)
{
  updatePosition();
  if (bbBehavior()) updateGrid(oldpos);
}

void WObject::updatePositionAndGrid(WObject *pold)
{
  updatePosition();
  if (bbBehavior()) updateGrid(pold);
}

/* accessor */
void WObject::updateDist()
{
  if (! solid || removed) return;
  solid->updateDist();
}

//
// VRSql interface
//

/** Checks whether position is managed by VRSql
 * if it is, get position
 */
void WObject::getPersist()
{
  if (! vrsql) vrsql = new VRSql();	// first take the VRSql handle;
  vrsql->getPos(this);
  updatePersist();
}

int16_t WObject::getPersist(int16_t state)
{
  if (! vrsql) vrsql = new VRSql();	// first take the VRSql handle;
  int st = vrsql->getState(this);
  //echo("state: name=%s state=%d", names.given, st);
  state = (st != ERR_SQL) ? st : 0; // updates state
  return state;
}

bool WObject::checkPersist()
{
  if (! vrsql) vrsql = new VRSql();
  int rows = vrsql->countRows(names.type);
  return rows;
}

void WObject::setPersist()
{
  if (! vrsql) vrsql = new VRSql();
  vrsql->deleteRow(this, names.given);
  vrsql->insertRow(this);
}

void WObject::updatePersist()
{
  if (! vrsql) vrsql = new VRSql();	// first take the VRSql handle;
  vrsql->deleteRow(this, names.given);
  vrsql->insertRow(this);
}

#if 0 //notused
/* Updates state for VRSql */
void WObject::updatePersist(int16_t _state)
{
  if (! vrsql) vrsql = new VRSql();	// first take the VRSql handle;
  vrsql->deleteRow(this, names.given);
  vrsql->insertRow(this);
}
#endif //notused

/** Flushes position for VRSql
 * if it is the case, get position and update it
 */
void WObject::savePersist()
{
  if (! vrsql) vrsql = new VRSql();	// first take the VRSql handle;
  if (vrsql && isBehavior(PERSISTENT) && !removed) {
    vrsql->deleteRow(this, names.given);
    vrsql->insertRow(this);
    vrsql->quit();
  }
}

void WObject::delPersist()
{
  if (! vrsql) vrsql = new VRSql();	// first take the VRSql handle;
  vrsql->deleteRow(this, names.given);
}

//
// Actions
//

/** Calls special methods for each object
 * called by GUI
 */
void WObject::specialAction(int act_id, void *data, time_t s, time_t us)
{
  if (isAction(type, act_id)) {
    doAction(type, act_id, this, data, s, us);
  }
}

bool WObject::runAction(const char *action)
{
  for (int i=0; i < ACTIONSNUMBER; i++) {
    if (! strcmp(getActionName(type, i), action)) {
      doAction(type, i, this, (void *)NULL, 0, 0);
      return true;
    }
  }
  return false;
}

/* Adds an object into the deleteList */
void WObject::toDelete()
{
  if (isValid()) {
    deleteList.push_back(this); // add to deleteList
    delFromGrid();
    removed = true;		// mark as removed
  }
  //dax8 delSolids();
  //dax8 if (solid) delete solid;
  solid = NULL;
}

/* Informs the GUI that a (possibly selected) object has been destroyed */
void WObject::clearObjectBar()
{
  ::g.gui.clearInfoBar(this);
}

/**
 * Gives instance or class and action names of an object if it exists
 * called by GUI for infoBar
 */
void WObject::getObjectNames(char **classname, char **instancename, char **actionnames)
{
  int a;
  static char actionname[ACTIONSNUMBER][ACTIONNAME_LEN];

  *classname    = (char *) typeName();
  *instancename = (char *) getInstance();
  *actionnames  = (char *) actionname;

  // clean actionname
  for (a=0; a < ACTIONSNUMBER; a++) {
    memset(actionname[a], 0, ACTIONNAME_LEN);
  }

  // copy actionname
  for (a=0; isActionName(type, a); a++) {
    copyActionName(type, a, actionname[a]);
    //echo("getObjectNames: t=%d a=%d n=%s", type, a, actionname[a]);
  }
  actionname[a][0] = '\0';
}

/**
 * Executes an object's click method if it's defined.
 * By Mathieu Seigneurin
 */
void WObject::click(GLint x, GLint y)
{
  V3 dir;

  ::g.render.clickDirection(x, y, &dir);
  click(dir);	// execute click method if exists
}

//
// Parse
//

/* parse accessors */
Parse * WObject::parse()
{
  return Parse::getParse();
}

char * WObject::parseAttributes(char *l)
{
  return Parse::getParse()->parseAttributes(l, this);
}

void WObject::parseSolid(char *ptok)
{
  Parse::getParse()->parseSolid(ptok, SEP, this);
}

void WObject::parseSolids(char *ptok)
{
  Parse::getParse()->parseSolid(ptok, SEP, this);
}

char * WObject::parseUrl(char *l, char *url)
{
  return Parse::getParse()->parseUrl(l, url);
}

char * WObject::parseName(char *l, char *name)
{
  return Parse::getParse()->parseName(l, name);
}

char * WObject::parseChannel(char *l, char *chan)
{
  return Parse::getParse()->parseChannel(l, chan);
}

char * WObject::parseColor(char *l, Pos &p)
{
  return Parse::getParse()->parseColor(l, p);
}

char * WObject::parseInt(char *l, int *value, const char *attrkey)
{
  return Parse::getParse()->parseInt(l, value, attrkey);
}

char * WObject::parseUInt8(char *l, uint8_t *value, const char *attrkey)
{
  return Parse::getParse()->parseUInt8(l, value, attrkey);
}

char * WObject::parseUInt16(char *l, uint16_t *value, const char *attrkey)
{
  return Parse::getParse()->parseUInt16(l, value, attrkey);
}

char * WObject::parseBool(char *l, bool *value, const char *attrkey)
{
  return Parse::getParse()->parseBool(l, value, attrkey);
}

char * WObject::parseFloat(char *l, float *value, const char *attrkey)
{
  return Parse::getParse()->parseFloat(l, value, attrkey);
}

char * WObject::parseVector3f(char *l, float *vector, const char *attrkey)
{
  return Parse::getParse()->parseVector3f(l, vector, attrkey);
}

char * WObject::parseVector3fv(char *l, V3 *vector, const char *attrkey)
{
  return Parse::getParse()->parseVector3fv(l, vector, attrkey);
}

char * WObject::parseString(char *l, char *str, const char *attrkey)
{
  return Parse::getParse()->parseString(l, str, attrkey);
}

char * WObject::parseString(char *l, char *str)
{
  return Parse::getParse()->parseString(l, str);
}

char * WObject::parseQuotedString(char *l, char *str, const char *attrkey)
{
  return Parse::getParse()->parseQuotedString(l, str, attrkey);
}

char * WObject::parseQuotedString(char *l, char *str)
{
  return Parse::getParse()->parseQuotedString(l, str);
}

char * WObject::parseCaption(char *l, char *str, const char *attrkey)
{
  return Parse::getParse()->parseCaption(l, str, attrkey);
}

char * WObject::parseRotation(char *l, Pos &p)
{
  return Parse::getParse()->parseRotation(l, p);
}

char * WObject::parseTranslation(char *l, Pos &p)
{
  return Parse::getParse()->parseTranslation(l, p);
}

char * WObject::parseGuide(char *l, float path[][5], uint8_t *segs)
{
  return Parse::getParse()->parseGuide(l, &path[1], segs);
}

/* parse tag : tokenize the line */
char * WObject::tokenize(char *l)
{
  Parse *parser = parse();

  char tag[Parse::TAG_LEN +2];
  if (*parser->tagobj) {
    // ignore close tag
    sprintf(tag, "</%s", parser->tagobj);
    char *p = strstr(l, tag);	// </type>
    if (p) *p = '\0';
  }

  // save solid string into geomsolid for VRSql purposes
  char *p = strstr(l, "<solid");
  if (p) {
    char *q, *s;
    geomsolid = new char[strlen(l)];
    strcpy(geomsolid, ++p);
    for (s = geomsolid; (q = strstr(s, "/>")) ; ) {
      s = q;			// end of geomsolid found
      p = strstr(s, "<solid");  // search next solid
      if (p) {
        s = p;
        continue;		// next
      }
      else {
        *s = '\0';
        break;			// end
      }
    }
    *s = '\0';
    for (s = geomsolid; (p = strchr(s, '<')) ; ) {
      *p = ' ';
      s = ++p;
    }
  }

  // tokenizes the object line, make all tokens of this line
  return strtok(l, SEP);
}


//
// List
//

/* Deletes a pointer of this object in an olist */
void WObject::delFromList(list<WObject*> &olist)
{
  for (list<WObject*>::iterator it = olist.begin(); it != olist.end(); ++it) {
    if (*it == this) {
      olist.remove(*it);
    }
  }
  return;
}

/* called by addToListOnce */
OList * WObject::addToList(OList *olist)
{
  if (! isValid()) return olist;

  OList *ol = new OList();
  ol->pobject = this;
  ol->next = olist;
  return ol;
}

/* called by addListToList */
OList * WObject::addOListOnce(OList *olist)
{
  for (OList *ol = olist; ol ; ol = ol->next) {
    if (ol->pobject && ol->pobject == this) {
      return olist;		// already in the list
    }
  }
  return addToList(olist);	// add it into the list
}

OList * WObject::delOList(OList *olist)
{
  OList *front = olist, *ol = NULL;

  if (! olist) {
    error("delOList: %s:%s NULL olist", names.type, getInstance());
    return NULL;
  }
  for (ol = olist; ol ; ol = ol->next) {  // sometimes crashes
    if (ol->pobject == this) {
      if (ol->next) {
        front = ol->next;
        if (ol) {
          //dax ol->deleted = true;
          delete ol;
        }
        ol = NULL;
        return front;
      }
    }
  }
  return front;
}

// static
WObject * WObject::byNum(uint16_t num)
{
  for (vector<WObject*>::iterator it = objectList.begin(); it != objectList.end(); ++it) {
    if ((*it)->num == num) return *it;
  }
  return (WObject *) NULL;
}

/* Concatenates (with test of ispointed & object) pointers list on an object */
OList * WObject::addListToList(OList *l1, OList *l2)
{
  if (! l1) {	// l1 is null
    if (! l2) {
      return NULL;
    }
    if (! l2->pobject) {
      return NULL;
    }
    if (l2->pobject != this) {
      return l2;
    }
    else {
      return NULL;
    }
  }
  else {
    if (l1->pobject && !(l1->pobject->inlist) && (l1->pobject != this)) {
      l1->pobject->inlist = true;
      if (l1->next) {
        return  addListToList(l1->next, l1->pobject->addOListOnce(l2));
      }
      else {
        return l1->pobject->addOListOnce(l2);
      }
    }
    else {
      //if (! l2) {
        //return NULL;
      //}
      if (l1->next) {
        return addListToList(l1->next, l2);
      }
      else {
        return l2;
      }
    }
  }
}

#if 0 // debug
// static
void WObject::show(const char *name)
{
  for (list<WObject*>::iterator it = objectList.begin(); it != objectList.end(); ++it) {
    if (! strcmp((*it)->names.instance, name)) {
      trace(DBG_FORCE, "%s p=%.2f,%.2f,%.2f it=%.2f,%.2f c=%.2f,%.2f,%.2f s=%.2f,%.2f,%.2f",
            name,
            (*it)->pos.x, (*it)->pos.y, (*it)->pos.z,
            (*it)->pos.ax, (*it)->pos.az,
            (*it)->pos.bbc.v[0], (*it)->pos.bbc.v[1], (*it)->pos.bbc.v[2],
            (*it)->pos.bbs.v[0], (*it)->pos.bbs.v[1], (*it)->pos.bbs.v[2]
           );
      break;
    }
  }
}
#endif


//
// Network
//

bool WObject::updatePosToNetwork(const Pos &oldpos, int propxy, int propz, int propaz, int propax, int propay)
{
  bool change = false;

  if ((pos.x != oldpos.x) || (pos.y != oldpos.y)) {
    netop->declareObjDelta(propxy);
    change = true;
  }
  if (ABSF(pos.z - oldpos.z) > 0.1) {
    netop->declareObjDelta(propz);
    change = true;
  }
  if (pos.az != oldpos.az) {
    netop->declareObjDelta(propaz);
    change = true;
  }
  if (pos.ax != oldpos.ax) {
    netop->declareObjDelta(propax);
    change = true;
  }
  if (pos.ay != oldpos.ay) {
    netop->declareObjDelta(propay);
    change = true;
  }
  return change;
}

/*
 * get_ functions
 */
void WObject::get_xy(WObject *po, Payload *pp)
{
  Pos oldpos = po->pos;
  pp->getPayload("ff", &po->pos.x, &po->pos.y);
  po->updatePositionAndGrid(oldpos);
}

void WObject::get_z(WObject *po, Payload *pp)
{
  Pos oldpos = po->pos;
  pp->getPayload("f", &po->pos.z);
  po->updatePositionAndGrid(oldpos);
}

void WObject::get_az(WObject *po, Payload *pp)
{
  Pos oldpos = po->pos;
  pp->getPayload("f", &po->pos.az);
  po->updatePositionAndGrid(oldpos);
}

void WObject::get_ay(WObject *po, Payload *pp)
{
  Pos oldpos = po->pos;
  pp->getPayload("f", &po->pos.ay);
  po->updatePositionAndGrid(oldpos);
}

void WObject::get_ax(WObject *po, Payload *pp)
{
  Pos oldpos = po->pos;
  pp->getPayload("f", &po->pos.ax);
  po->updatePositionAndGrid(oldpos);
}

void WObject::get_hname(WObject *po, Payload *pp)
{
  pp->getPayload("s", po->names.type);
}

/*
 * put_ functions
 */
void WObject::put_xy(WObject *po, Payload *pp)
{
  pp->putPayload("ff", po->pos.x, po->pos.y);
}

void WObject::put_z(WObject *po, Payload *pp)
{
  pp->putPayload("f", po->pos.z);
}

void WObject::put_az(WObject *po, Payload *pp)
{
  pp->putPayload("f", po->pos.az);
}

void WObject::put_ay(WObject *po, Payload *pp)
{
  pp->putPayload("f", po->pos.ay);
}

void WObject::put_ax(WObject *po, Payload *pp)
{
  pp->putPayload("f", po->pos.ax);
}

void WObject::put_hname(WObject *po, Payload *pp)
{
  pp->putPayload("s", po->names.type);
}

/** Gets property from Network */
void WObject::getProperty(uint8_t prop_id, Payload *pp)
{
  if (! isGetPropertyFunc(type, prop_id)) {
    error("getProperty: prop=%d doesn't match the object type=%d", prop_id, type);
    return;
  }
  runGetPropertyFunc(type, prop_id, this, pp);
}

/** Puts property to Network */
void WObject::putProperty(uint8_t prop_id, Payload *pp)
{
  if (! isPutPropertyFunc(type, prop_id)) {
    error("putProperty: prop=%d undefined for object=%d", prop_id, type);
    return;
  }
  runPutPropertyFunc(type, prop_id, this, pp);
}

void WObject::deleteReplica()
{
  if (this != localuser) {
    if (type == USER_TYPE) ::g.gui.removeUser((User *) this);
    delFromGrid();
    delFromList(mobileList);

    // delete Solids
    list<Solid*> solList = ::g.render.getSolidList();
    for (list<Solid*>::iterator s = solList.begin(); s != solList.end(); s++) {
      delete (*s);
    }
    solList.clear();

    if (netop) delete netop;
    netop = NULL;
  }
  else
    echo("%s disapeared, but he is back!", getInstance());
}
