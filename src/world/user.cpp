//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
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
// user.cpp
//
// Local User management (localuser)
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "user.hpp"
#include "matvec.hpp"   // V3 M4
#include "world.hpp"	// getName
#include "ball.hpp"	// BALL_TYPE
#include "dart.hpp"	// DART_TYPE
#include "bullet.hpp"	// BULLET_TYPE
#include "step.hpp"	// STEP_TYPE
#include "move.hpp"	// GRAVITY
#include "walls.hpp"	// intersect
#include "sql.hpp"	// getCountCart
#include "carrier.hpp"	// Carrier
#include "cart.hpp"	// Cart
#include "bubble.hpp"	// Bubble
#include "netobj.hpp"	// NetObj
#include "payload.hpp"	// Payload
#include "solid.hpp"	// solid ray_dlist
#include "draw.hpp"	// ray
#include "render.hpp"	// sharedRender getViewMode
#include "gui.hpp"	// addAvatar
#include "sound.hpp"	// playSound
#include "rtp.hpp"	// RTPNAME_LEN
#include "pref.hpp"	// user
#include "color.hpp"	// Color
#include "humanoid.hpp"	// Humanoid
#include "human.hpp"	// Human
#include "guy.hpp"	// Guy
#include "head.hpp"	// Head
#include "entry.hpp"	// query
#include "halo.hpp"	// Halo
#include "hat.hpp"	// Hat
#include "dress.hpp"	// Dress
#include "wings.hpp"	// Wings


const OClass User::oclass(USER_TYPE, "User", NULL, User::replicator);

const float User::LASTING = 0.015;	// 15 ms
const float User::DEF_WIDTH = 0.26;	// width = 26 cm
const float User::DEF_DEPTH = 0.16;	// depth = 16 cm
const float User::DEF_HEIGHT = 1.70;	// height= 170 cm
const float User::LSPEED = 2.;		// linear speed: 2 m/s
const float User::ASPEED = 1.;		// angular speed
const float User::DELTAZ = 0.02;	// 2 cm
const float User::GOTHROUGH = 0.05;	// 5 cm
const float User::FOVY = 26.;		// 26 deg
const float User::NEAR = 0.1;		// clip near: orig=30cm
const float User::FAR = 300.;		// clip far: 300m
const float User::DENSITY = 1.;

// global
User *localuser = NULL;

// local
static float fovy = User::FOVY;
static float near = User::NEAR;
static bool pause_gravity = false;	// with gravity by default


/** Sets defaults */
void User::defaults()
{
  width = DEF_WIDTH;
  depth = DEF_DEPTH;
  height = DEF_HEIGHT;
  if (::g.pref.tview)
    current_view = Render::VIEW_THIRD_PERSON_FAR;
  else
    current_view = Render::VIEW_FIRST_PERSON;
  ray = setV3(0, 0, 0);
}

/** Sets name */
void User::setName()
{
  if (name.given && ! *name.given) {
    name.given = new char[USER_LEN];		// alloc given.name
    sprintf(name.given, "%s", ::g.user);
    name.current = name.given;
  }
}

/** Resets position */
void User::resetPosition()
{
  pos.x = 0;
  pos.y = 0;
  pos.z = height/2;
  pos.az = 0;
  pos.ax = 0;
  pos.ay = 0;
}

/** Sets position */
void User::setPosition()
{
  resetPosition();
  Entry *entry = Entry::current();
  if (entry) {
    entry->query(this);
    trace1(DBG_INIT, "new entry: %.1f %.1f %.1f", pos.x, pos.y, pos.z);
    //echo("entry: %.1f %.1f %.1f", pos.x, pos.y, pos.z);
  } 
}

/** Checks if localuser position is out of bounds - returns true if true else returns false */
bool User::checkPosition()
{
  if ( ABSF(pos.x)>999 || ABSF(pos.y)>999 || ABSF(pos.z)>999 ) {
    echo("localuser->pos: %.1f %.1f %.1f", pos.x, pos.y, pos.z);
    echo("force localuser pos(0 0 2)");
    resetPosition();
    return true;
  }
  return false;
}

/** Sets view mode */
void User::setView(uint8_t view)
{
  ::g.render.setViewMode(view);
  current_view = view;

  switch (view) {
    case Render::VIEW_THIRD_PERSON :
    case Render::VIEW_THIRD_PERSON_FAR :
    case Render::VIEW_THIRD_PERSON_FRONT :
    case Render::VIEW_VERTICAL_NEAR :
    case Render::VIEW_VERTICAL_FAR :
    case Render::VIEW_VERTICAL_FROM_SKY :
    case Render::VIEW_TURN_AROUND :
      disableBehavior(UNVISIBLE);	// visible by myself
      if (head) head->visibility(true);
      break;
    case Render::VIEW_FIRST_PERSON :
    default :
      enableBehavior(UNVISIBLE);	// not visible by myself
      if (head) head->visibility(false);
      break;
  }
}

/** Builds avatar's geometry */
void User::geometry()
{
  char s[256];

  memset(s, 0, sizeof(s));
  memset(mensuration, 0, sizeof(mensuration));

  // fill mapping pref fields
  Pref* pref = &::g.pref;
  
  if (pref->my_widthstr)	width = atof(pref->my_widthstr);
  if (pref->my_depthstr)	depth = atof(pref->my_depthstr);
  if (pref->my_heightstr)	height = atof(pref->my_heightstr);

  if (pref->my_skinf) {
    if (pref->my_mapfrontstr)	front = strdup(pref->my_mapfrontstr);
    else			front = strdup(pref->my_skinf);
  }
  if (pref->my_skinb) {
    if (pref->my_mapbackstr)	back = strdup(pref->my_mapbackstr);
    else			back = strdup(pref->my_skinb);
  }
  left = strdup(" "); 		*left = 0;
  right = strdup(" "); 		*right = 0;
  if (pref->my_vrestr)		vre = strdup(pref->my_vrestr);
  else				{ vre = strdup(" "); *vre = 0; }
  if (pref->my_webstr)		web = strdup(pref->my_webstr);
  else				{ web = strdup(" "); *web = 0; }
  if (pref->my_avatar)		avatar = strdup(pref->my_avatar);
  else				{ avatar = strdup(" "); *avatar = 0; }
  if (pref->my_facestr)		uface = strdup(pref->my_facestr);
  else				{ uface = strdup(" "); *uface = 0; }
  if (pref->my_sexstr)		sex = strdup(pref->my_sexstr);
  else				{ sex = strdup(" "); *sex = 0; }
  if (pref->my_headstr)		headurl = strdup(pref->my_headstr);
  else				{ headurl = strdup(" "); *headurl = 0; }
  if (pref->my_skinstr)		skin = strdup(pref->my_skinstr);
  else				{ skin = strdup(" "); *skin = 0; }
  if (pref->my_buststr)		bust = strdup(pref->my_buststr);
  else				{ bust = strdup(" "); *bust = 0; }
  if (pref->my_colorstr)	color = strdup(pref->my_colorstr);
  else				{ color = strdup(" "); *color = 0; }
  if (pref->my_bapsstr)		baps = strdup(pref->my_bapsstr);

  if (isalpha(*avatar)) {	// avatar is defined in ~./.vreng/prefs
    // 5 available avatars: guy, human, humanoid, box, bbox
    if (! strcmp(avatar, "guy")) {
      guy = new Guy();
      sprintf(mensuration, "shape=\"guy\" dim=\"%f %f %f\"", width, depth, height);
    }
    else if (! strcmp(avatar, "humanoid")) {
      humanoid = new Humanoid();
      sprintf(mensuration, "shape=\"guy\" dim=\"%f %f %f\"", width, depth, height);
      pos.az -= M_PI_2;
      humanoid->pos.az -= M_PI_2;
      humanoid->pause();	//dax ??? OK WHY ???
      updatePos();
    }
    else if (! strcmp(avatar, "human")) {
      human = new Human();
      sprintf(mensuration, "shape=\"human\" dim=\"%f %f %f\"", width, depth, height);
      enableBehavior(SPECIFIC_RENDER);
    }
    else if (! strcmp(avatar, "box")) {
      sprintf(mensuration, "shape=\"box\" dim=\"%f %f %f\" yp=\"%s\" xp=\"%s\"",
                            depth, width, height, front, back);
    }
    else if (! strcmp(avatar, "bbox")) {
      sprintf(mensuration, "shape=\"bbox\" dim=\"%f %f %f\"", width, depth, height);
    }
    else {
      guy = new Guy();
      sprintf(mensuration, "shape=\"guy\" dim=\"%f %f %f\"", width, depth, height);
    }
  }
  else {	// take the default
    guy = new Guy();
    sprintf(mensuration, "shape=\"guy\" dim=\"%f %f %f\"", width, depth, height);
  }
  sprintf(s, "%s />", mensuration);
  trace1(DBG_WO, "User: s=%s", s);
  parseSolid(s);

  // avatar's head
  if (*headurl) {
    float color[3];
    Color::getRGB(skin, color);
    head = new Head(this, headurl, color);
  }

  if (guy) guy->setGivenName(::g.user);
  else if (humanoid) humanoid->setGivenName(::g.user);
  else if (human) human->setGivenName(::g.user);
}

/** Sets network identity */
void User::setRtcp()
{
  rtcpname = new char[Rtp::RTPNAME_LEN];
  email    = new char[Rtp::EMAIL_LEN];
  tool     = new char[Rtp::TOOL_LEN];
  Rtp::getRtcpName(rtcpname);
  Rtp::getRtcpEmail(email);
  Rtp::getRtcpTool(tool);
  ssrc = NetObj::getSsrc();
}

/** Sets observer view from user's eyes */
void User::setCamera()
{
  ::g.render.cameraProjection(FOVY, NEAR, FAR);
}

/** Informs the Gui */
void User::addGui()
{
  if (! guip) guip = ::g.gui.addAvatar(this);
}

/** Checks attached persist objects */
void User::checkPersist()
{
  int nitem;
  char pat[128], qname[128];

  VSql *vsql = new VSql();     // first get the the VSql handle;

  nitem = vsql->getCount(HALO_NAME);  // halos in VSql
  if (nitem) {
    sprintf(pat, "&%s", objectName());
    if (vsql->getName(HALO_NAME, pat, 0, qname) >= 0)
      doAction(HALO_TYPE, Halo::RECREATE, (User*)this, (void*)qname,0,0);
  }
  nitem = vsql->getCount(HAT_NAME);   // hats in VSql
  if (nitem) {
    sprintf(pat, "&%s", objectName());
    if (vsql->getName(HAT_NAME, pat, 0, qname) >= 0)
      doAction(HAT_TYPE, Hat::RECREATE, (User*)this, (void*)qname,0,0);
  }
  nitem = vsql->getCount(DRESS_NAME); // dresses in VSql
  if (nitem) {
    sprintf(pat, "&%s", objectName());
    if (vsql->getName(DRESS_NAME, pat, 0, qname) >= 0)
      doAction(DRESS_TYPE, Dress::RECREATE, (User*)this, (void*)qname,0,0);
  }
  nitem = vsql->getCount(WINGS_NAME); // wings in VSql
  if (nitem) {
    sprintf(pat, "&%s", objectName());
    if (vsql->getName(WINGS_NAME, pat, 0, qname) >= 0)
      doAction(WINGS_TYPE, Wings::RECREATE, (User*)this, (void*)qname,0,0);
  }
#if 0 //todo
  int cartnum = vsql->getCountCart();
  //TODO: get the rows
  // addObjectToCart
#endif //todo
}

/** Initialization of User */
void User::inits()
{
  setName();
  updateNames();
  mobileObject(LASTING);
  permanentMovement();	// gravity
  netop = createNetObj(PROPS);
  // netop->declareCreation(); // we don't need because delta do the job

  geometry();
  setPosition();	// position from entry
  updatePos();
  setCamera();
  if (current_view)
    setView(current_view);
  else
    setView(Render::VIEW_FIRST_PERSON);
  setVisible(true);
  setRtcp();		// network identity
  clearKeys();		// reset keys for movement
  addGui();		// informs GUI

  // attach carrier & cart
  carrier = new Carrier();
  char *basket = new char[32];
  strcpy(basket, "name=\"basket\"");
  //dax cart = new Cart(basket);

  checkPersist();	 // checks Persist objects
}

/** Creates localuser */
User::User()
{
  localuser = this;	// global
  carrier = NULL;
  cart = NULL;
  bubble = NULL;
  human = NULL;
  humanoid = NULL;
  guy = NULL;
  head = NULL;
  hit = 0;

  defaults();
  move.lspeed.v[0] = move.lspeed.v[1] = move.lspeed.v[2] = 0;
  move.aspeed.v[0] = move.aspeed.v[1] = move.aspeed.v[2] = 0;
  pos.az = 0;
  inits();
}

/** Replicates an user from the network */
Object * User::replicator(uint8_t type_id, Noid _noid, Payload *pp)
{
  return new User(type_id, _noid, pp);
}

/** Constructor of an User coming from the network */
User::User(uint8_t type_id, Noid _noid, Payload *pp)
{
  type = type_id;
  carrier = NULL;
  cart = NULL;
  bubble = NULL;
  human = NULL;
  humanoid = NULL;
  guy = NULL;
  head = NULL;
  defaults();
  getMemory();		// alloc geometries

  netop = replicate(PROPS, _noid);

  // hack to retrieve the name and the mapping
  netop->getProperty(/*  0 */ PROPHNAME, pp);
  netop->getProperty(/*  1 */ PROPMAPFRONT, pp);
  netop->getProperty(/*  2 */ PROPMAPBACK, pp);

  int idxgeom, idxend = 0;
  char s[256];
  memset(s, 0, sizeof(s));

  int idxvar = pp->tellPayload();	// note begin of var
  //echo("idxvar=%d[%02x]", idxvar, idxvar);
  if (((idxgeom = pp->tellPayload("shape=\"box\" dim=")) > 0) ||
      ((idxgeom = pp->tellPayload("shape=\"human\" dim=")) > 0) ||
      ((idxgeom = pp->tellPayload("shape=\"humanoid\" dim=")) > 0) ||
      ((idxgeom = pp->tellPayload("shape=\"guy\" dim=")) > 0)
     ) {
    // get replicated user characteristics from the network
    //echo("idxgeom=%d[%02x]", idxgeom, idxgeom);
    netop->getProperty(/* 09 */ PROPMENSURATION, pp);
    netop->getProperty(/* 10 */ PROPMAPLEFT, pp);
    netop->getProperty(/* 11 */ PROPMAPRIGHT, pp);
    netop->getProperty(/* 12 */ PROPVRE, pp);
    netop->getProperty(/* 13 */ PROPWEB, pp);
    netop->getProperty(/* 14 */ PROPSSRC, pp);
    netop->getProperty(/* 15 */ PROPRTCPNAME, pp);
    netop->getProperty(/* 16 */ PROPRTCPEMAIL, pp);
    netop->getProperty(/* 17 */ PROPMODEL, pp);
    netop->getProperty(/* 18 */ PROPFACE, pp);
    netop->getProperty(/* 19 */ PROPSEX, pp);
    netop->getProperty(/* 20 */ PROPHEAD, pp);
    netop->getProperty(/* 21 */ PROPSKIN, pp);
    netop->getProperty(/* 22 */ PROPBUST, pp);
    netop->getProperty(/* 23 */ PROPRAY, pp);
    idxend = pp->tellPayload();	// note end of properties

    if (isalpha(*avatar))
      sprintf(s, "%s />", mensuration);
    else
      sprintf(s, "%s yp=\"%s\" xp=\"%s\" />", mensuration, front, back);
  }
  else {
    // builtin user solid
    sprintf(s, "dim=\"%f %f %f\" xp=\"%s\" xn=\"%s\" />",
            DEF_WIDTH, DEF_DEPTH, DEF_HEIGHT, front, back);
  }
  //echo("replica: s=%s", s);
  parseSolid(s);

  // get the variable properties
  if (idxend > 0) {
    //echo("replica: read var props, idxend=%d", idxend);
    pp->seekPayload(idxvar);	// begin prop var
    for (int np = PROPBEGINVAR; np <= PROPENDVAR; np++) {
      netop->getProperty(np, pp);
    }
    pp->seekPayload(idxend);	// end properties
  }
  else {	// never executed
    error("replica: never executed idxend=%d", idxend);
    uint8_t _nbprop = netop->getProperties();
    for (int np = PROPBEGINVAR; np < _nbprop; np++) {
      netop->getProperty(np, pp);
    }
  }
  //echo("replica: new avatar: rctpname=%s", rtcpname);
  mobileObject(0);
  addGui();			// informs Gui
  ::g.gui.expandAvatar();	// shows new avatar coming in
  //echo("replica: name=%s ssrc=%x rtcpname=%s email=%s", objectName(), ssrc, rtcpname, email);
}

/** Allocates memory for User's members */
void User::getMemory()
{
  front    = new char[URL_LEN];
  back     = new char[URL_LEN];
  left     = new char[URL_LEN];
  right    = new char[URL_LEN];
  vre      = new char[URL_LEN];
  web      = new char[URL_LEN];
  avatar   = new char[URL_LEN];
  uface    = new char[URL_LEN];
  headurl  = new char[URL_LEN];
  sex      = new char[8];
  skin     = new char[8];
  bust     = new char[8];
  color    = new char[8];
  baps     = new char[URL_LEN];
  email    = new char[Rtp::EMAIL_LEN];
  tool     = new char[Rtp::TOOL_LEN];
  rtcpname = new char[Rtp::RTPNAME_LEN];
}

/** Quits - destructor */
User::~User()
{
  echo("User %s quits", objectName());
  ::g.gui.removeAvatar(this);

  // MS. if this destructor is called for a remote user,
  // we should not declare the deletion: it's not our problem.
  if (this == localuser && netop) {
    netop->declareDeletion();
    delete netop;		// delete NetObj
    netop = NULL;
  }
  if (vsql)    delete vsql;
  if (front)   delete front;
  if (back)    delete back;
  if (left)    delete left;
  if (right)   delete right;
  if (vre)     delete vre;
  if (web)     delete web;
  if (avatar)  delete avatar;
  if (uface)   delete uface;
  if (sex)     delete sex;
  if (headurl) delete headurl;
  if (skin)    delete skin;
  if (bust)    delete bust;
  if (color)   delete color;
  //BAD free if (baps)  delete baps;
}

/** Updates local user towards the network */
bool User::publish(const Pos &opos)
{
  bool change = false;

  if ((pos.x != opos.x) || (pos.y != opos.y)) {
    netop->declareDelta(PROPXY); change = true;
  }
  if (ABSF(pos.z - opos.z) > DELTAZ) { // if d < 2cm => not sent
    netop->declareDelta(PROPZ); change = true;
  }
  if (pos.az != opos.az) {
    netop->declareDelta(PROPAZ); change = true;
  }
  return change;
}

/** Updates times array */
void User::timing(time_t sec, time_t usec, float *dt)
{
  lasting(sec, usec, dt);
}

/** Informs a message sent by localuser */
void User::userWriting(const char *usermsg)
{
  if (strlen(usermsg) < MESS_LEN) {
    strcpy(localuser->message, usermsg);
  }
  else {
    strncpy(localuser->message, usermsg, MESS_LEN-1);
    localuser->message[MESS_LEN-1] = '\0';
  }
  localuser->lastmess++;
  localuser->netop->declareDelta(PROPMSG); // msg property
  if (localuser->bubble) {
    localuser->bubble->toDelete();	// delete previous bubble
  }
  localuser->bubble = new Bubble(localuser, localuser->message, Color::black, Bubble::BUBBLEVERSO);
  //dax localuser->bubble->setPos(pos.x, pos.y+lenText(localuser->message /2, pos.z+0.2, pos.az, pos.ax);
  Sound::playSound(KEYBOARDSND);
}

void User::userRequesting(const char *usermsg)
{
  if (strlen(usermsg) < MESS_LEN) {
    strcpy(localuser->request, usermsg);
  }
  else {
    strncpy(localuser->request, usermsg, MESS_LEN-1);
    localuser->request[MESS_LEN-1] = '\0';
  }
  localuser->lastmess++;
  localuser->netop->declareDelta(PROPMSG); // msg property
}

/** Imposed movement via keys or navig_menu or joystick */
void User::imposed(float dt)
{
  pos.x += dt * move.lspeed.v[0];
  pos.y += dt * move.lspeed.v[1];
  pos.z += dt * move.lspeed.v[2];
  pos.az += dt * move.aspeed.v[0];
  updatePos();
  if (localuser->human) {
    localuser->human->pos = pos;
    localuser->human->updatePos();
  }
  if (localuser->humanoid) {
    localuser->humanoid->pos = pos;
    localuser->humanoid->updatePos();
  }
  if (localuser->bubble) {
    localuser->bubble->setPosition();
    localuser->bubble->updatePos();
  }
}

/** Equations system handling permanent motions */
void User::permanent(float dt)
{
  if (! pause_gravity) {
    pos.z -= dt * GRAVITY;
  }
}

void User::bulletPutHit(Bullet *pbullet, Payload *pp)
{
  pp->putPayload("c", pbullet->hit);
}

void User::dartPutHit(Dart *pdart, Payload *pp)
{
  pp->putPayload("c", pdart->hit);
}

/** Intersection with an other object */
bool User::intersect(Object *pcur, Object *pold)
{
  // User has no control over ghost objects
  if (pcur->isBehavior(COLLIDE_GHOST)) return true;

  switch (pcur->type) {
  case BULLET_TYPE:
  case DART_TYPE:
    // projectile intersects user: hit
    if (hit == 0) {
      hit = 1;
      echo("%s:%s hits %s", pcur->typeName(), pcur->objectName(), objectName());
      if (pcur->type == DART_TYPE) {
        (reinterpret_cast<Dart *>(pcur)->hit) = 1;
        netop->sendDelta(Dart::PROPHIT);
        (reinterpret_cast<Dart *>(pcur)->hit) = 0;
        Sound::playSound(OUILLESND);
      }
      else if (pcur->type == BULLET_TYPE) {
        (reinterpret_cast<Bullet *>(pcur)->hit) = 1;
        netop->sendDelta(Bullet::PROPHIT);
        (reinterpret_cast<Bullet *>(pcur)->hit) = 0;
        Sound::playSound(RUPSSND);
      }
    }
    pcur->pos = pold->pos;
    pcur->toDelete();	// delete projectile
    break;
  case BALL_TYPE:
    pcur->pos.x += Ball::SHIFT;	// ball shifts
    pcur->pos.y += Ball::SHIFT;
    pcur->updateGrid(pold);
    break;
  case USER_TYPE:
    // user intersects an other user: slide
    pcur->pos.x += GOTHROUGH; // step to shift
    pcur->pos.y += GOTHROUGH;
    pcur->updateGrid(pold);
    break;
  case STEP_TYPE:
    return false;	//TODO
  default:
    pcur->updateGrid(pold);
  }
  return true;
}

/** Sends a ray */
void User::setRayDirection(GLint wx, GLint wy)
{
  const GLfloat *white = Color::white;

  GLfloat ex = pos.x + near; // +near
  GLfloat ey = pos.y;
  GLfloat ez = pos.z + height/2 - 0.10;		// top head - 10 cm
  GLint vp[4];

  glGetIntegerv(GL_VIEWPORT, vp);
  GLfloat tx = static_cast<GLfloat>(FAR);
  GLfloat ty = vp[2]/2 - static_cast<GLfloat>(wx);
  GLfloat tz = vp[3]/2 - static_cast<GLfloat>(wy);
  if (ty < 0) ty = MAX(ty, -FAR); else ty = MIN(ty, FAR);
  if (tz < 0) tz = MAX(tz, -FAR); else tz = MIN(tz, FAR);
 
  Draw::ray(&(getSolid()->ray_dlist), ex, ey, ez, tx, ty, tz, white, 0x3333);

  ray = setV3(tx, ty, tz);
  netop->declareDelta(User::PROPRAY); // publishes ray property to network
}

bool User::hasHead()
{
  return (bool) head;
}

/** Does a special action */
void User::specialAction(int action_id, void *d, time_t s, time_t u)
{
  Object *o = NULL;

  if (carrier && carrier->underControl()) {	// DON'T CHANGE THIS LINE ELSE localuser CAN'T MOVE
    o = carrier; 	// carrier
  }
  else {
    o = this;    	// user
  }
  if (isAction(o->type, action_id)) {
    doAction(o->type, action_id, o, d, s, u);
  }
}

void User::createBullet(User *user, void *d, time_t s, time_t u)
{
  if (isAction(BULLET_TYPE, Bullet::CREATE)) {
    doAction(BULLET_TYPE, Bullet::CREATE, user, d, s, u);
  }
}

void User::createDart(User *user, void *d, time_t s, time_t u)
{
  if (isAction(DART_TYPE, Dart::CREATE)) {
    doAction(DART_TYPE, Dart::CREATE, user, d, s, u);
  }
}

void User::defZoom(User *user, void *d, time_t s, time_t u)
{
  fovy = User::FOVY;
  near = User::NEAR;
  ::g.render.cameraProjection(fovy, near, User::FAR);
}

void User::incrZoom(User *user, void *d, time_t s, time_t u)
{
  fovy -= 1;
  if (fovy <= 1.0) fovy = 1;
  ::g.render.cameraProjection(fovy, near, User::FAR);
}

void User::decrZoom(User *user, void *d, time_t s, time_t u)
{
  fovy += 1;
  if (fovy >= 75.0) fovy = 75;
  ::g.render.cameraProjection(fovy, near, User::FAR);
}

void User::setZoom(User *user, void *d, time_t s, time_t u)
{
  int *fovy = static_cast<int*>(d);

  if (*fovy <= 0) *fovy = 1;
  if (*fovy >= 76) *fovy = 75;
  ::g.render.cameraProjection(*fovy, near, User::FAR);
}

void User::setRoll(User *user, void *d, time_t s, time_t u)
{
  float *angle = static_cast<float*>(d);
  user->pos.ax = DEG2RAD(*angle);
}

void User::incrRoll(User *user, void *d, time_t s, time_t u)
{
  user->pos.ax += M_PI/180;
  user->pos.z += 0.05;
  if (user->pos.ax >= M_PI_2) user->pos.ax = M_PI_2;
  user->updatePos();
}

void User::decrRoll(User *user, void *d, time_t s, time_t u)
{
  user->pos.ax -= M_PI/180;
  user->pos.z += 0.05;
  if (user->pos.ax <= -M_PI_2) user->pos.ax = -M_PI_2;
  user->updatePos();
}

void User::flyaway(User *user, void *d, time_t s, time_t u)
{
  pause_gravity = true;
  user->pos.ay -= M_PI/8;
  user->pos.z += 5;
  user->updatePos();
}

void User::toland(User *user, void *d, time_t s, time_t u)
{
  pause_gravity = false;
  if (::g.render.getViewMode() == Render::VIEW_FIRST_PERSON)
    user->pos.ay = 0;
  else
    user->pos.ay += M_PI/8;
  user->updatePos();
}

void User::setPitch(User *user, void *d, time_t s, time_t u)
{
  float *angle = static_cast<float*>(d);
  ::g.render.setPitch(-DEG2RAD(*angle));
}

void User::incrPitch(User *user, void *d, time_t s, time_t u)
{
  user->pos.ay -= M_PI/180;
  user->pos.z += 0.05;
  if (user->pos.ay <= -M_PI_2) user->pos.ay = -M_PI_2;
  user->updatePos();
}

void User::decrPitch(User *user, void *d, time_t s, time_t u)
{
  user->pos.ay += M_PI/180;
  user->pos.z += 0.05;
  if (user->pos.ay >= M_PI_2) user->pos.ay = M_PI_2;
  user->updatePos();
}

void User::defLinSpeed(User *user, void *d, time_t s, time_t u)
{
  user->lspeed = User::LSPEED;
}

void User::decrLinSpeed(User *user, void *d, time_t s, time_t u)
{
  if (user->lspeed > 1.0) user->lspeed -= 1;
}

void User::incrLinSpeed(User *user, void *d, time_t s, time_t u)
{
  user->lspeed += 1;
}

void User::setLspeed(User *user, void *d, time_t s, time_t u)
{
  float *ls = static_cast<float *>(d);
  user->lspeed = *ls;
}

void User::defAngSpeed(User *user, void *d, time_t s, time_t u)
{
  user->aspeed = User::ASPEED;
}

void User::decrAngSpeed(User *user, void *d, time_t s, time_t u)
{
  if (user->aspeed > 0.3) user->aspeed -= 0.2;
}

void User::incrAngSpeed(User *user, void *d, time_t s, time_t u)
{
  user->aspeed += 0.2;
}

void User::setAspeed(User *user, void *d, time_t s, time_t u)
{
  float *as = static_cast<float *>(d);
  user->aspeed = *as;
}

void User::mapView(User *user, void *d, time_t s, time_t u)
{
  ::g.render.switchViewMap();
}

void User::switchView(User *user, void *d, time_t s, time_t u)
{
  user->setView((::g.render.getViewMode() + 1) % Render::VIEW_NUMBER);
}

void User::firstView(User *user, void *d, time_t s, time_t u)
{
  user->setView(Render::VIEW_FIRST_PERSON);
}

void User::thirdView(User *user, void *d, time_t s, time_t u)
{
  user->setView(Render::VIEW_THIRD_PERSON_FAR);
}

void User::thirdView_RotL(User *user, void *d, time_t s, time_t u)
{
  if (::g.render.getViewMode() == Render::VIEW_THIRD_PERSON) {
    ::g.render.third_xRot += M_PI/18;
  }
}

void User::thirdView_RotR(User *user, void *d, time_t s, time_t u)
{
  if (::g.render.getViewMode() == Render::VIEW_THIRD_PERSON) {
    ::g.render.third_xRot -= M_PI/18;
  }
}

void User::thirdView_RotU(User *user, void *d, time_t s, time_t u)
{
  Render* render = &::g.render;
  if (render->getViewMode() == Render::VIEW_THIRD_PERSON) {
    if (render->third_xRot + M_PI_2/4 < M_PI_2) {
      render->third_yRot += M_PI/18;
    }
  }
}

void User::thirdView_RotD(User *user, void *d, time_t s, time_t u)
{
  Render* render = &::g.render;
  if (render->getViewMode() == Render::VIEW_THIRD_PERSON) {
    if (render->third_xRot + M_PI_2/4 > -M_PI_2/4) {
      render->third_yRot -= M_PI/18;
    }
  }
}

void User::thirdView_Near(User *user, void *d, time_t s, time_t u)
{
  Render* render = &::g.render;
  if (render->getViewMode() == Render::VIEW_THIRD_PERSON) {
    if (render->third_Near > -1.5) {
      render->third_Near -= 0.1;
    }
  }
}

void User::thirdView_Far(User *user, void *d, time_t s, time_t u)
{
  Render* render = &::g.render;
  if (render->getViewMode() == Render::VIEW_THIRD_PERSON) {
    render->third_Near += 0.1;
  }
}

void User::pause(User *user, void *d, time_t s, time_t u)
{
  pause_gravity ^= 1;
}

void User::pauseOn(User *user, void *d, time_t s, time_t u)
{
  pause_gravity = true;
}

void User::pauseOff(User *user, void *d, time_t s, time_t u)
{
  pause_gravity = false;
}

void User::enableGravity()
{
  pause_gravity = false;
}

void User::disableGravity()
{
  pause_gravity = true;
}

void User::get_username(User *pu, Payload *pp)
{
  pp->getPayload("s", pu->objectName());
  trace1(DBG_WO, "get_username=%s", pu->objectName());
}

void User::u_get_xy(Object *po, Payload *pp)
{
  get_xy(po, pp);
  if (po == localuser) {
    echo("Something's pushing me!");
    po->updateCamera(po->pos);
  }
}

void User::u_get_z(Object *po, Payload *pp)
{
  get_z(po, pp);
  if (po == localuser) {
    echo("Something's lifting me!");
    po->updateCamera(po->pos);
  }
}

void User::u_get_ax(Object *po, Payload *pp)
{
  get_ax(po, pp);
  if (po == localuser) {
    echo("Something's twisting me!");
    po->updateCamera(po->pos);
  }
}

void User::u_get_ay(Object *po, Payload *pp)
{
  get_ay(po, pp);
  if (po == localuser) {
    echo("Something's twisting me!");
    po->updateCamera(po->pos);
  }
}

void User::u_get_az(Object *po, Payload *pp)
{
  get_az(po, pp);
  if (po == localuser) {
    echo("Something's twisting me!");
    po->updateCamera(po->pos);
  }
}

void User::get_msg(User *pu, Payload *pp)
{
  if (pu) {
    int mess;
    char msg[User::MESS_LEN];
    float blue[] = {0,0,1};

    memset(msg, 0, sizeof(msg));
    pp->getPayload("ds", &mess, msg);
    if (strlen(msg) && mess != pu->lastmess) {
      ::g.gui.writeMessage("chat", pu->objectName(), msg);
      pu->lastmess = mess;
      pu->bubble = localuser->bubble;
      if (pu->bubble)
        pu->bubble->toDelete();	// delete previous bubble
      pu->bubble = new Bubble(pu, msg, blue, Bubble::BUBBLERECTO);
    }
  }
}

void User::get_infos(User *pu, Payload *pp)
{ if (pu) pp->getPayload("s", pu->name.infos); }

void User::get_mensuration(User *pu, Payload *pp)
{ if (pu) pp->getPayload("s", pu->mensuration); }

void User::get_mapfront(User *pu, Payload *pp)
{ if (pu) pp->getPayload("s", pu->front); }

void User::get_mapback(User *pu, Payload *pp)
{ if (pu) pp->getPayload("s", pu->back); }

void User::get_mapleft(User *pu, Payload *pp)
{ if (pu) pp->getPayload("s", pu->left); }

void User::get_mapright(User *pu, Payload *pp)
{ if (pu) pp->getPayload("s", pu->right); }

void User::get_vre(User *pu, Payload *pp)
{ if (pu) pp->getPayload("s", pu->vre); }

void User::get_web(User *pu, Payload *pp)
{ if (pu) pp->getPayload("s", pu->web); }

void User::get_avatar(User *pu, Payload *pp)
{ if (pu) pp->getPayload("s", pu->avatar); }

void User::get_uface(User *pu, Payload *pp)
{ if (pu) pp->getPayload("s", pu->uface); }

void User::get_sex(User *pu, Payload *pp)
{ if (pu) pp->getPayload("s", pu->sex); }

void User::get_head(User *pu, Payload *pp)
{ if (pu) pp->getPayload("s", pu->head); }

void User::get_skin(User *pu, Payload *pp)
{ if (pu) pp->getPayload("s", pu->skin); }

void User::get_bust(User *pu, Payload *pp)
{ if (pu) pp->getPayload("s", pu->bust); }

void User::get_color(User *pu, Payload *pp)
{ if (pu) pp->getPayload("s", pu->color); }

void User::get_ssrc(User *pu, Payload *pp)
{ if (pu) pp->getPayload("d", &(pu->ssrc)); }

void User::get_rtcpname(User *pu, Payload *pp)
{ if (pu) pp->getPayload("s", pu->rtcpname); }

void User::get_rtcpemail(User *pu, Payload *pp)
{ if (pu) pp->getPayload("s", pu->email); }

void User::get_ray(User *pu, Payload *pp)
{
  if (pu) {
    float ex, ey, ez, tx, ty, tz;
    pp->getPayload("ffffff", &ex, &ey, &ez, &tx, &ty, &tz);
    if (pu->getSolid()) {
      if (tx) {
        float blue[4] = {0,0,1,1}; //blue
        //echo("get_ray: %.1f,%.1f,%.1f -> %.1f,%.1f,%.1f", ex, ey, ez, tx, ty, tz);
        Draw::ray(&pu->getSolid()->ray_dlist, ex,ey,ez, tx,ty,tz, blue, 0);
      }
      else {
        pu->getSolid()->ray_dlist = 0;
        pu->ray = setV3(0, 0, 0);
      }
    }
  }
}

void User::put_username(User *pu, Payload *pp)
{ if (pu) pp->putPayload("s", pu->objectName()); }

void User::put_msg(User *pu, Payload *pp)
{ if (pu) pp->putPayload("ds", pu->lastmess, pu->message); }

void User::put_infos(User *pu, Payload *pp)
{ if (pu) pp->putPayload("s", pu->name.infos); }

void User::put_mensuration(User *pu, Payload *pp)
{ if (pu) pp->putPayload("s", pu->mensuration); }

void User::put_mapfront(User *pu, Payload *pp)
{ if (pu) pp->putPayload("s", pu->front); }

void User::put_mapback(User *pu, Payload *pp)
{ if (pu) pp->putPayload("s", pu->back); }

void User::put_mapleft(User *pu, Payload *pp)
{ if (pu) pp->putPayload("s", pu->left); }

void User::put_mapright(User *pu, Payload *pp)
{ if (pu) pp->putPayload("s", pu->right); }

void User::put_vre(User *pu, Payload *pp)
{ if (pu) pp->putPayload("s", pu->vre); }

void User::put_web(User *pu, Payload *pp)
{ if (pu) pp->putPayload("s", pu->web); }

void User::put_avatar(User *pu, Payload *pp)
{ if (pu) pp->putPayload("s", pu->avatar); }

void User::put_uface(User *pu, Payload *pp)
{ if (pu) pp->putPayload("s", pu->uface); }

void User::put_sex(User *pu, Payload *pp)
{ if (pu) pp->putPayload("s", pu->sex); }

void User::put_head(User *pu, Payload *pp)
{ if (pu) pp->putPayload("s", pu->head); }

void User::put_skin(User *pu, Payload *pp)
{ if (pu) pp->putPayload("s", pu->skin); }

void User::put_bust(User *pu, Payload *pp)
{ if (pu) pp->putPayload("s", pu->bust); }

void User::put_color(User *pu, Payload *pp)
{ if (pu) pp->putPayload("s", pu->color); }

void User::put_ssrc(User *pu, Payload *pp)
{ if (pu) pp->putPayload("d", pu->ssrc); }

void User::put_rtcpname(User *pu, Payload *pp)
{ if (pu) pp->putPayload("s", pu->rtcpname); }

void User::put_rtcpemail(User *pu, Payload *pp)
{ if (pu) pp->putPayload("s", pu->email); }

void User::put_ray(User *pu, Payload *pp)
{
  if (pu) {
    pp->putPayload("ffffff", pu->pos.x, pu->pos.y, pu->pos.z + pu->height/2 - 0.10, pu->ray.v[0], pu->ray.v[1], pu->ray.v[2]);
    if (pu->ray.v[0]) {
      //echo("put_ray: %.1f,%.1f,%.1f -> %.1f,%.1f,%.1f", pu->pos.x, pu->pos.y, pu->pos.z + pu->height/2 - 0.10, pu->ray.v[0], pu->ray.v[1], pu->ray.v[2]);
    }
  }
}

/** user functions initialization */
void User::funcs()
{
  getPropFunc(USER_TYPE, PROPHNAME, _Payload get_username);
  getPropFunc(USER_TYPE, PROPMENSURATION, _Payload get_mensuration);
  getPropFunc(USER_TYPE, PROPMAPFRONT, _Payload get_mapfront);
  getPropFunc(USER_TYPE, PROPMAPBACK, _Payload get_mapback);
  getPropFunc(USER_TYPE, PROPMAPLEFT, _Payload get_mapleft);
  getPropFunc(USER_TYPE, PROPMAPRIGHT, _Payload get_mapright);
  getPropFunc(USER_TYPE, PROPXY, _Payload u_get_xy);
  getPropFunc(USER_TYPE, PROPZ, _Payload u_get_z);
  getPropFunc(USER_TYPE, PROPAZ, _Payload u_get_az);
  getPropFunc(USER_TYPE, PROPAY, _Payload u_get_ay);
  getPropFunc(USER_TYPE, PROPAX, _Payload u_get_ax);
  getPropFunc(USER_TYPE, PROPMSG, _Payload get_msg);
  getPropFunc(USER_TYPE, PROPVRE, _Payload get_vre);
  getPropFunc(USER_TYPE, PROPWEB, _Payload get_web);
  getPropFunc(USER_TYPE, PROPSSRC, _Payload get_ssrc);
  getPropFunc(USER_TYPE, PROPRTCPNAME, _Payload get_rtcpname);
  getPropFunc(USER_TYPE, PROPRTCPEMAIL, _Payload get_rtcpemail);
  getPropFunc(USER_TYPE, PROPMODEL, _Payload get_avatar);
  getPropFunc(USER_TYPE, PROPFACE, _Payload get_uface);
  getPropFunc(USER_TYPE, PROPSEX, _Payload get_sex);
  getPropFunc(USER_TYPE, PROPHEAD, _Payload get_head);
  getPropFunc(USER_TYPE, PROPSKIN, _Payload get_skin);
  getPropFunc(USER_TYPE, PROPBUST, _Payload get_bust);
  getPropFunc(USER_TYPE, PROPRAY, _Payload get_ray);

  putPropFunc(USER_TYPE, PROPHNAME, _Payload put_username);
  putPropFunc(USER_TYPE, PROPMENSURATION, _Payload put_mensuration);
  putPropFunc(USER_TYPE, PROPMAPFRONT, _Payload put_mapfront);
  putPropFunc(USER_TYPE, PROPMAPBACK, _Payload put_mapback);
  putPropFunc(USER_TYPE, PROPMAPLEFT, _Payload put_mapleft);
  putPropFunc(USER_TYPE, PROPMAPRIGHT, _Payload put_mapright);
  putPropFunc(USER_TYPE, PROPXY, _Payload put_xy);
  putPropFunc(USER_TYPE, PROPZ, _Payload put_z);
  putPropFunc(USER_TYPE, PROPAZ, _Payload put_az);
  putPropFunc(USER_TYPE, PROPAY, _Payload put_ay);
  putPropFunc(USER_TYPE, PROPAX, _Payload put_ax);
  putPropFunc(USER_TYPE, PROPMSG, _Payload put_msg);
  putPropFunc(USER_TYPE, PROPVRE, _Payload put_vre);
  putPropFunc(USER_TYPE, PROPWEB, _Payload put_web);
  putPropFunc(USER_TYPE, PROPSSRC, _Payload put_ssrc);
  putPropFunc(USER_TYPE, PROPRTCPNAME, _Payload put_rtcpname);
  putPropFunc(USER_TYPE, PROPRTCPEMAIL, _Payload put_rtcpemail);
  putPropFunc(USER_TYPE, PROPMODEL, _Payload put_avatar);
  putPropFunc(USER_TYPE, PROPFACE, _Payload put_uface);
  putPropFunc(USER_TYPE, PROPSEX, _Payload put_sex);
  putPropFunc(USER_TYPE, PROPHEAD, _Payload put_head);
  putPropFunc(USER_TYPE, PROPSKIN, _Payload put_skin);
  putPropFunc(USER_TYPE, PROPBUST, _Payload put_bust);
  putPropFunc(USER_TYPE, PROPRAY, _Payload put_ray);

  setActionFunc(USER_TYPE, U_BULLET, _Action createBullet, "");
  setActionFunc(USER_TYPE, U_DART, _Action createDart, "");
  setActionFunc(USER_TYPE, U_FOVYDEF, _Action defZoom, "");
  setActionFunc(USER_TYPE, U_FOVYLESS, _Action incrZoom, "");
  setActionFunc(USER_TYPE, U_FOVYMORE, _Action decrZoom, "");
  setActionFunc(USER_TYPE, U_FOVYSET, _Action setZoom, "");
  setActionFunc(USER_TYPE, U_LSPEEDDEF, _Action defLinSpeed, "");
  setActionFunc(USER_TYPE, U_LSPEEDLESS, _Action decrLinSpeed, "");
  setActionFunc(USER_TYPE, U_LSPEEDMORE, _Action incrLinSpeed, "");
  setActionFunc(USER_TYPE, U_ASPEEDDEF, _Action defAngSpeed, "");
  setActionFunc(USER_TYPE, U_ASPEEDLESS, _Action decrAngSpeed, "");
  setActionFunc(USER_TYPE, U_ASPEEDMORE, _Action incrAngSpeed, "");
  setActionFunc(USER_TYPE, U_PAUSE, _Action pause, "");
  setActionFunc(USER_TYPE, U_PAUSEON, _Action pauseOn, "");
  setActionFunc(USER_TYPE, U_PAUSEOFF, _Action pauseOff, "");
  setActionFunc(USER_TYPE, U_SETLSPEED, _Action setLspeed, "");
  setActionFunc(USER_TYPE, U_SETASPEED, _Action setAspeed, "");
  setActionFunc(USER_TYPE, U_SWITCHVIEW, _Action switchView, "");
  setActionFunc(USER_TYPE, U_MAPVIEW, _Action mapView, "");
  setActionFunc(USER_TYPE, U_FIRSTVIEW, _Action firstView, "");
  setActionFunc(USER_TYPE, U_THIRDVIEWFAR, _Action thirdView, "");
  setActionFunc(USER_TYPE, U_TPVIEWROTL, _Action thirdView_RotL, "");
  setActionFunc(USER_TYPE, U_TPVIEWROTR, _Action thirdView_RotR, "");
  setActionFunc(USER_TYPE, U_TPVIEWROTU, _Action thirdView_RotU, "");
  setActionFunc(USER_TYPE, U_TPVIEWROTD, _Action thirdView_RotD, "");
  setActionFunc(USER_TYPE, U_TPVIEWROTN, _Action thirdView_Near, "");
  setActionFunc(USER_TYPE, U_TPVIEWROTF, _Action thirdView_Far, "");
  setActionFunc(USER_TYPE, U_SETROLL, _Action setRoll, "");
  setActionFunc(USER_TYPE, U_SETPITCH, _Action setPitch, "");
  setActionFunc(USER_TYPE, U_PITCHMORE, _Action incrPitch, "");
  setActionFunc(USER_TYPE, U_PITCHLESS, _Action decrPitch, "");
  setActionFunc(USER_TYPE, U_ROLLMORE, _Action incrRoll, "");
  setActionFunc(USER_TYPE, U_ROLLLESS, _Action decrRoll, "");
  setActionFunc(USER_TYPE, U_FLYAWAY, _Action flyaway, "");
  setActionFunc(USER_TYPE, U_TOLAND, _Action toland, "");
}
