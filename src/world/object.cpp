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
// object.cpp
//
// Objects management
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "object.hpp"
#include "matvec.hpp"	// V3 M4
#include "world.hpp"	// World::current
#include "user.hpp"	// localuser
#include "netobj.hpp"	// NetObj
#include "sql.hpp"	// VSql
#include "solid.hpp"	// Solid
#include "olist.hpp"	// OList
#include "gui.hpp"	// clearInfoBar, removeAvatar
#include "payload.hpp"	// Payload
#include "carrier.hpp"	// carrier->take
#include "render.hpp"	// getVisiblePosition
#include "stat.hpp"	// new_wobject
#include "timer.hpp"	// ::g.timer

#include <list>
#include <vector>

// global
std::vector<Object*> objectList;
std::vector<Object*> stillList;
std::list<Object*> mobileList;
std::vector<Object*> fluidList;
std::vector<Object*> clothList;
std::vector<Object*> invisList;
std::vector<Object*> deleteList;
std::vector<Object*> lightList;

// local
static uint32_t objectNum = 0;
static struct Hash hashtable[NAME_HASH_SIZE];


/** Object constructor */
Object::Object()
{
  new_wobject++;

  netop = NULL;
  guip = NULL;
  pointed = false;
  removed = false;
  deleted = false;
  visible = true;

  state = NONE;
  type = 0;
  mode = 0;
  behavior = 0;

  name.type = new char[TYPENAME_LEN];
  name.url = new char[URL_LEN];
  *name.type = '\0';
  *name.url = '\0';

  name.given = NULL;
  name.owner = NULL;
  name.implicit = NULL;
  name.current = NULL;
  name.world = NULL;
  name.category = NULL;
  name.infos = NULL;
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
  vsql = NULL;
}

/** Object destructor */
Object::~Object()
{
  if (name.type) delete[] name.type;
  if (name.given) delete[] name.given;
  if (name.url) delete[] name.url;
  if (name.owner) delete[] name.owner;
  if (name.implicit) delete[] name.implicit;
  if (name.category) delete[] name.category;
  if (name.infos) delete[] name.infos;
  if (geomsolid) delete[] geomsolid;

  if (! isBehavior(COLLIDE_NEVER)) {
    delGrid();
  }

  delSolids();	// delete all solids

  // delete NetObj
  if (netop && (mode == MOBILE)) {
    if (! isPermanent()) {
      netop->declareDeletion();
    }
    delete netop;
    netop = NULL;
  }
  del_wobject++;
}

/** Initializes Object. */
void Object::initObject(uint8_t _mode)
{
  mode = _mode;
  type = typeId();
  num = objectNum++;

  //echo("num=%d mode=%d type=%d", num, mode, type);
  setObjectId();
  initPosition();
  objectList.push_back(this);	// add to objectList

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
  if (! isBehavior(NO_BBABLE)) {
    updateBB();
    addGrid();
  }
  updateNames();
}

/** Initializes still object */
void Object::initStillObject()
{
  initObject(STILL);
}

/** Initializes mobile object */
void Object::initMobileObject(float last)
{
  initObject(MOBILE);
  if (last) {
    setLasting(last);
  }
}

/** Initializes fluid object */
void Object::initFluidObject(float last)
{
  initObject(FLUID);
  if (last) {
    setLasting(last);
  }
}

/** Initializes cloth object */
void Object::initClothObject(float last)
{
  initObject(CLOTH);
  if (last) {
    setLasting(last);
  }
}

/** Initializes ephemeral object */
void Object::initEphemeralObject(float last)
{
  initObject(MOBILE);
  if (last) {
    setLasting(last);
  }
}

/** Enables a behavior */
void Object::enableBehavior(uint32_t flag)
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

/** Disables a behavior */
void Object::disableBehavior(uint32_t flag)
{
  behavior &= ~flag;

  switch (flag) {
    case UNVISIBLE: setVisible(true); break;
  }
}

/** Checks a behavior */
bool Object::isBehavior(uint32_t flag) const
{
  return (behavior & flag);
}

/** Checks if valid type */
bool Object::isValid() const
{
  return OClass::isValidType(type);
}

/** Sets type */
void Object::setType(uint8_t _type)
{
  type = _type;
}

/** Checks if is permanent */
bool Object::isPermanent() const
{
  return (netop) ? netop->isPermanent() : false;
}

/** Checks if is seen */
bool Object::isSeen()
{
  V3 vseen = ::g.render.getVisiblePosition(this);
  return vseen.v[2];  // seen = v[2]
}

/** Sets the owner name */
void Object::setOwner(const char *_owner)
{
  name.owner = new char[USER_LEN];
  strcpy(name.owner, _owner);
}

/** Sets the owner */
void Object::setOwner()
{
  if (localuser)
    setOwner(localuser->name.current);
  else
    setOwner("me");
}

/** Gets current object number */
uint16_t Object::getNum()
{
  num = ++objectNum;
  return num;
}

/** Returns object's name */
const char * Object::objectName() const
{
  if (name.given && *name.given) {
    return name.given;
  }
  else if (name.implicit && *name.implicit) {
    return name.implicit;
  }
  else {
    return NULL;
  }
}

/** Sets name.given */
void Object::setGivenName(const char *_name)
{
  if (! name.given) {
    name.given = new char[OBJNAME_LEN];
  }
  strcpy(name.given, _name);
}

/** Returns owner's name */
const char * Object::ownerName() const
{
  return name.owner;
}

/** Returns world's name */
const char * Object::worldName() const
{
  return name.world;
}


////////////////
//
// Solids
//

/** Returns the first solid of the object - accessor */
Solid* Object::getSolid() const
{
  return solid;
}

/** Sets visible - accessor */
void Object::setVisible(bool flag)
{
  visible = flag;
  if (solid) solid->setVisible(flag);
}

/** Checks if is visible - accessor */
bool Object::isVisible() const
{
  if (solid)
    return solid->visible;
  else
    return false;
}

/** Sets a ray - accessor */
void Object::setRay(GLint wx, GLint wy)
{
  if (solid) solid->setRay(wx, wy);
}

/** Resets a ray - accessor */
void Object::resetRay()
{
  if (! isValid()) return;
  if (solid) solid->resetRay();
}

/** Sets reflexive - accessor */
void Object::setReflexive(bool flag)
{
  if (solid) solid->setReflexive(flag);
}

/** Adds a Object pointer to this object in the solidList (friend) */
void Object::addSolid(Solid* psolid)
{
  _solidList.push_back(psolid);	// add solid to solidList
  psolid->wobject = this;	// Solid is friend of Object
  solid = psolid;		// keep solid pointer in Object
}

/** Deletes all solids of this object */
void Object::delSolids()
{
  std::list<Solid*> solList = ::g.render.getSolidList();
  for (std::list<Solid*>::iterator it = solList.begin(); it != solList.end(); ++it) {
    if ((*it)->wobject == this) {
      delete *it;
    }
  }
}

/** Gets 3D position - accessor */
void Object::getPosition(M4& mpos)
{
  if (solid) solid->getPosition(mpos);
}

/** Gets materials - accessor */
void Object::getMaterials(GLfloat *dif, GLfloat *amb, GLfloat *spe, GLfloat *emi, GLint *shi, GLfloat *alpha)
{
  if (solid) solid->getMaterials(dif, amb, spe, emi, shi, alpha);
}

/** Gets size of solid - accessor */
void Object::getDim(V3 &dim)
{
  if (solid) solid->getDimBB(dim);
}

/** Gets relative center and size of solid - accessor */
void Object::getRelBB(V3 &center, V3 &size)
{
  if (solid) solid->getRelBB(center, size);
}

/** Gets the number of frames of this solid - accessor */
uint8_t Object::getFrames()
{
  if (solid) return solid->nbframes;
  else return 0;
}

/** Gets index of current frame of this solid - accessor */
uint8_t Object::getFrame()
{
  if (solid) return solid->frame;
  else return 0;
}

/** Sets a frame number - accessor */
void Object::setFrame(uint8_t _frame)
{
  if (solid) solid->setFrame(_frame);      // set frame
}

/** Set flashy - accessor */
void Object::setFlashy(float *color)
{
  if (solid) solid->setFlashyEdges(color);
}

/** Set flashy - accessor */
void Object::setFlashy()
{
  if (solid) solid->setFlashyEdges(true);
}

/** Reset flashy - accessor */
void Object::resetFlashy()
{
  if (! isValid()) return;
  if (solid) solid->resetFlashyEdges();
}

////////////////
//
// Network
//

uint32_t Object::getSrc() const
{
  return noid.src_id;
}

uint16_t Object::getPort() const
{
  return noid.port_id;
}

uint16_t Object::getObj() const
{
  return noid.obj_id;
}

/** Assigns a unique identifier to each Vreng object */
void Object::setObjectId()
{
  noid.src_id = NetObj::getSsrc();		// Application's identifier
  noid.port_id = NetObj::getPort();		// Comm port identifier
  NetObj::setObj(NetObj::getObj()+1);		// myObjId++
  noid.obj_id = htons(NetObj::getObj());	// Application wide unique number
}

/** Creates local permanent NetObj */
NetObj * Object::createNetObj(uint8_t props, uint16_t oid)
{
  netop = new NetObj(this, props, oid);
  return netop;
}

/** Creates local volatile NetObj */
NetObj * Object::createVolatile(uint8_t props)
{
  netop = new NetObj(this, props);
  return netop;
}

/** Replicates distant volatile NetObj */
NetObj * Object::replicate(uint8_t props, Noid _noid)
{
  noid.src_id = _noid.src_id;
  noid.port_id = _noid.port_id;
  noid.obj_id = _noid.obj_id;
  netop = new NetObj(this, props, _noid);
  return netop;
}

////////////////
//
// Gui
//

/** Removes an object from scene */
bool Object::removeFromScene()
{
  if (! strcmp(name.owner, localuser->name.current)) {
    if (vsql) vsql->deleteRow(this);
    toDelete();
    clearObjectBar();
    return true;
  }
  else {
    echo("permission denied, owner is %s", name.owner);
    removed = false;
    return false;
  }
}

/** Informs the GUI that a (possibly selected) object has been destroyed */
void Object::clearObjectBar()
{
  ::g.gui.clearInfoBar(this);
}

/**
 * Gives current or class and action names of an object if it exists
 * called by GUI for infoBar
 */
void Object::getObjectNames(char **classname, char **currentname, char **actionnames)
{
  int a;
  static char actionname[ACTIONSNUMBER][ACTIONNAME_LEN];

  *classname   = const_cast<char *>(typeName());
  *currentname = const_cast<char *>(objectName());
  *actionnames = reinterpret_cast<char *>(actionname);

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

////////////////
//
// name
//

/** Inits hash table */
void Object::initNames()  
{ 
  for (int i=0; i < NAME_HASH_SIZE; i++) {
    hashtable[i].hname = new char[OBJNAME_LEN];
    memset(hashtable[i].hname, 0, OBJNAME_LEN);
  }
}

/** Returns a hash number from name */
static uint32_t hash_name(const char *s)
{ 
  uint32_t h;

  for (h=0; *s ; s++) {
    h = ((h << 8) + *s) % NAME_HASH_SIZE;
  }
  return h;
} 

/** Sets a name in hash table */
void Object::setObjectName(const char *name)
{ 
  if (! name) return;

  char fullname[OBJNAME_LEN];
  sprintf(fullname, "%s@%s", name, World::current()->name);
  uint32_t hval = hash_name(fullname);
  while (hval) {
    if ((*(hashtable[hval].hname) == '\0') ||
        (! strcmp(hashtable[hval].hname, NAME_DELETED))) {
      strcpy(hashtable[hval].hname, fullname);
      hashtable[hval].po = this;
      return;
    }
    hval = (hval + 1) % NAME_HASH_SIZE;
  }
}

/** Gets an object by its name */
Object * Object::getObject(const char *name)
{
  if (! name) return NULL;

  char fullname[OBJNAME_LEN];
  sprintf(fullname, "%s@%s", name, World::current()->name);
  uint32_t hval = hash_name(fullname);
  trace1(DBG_WO, "getObject: hval=%d name=%s", hval, fullname);
  while (hval) {
    if (*(hashtable[hval].hname) == '\0') {
      return NULL;          // not found
    }
    if (! strcmp(hashtable[hval].hname, fullname)) {
      return hashtable[hval].po;        // found
    }
    hval = (hval + 1) % NAME_HASH_SIZE;
  }
  return NULL;              // not found
}

/** Forces Object name.type name.implicit */
void Object::forceNames(const char *newname)
{
  strcpy(name.type, newname);
  name.implicit = new char[OBJNAME_LEN];
  sprintf(name.implicit, "%s%d", newname, num);
  if (isupper(*(name.implicit))) {
    *name.implicit = tolower(*(name.implicit)); // name.implicit in lowercase
  }
  name.current = name.implicit;
}

/** Updates object name */
void Object::updateNames()
{
  if (! isValid()) return;

  const OClass *oclass = OClass::getOClass(type);
  if (oclass) {
    strcpy(name.type, oclass->type_name);
  }
  else {
    error("updateNames: no name found for type=%d", type);
  }

  if (name.given && *name.given) {
    name.current = name.given;
  }
  else {	// no given name
    name.implicit = new char[OBJNAME_LEN];
    sprintf(name.implicit, "%s%d", typeName(), num);
    if (isupper(*(name.implicit))) {
      *name.implicit = tolower(*(name.implicit)); // name.implicit in lowercase
    }
    name.current = name.implicit;
  }

  setObjectName(name.current);
  name.world = World::current()->name;

  if (name.owner && *name.owner == 0) {
    setOwner("public");  // public by default
  }
}

/** Updates the Bounding Box */
void Object::updateBB()
{
  if (! solid || removed) return;
  solid->updateBB(pos.az);
  solid->getAbsBB(pos.bbc, pos.bbs);
}

/** Inits 3D and grid position */
void Object::initPosition()
{
  update3D(pos);
  if (! isBehavior(NO_BBABLE)) {
    updateBB();
    addGrid();
  }
}

/** Updates 3D position */
void Object::updatePosition()
{
  updateAll3D(pos);
  if (! isBehavior(NO_BBABLE)) {
    updateBB();
  }
  pos.alter = true;	// has changed
  updateDist();
}

/** Updates 3D and grid position */
void Object::updatePositionAndGrid(Pos &oldpos)
{
  updatePosition();
  if (! isBehavior(NO_BBABLE)) updGrid(oldpos);
}

void Object::updatePositionAndGrid(Object *pold)
{
  updatePosition();
  if (! isBehavior(NO_BBABLE)) updGrid(pold);
}

/** Updates distance - accessor */
void Object::updateDist()
{
  if (! solid || removed) return;
  solid->updateDist();
}

////////////////
//
// VSql interface
//

/** Checks whether position is managed by VSql
 * if it is, get position
 */
void Object::getPersist()
{
  if (! vsql) vsql = new VSql();	// first take the VSql handle;
  vsql->getPos(this);
  updatePersist();
}

int16_t Object::getPersist(int16_t state)
{
  if (! vsql) vsql = new VSql();	// first take the VSql handle;
  int st = vsql->getState(this);
  //echo("state: name=%s state=%d", objectName(), st);
  state = (st != ERR_SQL) ? st : 0; // updates state
  return state;
}

bool Object::checkPersist()
{
  if (! vsql) vsql = new VSql();
  int rows = vsql->countRows(typeName());
  return rows;
}

void Object::setPersist()
{
  if (! vsql) vsql = new VSql();
  vsql->deleteRow(this, objectName());
  vsql->insertRow(this);
}

void Object::updatePersist()
{
  if (! vsql) vsql = new VSql();	// first take the VSql handle;
  vsql->deleteRow(this, objectName());
  vsql->insertRow(this);
}

/** Flushes position for VSql
 * if it is the case, get position and update it
 */
void Object::savePersist()
{
  if (! vsql) vsql = new VSql();	// first take the VSql handle;
  if (vsql && isBehavior(PERSISTENT) && !removed) {
    vsql->deleteRow(this, objectName());
    vsql->insertRow(this);
    vsql->quit();
  }
}

void Object::delPersist()
{
  if (! vsql) vsql = new VSql();	// first take the VSql handle;
  vsql->deleteRow(this, objectName());
}

////////////////
//
// Actions
//

/** Calls special methods for each object
 * called by GUI
 */
void Object::specialAction(int act_id, void *data, time_t s, time_t us)
{
  if (isAction(type, act_id)) {
    doAction(type, act_id, this, data, s, us);
  }
}

bool Object::runAction(const char *action)
{
  for (int i=0; i < ACTIONSNUMBER; i++) {
    if (! strcmp(getActionName(type, i), action)) {
      doAction(type, i, this, NULL, 0, 0);
      return true;
    }
  }
  return false;
}

/** Adds an object into the deleteList */
void Object::toDelete()
{
  if (isValid()) {
    deleteList.push_back(this); // add to deleteList
    delGrid();
    removed = true;		// mark as removed
  }
  //dax8 delSolids();
  //dax8 if (solid) delete solid;
  solid = NULL;
}

/**
 * Executes an object's click method if it's defined.
 * By Mathieu Seigneurin
 */
void Object::click(GLint x, GLint y)
{
  V3 dir;

  ::g.render.clickDirection(x, y, &dir);
  click(dir);	// execute click method if exists
}

////////////////
//
// Parsing vre lines
//

/** parse accessor */
Parse * Object::parse()
{
  return Parse::getParse();
}

char * Object::parseAttributes(char *l)
{
  return Parse::getParse()->parseAttributes(l, this);
}

void Object::parseSolid(char *ptok)
{
  Parse::getParse()->parseSolid(ptok, TOK_SEP, this);
}

void Object::parseSolids(char *ptok)
{
  Parse::getParse()->parseSolid(ptok, TOK_SEP, this);
}

char * Object::parseUrl(char *l, char *url)
{
  return Parse::getParse()->parseUrl(l, url);
}

char * Object::parseName(char *l, char *name)
{
  return Parse::getParse()->parseName(l, name);
}

char * Object::parseChannel(char *l, char *chan)
{
  return Parse::getParse()->parseChannel(l, chan);
}

char * Object::parseColor(char *l, Pos &p)
{
  return Parse::getParse()->parseColor(l, p);
}

char * Object::parseInt(char *l, int *value, const char *attrkey)
{
  return Parse::getParse()->parseInt(l, value, attrkey);
}

char * Object::parseUInt8(char *l, uint8_t *value, const char *attrkey)
{
  return Parse::getParse()->parseUInt8(l, value, attrkey);
}

char * Object::parseUInt16(char *l, uint16_t *value, const char *attrkey)
{
  return Parse::getParse()->parseUInt16(l, value, attrkey);
}

char * Object::parseBool(char *l, bool *value, const char *attrkey)
{
  return Parse::getParse()->parseBool(l, value, attrkey);
}

char * Object::parseFloat(char *l, float *value, const char *attrkey)
{
  return Parse::getParse()->parseFloat(l, value, attrkey);
}

char * Object::parseVector3f(char *l, float *vector, const char *attrkey)
{
  return Parse::getParse()->parseVector3f(l, vector, attrkey);
}

char * Object::parseVector3fv(char *l, V3 *vector, const char *attrkey)
{
  return Parse::getParse()->parseVector3fv(l, vector, attrkey);
}

char * Object::parseString(char *l, char *str, const char *attrkey)
{
  return Parse::getParse()->parseString(l, str, attrkey);
}

char * Object::parseString(char *l, char *str)
{
  return Parse::getParse()->parseString(l, str);
}

char * Object::parseQuotedString(char *l, char *str, const char *attrkey)
{
  return Parse::getParse()->parseQuotedString(l, str, attrkey);
}

char * Object::parseQuotedString(char *l, char *str)
{
  return Parse::getParse()->parseQuotedString(l, str);
}

char * Object::parseCaption(char *l, char *str, const char *attrkey)
{
  return Parse::getParse()->parseCaption(l, str, attrkey);
}

char * Object::parseRotation(char *l, Pos &p)
{
  return Parse::getParse()->parseRotation(l, p);
}

char * Object::parseTranslation(char *l, Pos &p)
{
  return Parse::getParse()->parseTranslation(l, p);
}

char * Object::parseGuide(char *l, float path[][5], uint8_t *segs)
{
  return Parse::getParse()->parseGuide(l, &path[1], segs);
}

/** Tokenizes the line to parse */
char * Object::tokenize(char *l)
{
  Parse *parser = parse();

  char tag[Parse::TAG_LEN +2];
  if (*parser->tag) {
    // ignore close tag
    sprintf(tag, "</%s", parser->tag);
    char *p = strstr(l, tag);	// </type>
    if (p) *p = '\0';
  }

  // save solid string into geomsolid for VSql purposes
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
  return strtok(l, TOK_SEP);
}

////////////////
//
// Lists
//

/** Deletes a pointer of this object in an olist */
void Object::delFromList(list<Object*> &olist)
{
  for (std::list<Object*>::iterator it = olist.begin(); it != olist.end(); ++it) {
    if (*it == this) {
      olist.remove(*it);
    }
  }
  return;
}

/** called by addToListOnce */
OList * Object::addToList(OList *olist)
{
  if (! isValid()) return olist;

  OList *ol = new OList();
  ol->pobject = this;
  ol->next = olist;
  return ol;
}

/** called by addListToList */
OList * Object::addOListOnce(OList *olist)
{
  for (OList *ol = olist; ol ; ol = ol->next) {
    if (ol->pobject && ol->pobject == this) {
      return olist;		// already in the list
    }
  }
  return addToList(olist);	// add it into the list
}

OList * Object::delOList(OList *olist)
{
  OList *front = olist, *ol = NULL;

  if (! olist) {
    error("delOList: %s:%s NULL olist", typeName(), objectName());
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

/** Gets wobject by its number - static */
Object * Object::byNum(uint16_t num)
{
  for (std::vector<Object*>::iterator it = objectList.begin(); it != objectList.end(); ++it) {
    if ((*it)->num == num) return *it;
  }
  return NULL;
}

/** Concatenates (with test of ispointed & object) pointers list on an object */
OList * Object::addListToList(OList *l1, OList *l2)
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
    if (l1->pobject && !(l1->pobject->pointed) && (l1->pobject != this)) {
      l1->pobject->pointed = true;
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
/** Shows an object - static */
void Object::show(const char *name)
{
  for (std::list<Object*>::iterator it = objectList.begin(); it != objectList.end(); ++it) {
    if (! strcmp((*it)->name.current, name)) {
      trace1(DBG_FORCE, "%s p=%.2f,%.2f,%.2f it=%.2f,%.2f c=%.2f,%.2f,%.2f s=%.2f,%.2f,%.2f",
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

////////////////
//
// Network
//

/** Publishes position to the network */
bool Object::publishPos(const Pos &oldpos, int propxy, int propz, int propaz, int propax, int propay)
{
  bool change = false;

  if ((pos.x != oldpos.x) || (pos.y != oldpos.y)) {
    netop->declareDelta(propxy);
    change = true;
  }
  if (ABSF(pos.z - oldpos.z) > 0.1) {
    netop->declareDelta(propz);
    change = true;
  }
  if (pos.az != oldpos.az) {
    netop->declareDelta(propaz);
    change = true;
  }
  if (pos.ax != oldpos.ax) {
    netop->declareDelta(propax);
    change = true;
  }
  if (pos.ay != oldpos.ay) {
    netop->declareDelta(propay);
    change = true;
  }
  return change;
}

//
// get_ functions
//

void Object::get_xy(Object *po, Payload *pp)
{
  Pos oldpos = po->pos;
  pp->getPayload("ff", &po->pos.x, &po->pos.y);
  po->updatePositionAndGrid(oldpos);
}

void Object::get_z(Object *po, Payload *pp)
{
  Pos oldpos = po->pos;
  pp->getPayload("f", &po->pos.z);
  po->updatePositionAndGrid(oldpos);
}

void Object::get_az(Object *po, Payload *pp)
{
  Pos oldpos = po->pos;
  pp->getPayload("f", &po->pos.az);
  po->updatePositionAndGrid(oldpos);
}

void Object::get_ay(Object *po, Payload *pp)
{
  Pos oldpos = po->pos;
  pp->getPayload("f", &po->pos.ay);
  po->updatePositionAndGrid(oldpos);
}

void Object::get_ax(Object *po, Payload *pp)
{
  Pos oldpos = po->pos;
  pp->getPayload("f", &po->pos.ax);
  po->updatePositionAndGrid(oldpos);
}

void Object::get_hname(Object *po, Payload *pp)
{
  pp->getPayload("s", po->typeName());
}

//
// put_ functions
//

void Object::put_xy(Object *po, Payload *pp)
{
  pp->putPayload("ff", po->pos.x, po->pos.y);
}

void Object::put_z(Object *po, Payload *pp)
{
  pp->putPayload("f", po->pos.z);
}

void Object::put_az(Object *po, Payload *pp)
{
  pp->putPayload("f", po->pos.az);
}

void Object::put_ay(Object *po, Payload *pp)
{
  pp->putPayload("f", po->pos.ay);
}

void Object::put_ax(Object *po, Payload *pp)
{
  pp->putPayload("f", po->pos.ax);
}

void Object::put_hname(Object *po, Payload *pp)
{
  pp->putPayload("s", po->typeName());
}

/** Gets property from Network */
void Object::getProperty(uint8_t prop_id, Payload *pp)
{
  if (! isGetPropFunc(type, prop_id)) {
    error("getProperty: prop=%d doesn't match the object type=%d", prop_id, type);
    return;
  }
  runGetPropFunc(type, prop_id, this, pp);
}

/** Puts property to Network */
void Object::putProperty(uint8_t prop_id, Payload *pp)
{
  if (! isPutPropFunc(type, prop_id)) {
    //if (type != USER_TYPE)
      error("putProperty: prop=%d undefined for object=%d", prop_id, type);
    return;
  }
  runPutPropFunc(type, prop_id, this, pp);
}

/** Deletes a replicated object */
void Object::deleteReplica()
{
  if (this != localuser) {
    if (type == USER_TYPE) ::g.gui.removeAvatar((User *) this);
    delGrid();
    delFromList(mobileList);

    // delete Solids
    std::list<Solid*> solList = ::g.render.getSolidList();
    for (std::list<Solid*>::iterator s = solList.begin(); s != solList.end(); s++) {
      delete (*s);
    }
    solList.clear();

    if (netop) delete netop;
    netop = NULL;
  }
  else
    echo("%s disapeared, but he is back!", objectName());
}
