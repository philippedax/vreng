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
#include "prof.hpp"	// new_wobject
#include "timer.hpp"	// ::g.timer

#include <list>
using namespace std;

// global
list<WObject*> objectList;
list<WObject*> stillList;
list<WObject*> mobileList;
list<WObject*> fluidList;
list<WObject*> invisList;
list<WObject*> deleteList;
list<WObject*> lightList;

// local
static uint32_t objectNum = 0;


/* WObject constructor */
WObject::WObject()
{
  new_wobject++;

  noh = NULL;
  guip = NULL;
  inlist = false;
  removed = false;
  objectbar = false;

  state = NONE;
  type = 0;
  mode = 0;
  behavior = 0;

  memset(names.type, 0, sizeof(names.type));
  memset(names.given, 0, sizeof(names.given));
  memset(names.url, 0, sizeof(names.url));
  memset(names.owner, 0, sizeof(names.owner));
  memset(chan, 0, sizeof(chan));

  names.implicit = NULL;
  names.instance = NULL;
  names.world = NULL;
  names.category = NULL;
  names.infos = NULL;
  geometry = NULL;

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

  noid.src_id = 0;
  noid.port_id = 0;
  noid.obj_id = 0;

  prior = PRIOR_MEDIUM;

  solid = NULL;
  ::g.render.relsolidList.clear();

#if VRSQL
  psql = NULL;
#endif
  flare = NULL;
}

/* WObject destructor */
WObject::~WObject()
{
  if (! isBehavior(COLLIDE_NEVER)) {
    delFromGrid();
  }

  deleteSolids();	// delete all solids

  // delete NetObject
  if (noh && (mode == MOBILE)) {
    if (! isPermanent()) {
      noh->declareDeletion();
    }
    delete noh;
    noh = NULL;
  }
  if (names.implicit) delete[] names.implicit;
  if (names.category) delete[] names.category;
  if (names.infos) delete[] names.infos;
  if (geometry) delete[] geometry;
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
  addToList(objectList);	// add to objectList

  //error("num=%d mode=%d type=%d", num, mode, type);
  switch (mode) {

    case STILL:
      addToList(stillList);	// add to stillList
      break;

    case MOBILE:
      if (isBehavior(PERSISTENT)) {
        getPersistency();	// calls persistency VRSql
      }
      addToList(mobileList);	// add to mobileList
      break;

    case FLUID:
      enableBehavior(NO_ELEMENTARY_MOVE);
      addToList(fluidList);	// add to fluidList
      break;

    case INVISIBLE:
      enableBehavior(NO_BBABLE);
      enableBehavior(UNSELECTABLE);
      enableBehavior(NO_ELEMENTARY_MOVE);
      addToList(invisList);	// add to invisList
      break;

    case EPHEMERAL:
      enableBehavior(NO_BBABLE);
      enableBehavior(UNSELECTABLE);
      enableBehavior(NO_ELEMENTARY_MOVE);
      addToList(mobileList);	// add to mobileList
      break;

    case MOBILEINVISIBLE:
      enableBehavior(NO_BBABLE);
      enableBehavior(UNSELECTABLE);
      enableBehavior(NO_ELEMENTARY_MOVE);
      addToList(mobileList);
      addToList(invisList);	// add to invisList
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

void WObject::setRenderPrior(uint8_t _prior)
{
  prior = _prior;
}

uint8_t WObject::getRenderPrior() const
{
  return prior;
}

bool WObject::isValid() const
{
  return OClass::isValidType(type);
}

void WObject::setType(int _type)
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
  return (noh) ? noh->isPermanent() : false;
}

bool WObject::isSeen()
{
  V3 vseen = ::g.render.getVisiblePosition(this);
  return vseen.v[2];  // seen = v[2]
}

uint32_t WObject::collideBehavior() const
{
  return (behavior & (COLLIDE_ONCE | COLLIDE_NEVER | COLLIDE_GHOST));
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
void WObject::getObjectNameById(uint8_t type_id, char *name)
{
  const OClass *oclass = OClass::getOClass(type_id);
  if (! oclass)
    warning("getObjectNameById: no name found for type=%d", type_id);
  else
    strcpy(name, oclass->type_name);
  return;
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

/* solids accessor */
const WObject::SolidList& WObject::solids() const
{
  return _solids;
}

uint32_t WObject::countOfSolids() const
{
  return _solids.size();
}

/* returns the first solid of the object - accessor */
Solid* WObject::getSolid() const
{
  return solid;
}

void WObject::setVisible(bool flag)
{
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
  _solids.push_back(psolid);	// add solid to solidList
  psolid->wobject = this;	// Solid is friend of WObject
  solid = psolid;		// keep solid pointer in WObject
}

/* deletes all solids of this object */
void WObject::deleteSolids()
{
  if (_solids.empty()) return;
  for (SolidList::iterator it = _solids.begin(); it != _solids.end(); ++it) {
    delete(*it);
  }
  _solids.erase(_solids.begin(), _solids.end());
}

void WObject::getPosition(M4& mpos)
{
  if (solid) solid->getPosition(mpos);
}

void WObject::getMaterials(GLfloat *dif, GLfloat *amb, GLfloat *spe, GLfloat *emi, GLint *shi, GLfloat *alpha)
{
  if (solid) solid->getMaterials(dif, amb, spe, emi, shi, alpha);
}

/* Gets relative center and size of solid */
void WObject::getRelBB(V3 &center, V3 &size)
{
  if (solid) solid->getRelBB(center, size);
}

/* Gets size of solid */
void WObject::getDimBB(V3 &dim)
{
  if (solid) solid->getDimBB(dim);
}

/* Gets relative center of solid */
void WObject::getCentBB(V3 &center)
{
  if (solid) solid->getCentBB(center);
}

/* Gets absolute center and size of solid */
void WObject::getAbsBB(V3 &center, V3 &size)
{
  if (solid) solid->getAbsBB(center, size);
}

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
  if (solid) solid->resetFlashyEdges();
}

//
// net
//

void WObject::setSrcId(uint32_t _src_id)
{
  noid.src_id = _src_id;
}

uint32_t WObject::getSrcId() const
{
  return noid.src_id;
}

void WObject::setPortId(uint16_t _port_id)
{
  noid.port_id = _port_id;
}

uint16_t WObject::getPortId() const
{
  return noid.port_id;
}

void WObject::setObjId(uint16_t _obj_id)
{
  noid.obj_id = _obj_id;
}

uint16_t WObject::getObjId() const
{
  return noid.obj_id;
}

/* Assigns a unique identifier to each Vreng object */
void WObject::setWObjectId()
{
  noid.src_id = NetObject::getMySsrcId();	// Application's identifier
  noid.port_id = NetObject::getMyPortId();	// Comm port identifier
  NetObject::setMyObjId(NetObject::getMyObjId() + 1);	// myObjId++
  noid.obj_id = htons(NetObject::getMyObjId()); // Application wide unique number
}

/* Copies Noid in WObjectId */
void WObject::copyNoid(Noid _noid)
{
  noid.src_id = _noid.src_id;
  noid.port_id = _noid.port_id;
  noid.obj_id = _noid.obj_id;
}

/* Creates local permanent NetObject */
void WObject::createPermanentNetObject(uint8_t props, uint16_t oid)
{
  noh = new NetObject(this, props, oid);
}

/* Creates local volatile NetObject */
void WObject::createVolatileNetObject(uint8_t props)
{
  noh = new NetObject(this, props);
}

/* Replicates distant volatile NetObject */
void WObject::replicateVolatileNetObject(uint8_t props, Noid _noid)
{
  noh = new NetObject(this, props, _noid);
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
#if VRSQL
    if (psql) psql->deleteRow(this);
#endif
    toDelete();
    clearObjectBar();
    return true;
  }
  else {
    notice("Permission denied, owner is %s", names.owner);
    removed = false;
    return false;
  }
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

  setObjName(names.instance);
  names.world = World::current()->getName();

  if (*names.owner == 0) {
    setOwner("public");  // public by default
  }
}

/* Updates the Bounding Box */
void WObject::updateBB()
{
  if (! solid) return;
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
  update3D(pos);
  if (bbBehavior()) updateBB();
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
  if (! solid) return;
  solid->updateDist();
}

void WObject::moveObject(WObject *po, void *d, time_t s, time_t u)
{
  localuser->carrier->take(po);
}

//
// VRSql interface
//

/** Checks whether position is managed by VRSql
 * if it is, get position
 */
void WObject::getPersistency()
{
#if VRSQL
  if (! psql) psql = VRSql::getVRSql();	// first take the VRSql handle;
  if (psql && givenName()) {
    psql->getPos(this);
  }
  updatePersistency();
#endif
}

void WObject::getPersistency(int16_t state)
{
#if VRSQL
  if (! psql) psql = VRSql::getVRSql();	// first take the VRSql handle;
  if (psql && givenName()) {
    int st = psql->getState(this);
    trace(DBG_SQL, "state: name=%s state=%d", names.instance, st);
    state = (st != ERR_SQL) ? st : 0; // updates state
  }
#endif
}

void WObject::updatePersistency()
{
#if VRSQL
  if (! psql) psql = VRSql::getVRSql();	// first take the VRSql handle;
  if (psql && givenName()) {
    progression('m');
    ::g.timer.mysql.start();
    psql->updatePos(this);
    pos.z = psql->getPosZ(this);
    psql->updatePosZ(this);
    ::g.timer.mysql.stop();
  }
#endif
}

/* Updates state for VRSql */
void WObject::updatePersistency(int16_t _state)
{
#if VRSQL
  if (! psql) psql = VRSql::getVRSql();	// first take the VRSql handle;
  if (psql && givenName()) {
    ::g.timer.mysql.start();
    progression('m');
    psql->updateState(this, _state);
    ::g.timer.mysql.stop();
  }
#endif
}

/** Flushes position for VRSql
 * if it is the case, get position and update it
 */
void WObject::savePersistency()
{
#if VRSQL
  if (! psql) psql = VRSql::getVRSql();	// first take the VRSql handle;
  if (psql && isBehavior(PERSISTENT) && !removed && givenName()) {
    //trace(DBG_FORCE, "savePersistency: %s pos.alter=%d", names.instance, pos.alter);
    // update VRSql table only if object has changed
    if (pos.alter) psql->updatePos(this);
    if (isBehavior(DYNAMIC)) psql->updateOwner(this);
    psql->quit();
  }
#endif
}

/* Quits VRSql */
void WObject::quitPersistency()
{
#if VRSQL
  if (psql) psql->quit();
#endif
}

//
// Actions
//

/* Returns how many actions have this object */
uint8_t WObject::numberAction()
{
  int i;
  for (i=0; isAction(typeId(), i); i++) ;
  return i;
}

/* Checks if this object have actions */
bool WObject::haveAction()
{
  if (! isValid()) return false;
  return isAction(typeId(), 0);
}

/* Adds an object into the deleteList */
void WObject::toDelete()
{
  if (isValid()) {
    deleteList.push_back(this); // add to deleteList
    delFromGrid();
    removed = true;		// mark as removed
  }
  //dax8 deleteSolids();
  //dax8 if (solid) { delete solid; solid = NULL; }
}

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

/* Informs the GUI that a (possibly selected) object has been destroyed */
void WObject::clearObjectBar()
{
  if (objectbar) ::g.gui.clearInfoBar(this);
}

/**
 * Gives instance or class and method names of an object if it exists
 * called by GUI for infoBar
 */
void WObject::getObjectHumanName(char **classname, char **instancename, char **actionnames)
{
  int a;
  static char methodname[ACTIONSNUMBER][ACTIONNAME_LEN];

  *classname    = (char *) typeName();
  *instancename = (char *) getInstance();
  *actionnames  = (char *) methodname;

  // clean methodname
  for (a=0; a < ACTIONSNUMBER; a++) memset(methodname[a], 0, ACTIONNAME_LEN);

  // copy method names
  for (a=0; isActionName(type, a); a++) {
    copyActionName(type, a, methodname[a]);
    trace(DBG_WO, "getObjectHumanName: type=%d a=%d name=%s", type, a, methodname[a]);
  }
  methodname[a][0] = '\0';
  objectbar = true;
}

/**
 * Execute an object's click method if it's defined.
 * By Mathieu Seigneurin
 */
void WObject::click(GLint x, GLint y)
{
  V3 dir;

  //error("x=%d y=%d", x,y);
  ::g.render.clickDirection(x, y, &dir);
  click(dir);	// execute click method if exists
}

/* accessor */
Parse * WObject::parse()
{
  return Parse::getParse();
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

  // save solid string into geometry for VRSql purposes
  char *p = strstr(l, "<solid");
  if (p) {
    char *q, *s;
    geometry = new char[strlen(l)];
    strcpy(geometry, ++p);
    for (s = geometry; (q = strstr(s, "/>")) ; ) {
      s = q;			// end of geometry found
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
    for (s = geometry; (p = strchr(s, '<')) ; ) {
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

/* Clears an olist */
void WObject::clearList(list<WObject*> &olist)
{
  olist.clear();
}

/* Adds a pointer of this object in an olist */
void WObject::addToList(list<WObject*> &olist)
{
  olist.push_back(this);
}

/* Adds a pointer of this object to an olist if it's not already there */
void WObject::addToListOnce(list<WObject*> &olist)
{
  for (list<WObject*>::iterator it = olist.begin(); it != olist.end(); ++it) {
    if (*it == this) {
      return;		// already in the list
    }
  }
  olist.push_back(this);	// add it into the list
}

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

OList * WObject::addToList(OList *olist)
{
  if (! isValid()) return olist;

  OList *ol = new OList();
  ol->pobject = this;
  ol->next = olist;
  return ol;
}

OList * WObject::addOListOnce(OList *olist)
{
  for (OList *ol = olist; ol ; ol = ol->next) {
    if (ol->pobject && ol->pobject == this)
      return olist;		// already in the list
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
        if (ol) delete ol;
        ol = NULL;
        return front;
      }
    }
  }
  return front;
}

bool WObject::isStill()
{
  for (list<WObject*>::iterator it = stillList.begin(); it != stillList.end(); ++it) {
    if (*it == this)  return true;
  }
  return false;
}

bool WObject::isMobile()
{
  for (list<WObject*>::iterator it = mobileList.begin(); it != mobileList.end(); ++it) {
    if (*it == this)  return true;
  }
  return false;
}

bool WObject::isFluid()
{
  for (list<WObject*>::iterator it = fluidList.begin(); it != fluidList.end(); ++it) {
    if (*it == this)  return true;
  }
  return false;
}

bool WObject::isEphemeral()
{
  for (list<WObject*>::iterator it = mobileList.begin(); it != mobileList.end(); ++it) {
    if (*it == this && (*it)->mode == EPHEMERAL)  return true;
  }
  return false;
}

// virtual
WObject * WObject::byWObject(WObject *po)
{
  for (list<WObject*>::iterator it = objectList.begin(); it != objectList.end(); ++it) {
    if ((*it) == po) return *it;
  }
  return (WObject *) NULL;
} 

// static
WObject * WObject::byNum(uint16_t num)
{
  for (list<WObject*>::iterator it = objectList.begin(); it != objectList.end(); ++it) {
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
    noh->declareObjDelta(propxy);
    change = true;
  }
  if (ABSF(pos.z - oldpos.z) > 0.1) {
    noh->declareObjDelta(propz);
    change = true;
  }
  if (pos.az != oldpos.az) {
    noh->declareObjDelta(propaz);
    change = true;
  }
  if (pos.ax != oldpos.ax) {
    noh->declareObjDelta(propax);
    change = true;
  }
  if (pos.ay != oldpos.ay) {
    noh->declareObjDelta(propay);
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
  //debug error("getProperty: prop=%d type=%d", prop_id, type);
  if (! isGetPropertyFunc(type, prop_id)) {
    error("getProperty: prop=%d doesn't match the object type=%d", prop_id, type); return;
  }
  runGetPropertyFunc(type, prop_id, this, pp);
}

/** Puts property to Network */
void WObject::putProperty(uint8_t prop_id, Payload *pp)
{
  if (! isPutPropertyFunc(type, prop_id)) {
    error("putProperty: prop=%d undefined for object=%d", prop_id, type); return;
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
    for (SolidList::iterator s = _solids.begin(); s != _solids.end(); s++) {
      delete (*s);
    }
    _solids.erase(_solids.begin(), _solids.end());

    if (noh) delete noh;
    noh = NULL; // delete NetObject
  }
  else warning("%s disapeared, but he is back!", getInstance());
}
