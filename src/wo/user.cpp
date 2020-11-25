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
#include "vreng.hpp"
#include "user.hpp"
#include "world.hpp"	// getName
#include "ball.hpp"	// BALL_TYPE
#include "dart.hpp"	// DART_TYPE
#include "bullet.hpp"	// BULLET_TYPE
#include "thing.hpp"	// THING_TYPE
#include "step.hpp"	// STEP_TYPE
#include "move.hpp"	// GRAVITY
#include "walls.hpp"	// whenIntersect
#include "mysql.hpp"	// getCountCart
#include "carrier.hpp"	// Carrier
#include "cart.hpp"	// Cart
#include "bubble.hpp"	// Bubble
#include "netobj.hpp"	// NetObject
#include "payload.hpp"	// Payload
#include "solid.hpp"	// solid ray_dlist
#include "draw.hpp"	// ray
#include "render.hpp"	// sharedRender getViewMode
#include "gui.hpp"	// addUser
#include "sound.hpp"	// playSound
#include "rtp.hpp"	// RTPNAME_LEN
#include "pref.hpp"	// user
#include "color.hpp"	// Color
#include "man.hpp"	// Man
#include "guy.hpp"	// Guy
#include "entry.hpp"	// query
#include "halo.hpp"	// Halo
#include "hat.hpp"	// Hat
#include "dress.hpp"	// Dress
#include "wings.hpp"	// Wings
#include "head.hpp"	// Head
#include "android.hpp"	// Android


const OClass User::oclass(USER_TYPE, "User", NULL, User::replicator);
const float User::LASTING = 0.015;	// 15 ms
const float User::DEFAULTWIDTH = 0.26;	// width = 26 cm
const float User::DEFAULTDEPTH = 0.16;	// depth = 16 cm
const float User::DEFAULTHEIGHT = 1.70;	// height= 170 cm
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


void User::defaults()
{
  width = DEFAULTWIDTH;
  depth = DEFAULTDEPTH;
  height = DEFAULTHEIGHT;
  pos.z = height;
  lspeed = LSPEED;
  aspeed = ASPEED;
  ray = newV3(0, 0, 0);
  hit = 0;
  goingup = false;
  carrier = NULL;
  cart = NULL;
  bubble = NULL;
  man = NULL;
  guy = NULL;
  android = NULL;
  head = NULL;
  current_view = 0;
}

void User::setName()
{
  if (! explicitName()) {
    sprintf(names.named, "%s", ::g.user);
    names.instance = names.named;
  }
}

void User::resetPosition()
{
  pos.x = 0;
  pos.y = 0;
  pos.z = height/2 + 0.15;
  pos.az = 0;
  pos.ax = 0;
}

void User::setPosition()
{
  resetPosition();
  Entry *entry = Entry::current();
  if (entry) {
    entry->query(this);
    trace(DBG_INIT, "new entry: %.2f %.2f %.2f", pos.x, pos.y, pos.z);
  } 
}

/* Sets view mode */
void User::setView(uint8_t view)
{
  ::g.render.setViewMode(view);
  current_view = view;

  switch (view) {
    case Render::VIEW_THIRD_PERSON :
    case Render::VIEW_THIRD_PERSON_FAR :
    case Render::VIEW_VERTICAL :
    case Render::VIEW_VERTICAL_FAR :
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

void User::makeSolid()
{
  char s[256];
  memset(s, 0, sizeof(s));
  memset(mensuration, 0, sizeof(mensuration));

  // fill mapping fields
  Pref* pref = &::g.pref;
  
  if (pref->my_widthstr)	width = atof(pref->my_widthstr);
  if (pref->my_depthstr)	depth = atof(pref->my_depthstr);
  if (pref->my_heightstr)	height = atof(pref->my_heightstr);
  if (::g.skinf) {
    if (pref->my_mapfrontstr)	front = strdup(pref->my_mapfrontstr);
    else			front = strdup(::g.skinf);
  }
  if (::g.skinb) {
    if (pref->my_mapbackstr)	back = strdup(pref->my_mapbackstr);
    else			back = strdup(::g.skinb);
  }
  left = strdup(" "); 		*left = 0;
  right = strdup(" "); 		*right = 0;
  if (pref->my_vrestr)		vre = strdup(pref->my_vrestr);
  else				{ vre = strdup(" "); *vre = 0; }
  if (pref->my_webstr)		web = strdup(pref->my_webstr);
  else				{ web = strdup(" "); *web = 0; }
  if (pref->my_avatar)		model = strdup(pref->my_avatar);
  else				{ model = strdup(" "); *model = 0; }
  if (pref->my_facestr)		face = strdup(pref->my_facestr);
  else				{ face = strdup(" "); *face = 0; }
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

  if (isalpha(*model)) {	// model is defined in prefs
    // 5 models available : guy, man, android, box, bbox
    if (! strcmp(model, "guy")) {
      guy = new Guy();
      sprintf(mensuration, "shape=\"guy\" size=\"%.2f %.2f %.2f\"", depth, width, height);
    }
    else if (! strcmp(model, "man")) {
      man = new Man(width, depth, height);
      sprintf(mensuration, "shape=\"man\" size=\"%.2f %.2f %.2f\"", depth, width, height);
    }
    else if (! strcmp(model, "android")) {
      android = new Android();
      sprintf(mensuration, "shape=\"guy\" size=\"%.2f %.2f %.2f\"", depth, width, height);
    }
    else if (! strcmp(model, "box")) {
      sprintf(mensuration, "shape=\"box\" size=\"%.2f %.2f %.2f\"", depth, width, height);
    }
    else {
      sprintf(mensuration, "shape=\"bbox\" size=\"%.2f %.2f %.2f\"", depth, width, height);
    }
  }
  else {	// take the default
    guy = new Guy();
    sprintf(mensuration, "shape=\"guy\" size=\"%.2f %.2f %.2f\"", depth, width, height);
  }
  sprintf(s, "%s yp=\"%s\" xp=\"%s\" />", mensuration, front, back);
  trace(DBG_WO, "User: s=%s", s);
  parse()->parseSolid(s, SEP, this);

  // avatar's head
  if (*headurl) {
    float color[3];
    Color::getRGB(skin, color);
    head = new Head(this, headurl, color);
  }
}

void User::setRtcp()
{
  rtcpname = new char[Rtp::RTPNAME_LEN];
  email    = new char[Rtp::EMAIL_LEN];
  tool     = new char[Rtp::TOOL_LEN];
  Rtp::getRtcpName(rtcpname);
  Rtp::getRtcpEmail(email);
  Rtp::getRtcpTool(tool);
  ssrc = NetObject::getMySsrcId();
  trace(DBG_WO, "User: name=%s ssrc=%x rtcpname=%s email=%s", getInstance(), ssrc, rtcpname, email);
}

void User::setCamera()
{
  ::g.render.cameraProjection(FOVY, NEAR, FAR); // view from user
}

void User::addGui()
{
  if (! guip) guip = ::g.gui.addUser(this);
}

void User::checkPersist()
{
#if HAVE_MYSQL
  VRSql *psql = VRSql::getVRSql();     // first get the the VRSql handle;
  if (psql) {
    int nitem;
    char pat[256], qname[256];

    nitem = psql->getCount(HALO_NAME);  // halos in MySql
    if (nitem) {
      sprintf(pat, "&%s", getInstance());
      if (psql->getName(HALO_NAME, pat, 0, qname) >= 0)
        doAction(HALO_TYPE, Halo::RECREATE, (User*)this, (void*)qname,0,0);
    }
    nitem = psql->getCount(HAT_NAME);   // hats in MySql
    if (nitem) {
      sprintf(pat, "&%s", getInstance());
      if (psql->getName(HAT_NAME, pat, 0, qname) >= 0)
        doAction(HAT_TYPE, Hat::RECREATE, (User*)this, (void*)qname,0,0);
    }
    nitem = psql->getCount(DRESS_NAME); // dresses in MySql
    if (nitem) {
      sprintf(pat, "&%s", getInstance());
      if (psql->getName(DRESS_NAME, pat, 0, qname) >= 0)
        doAction(DRESS_TYPE, Dress::RECREATE, (User*)this, (void*)qname,0,0);
    }
    nitem = psql->getCount(WINGS_NAME); // wings in MySql
    if (nitem) {
      sprintf(pat, "&%s", getInstance());
      if (psql->getName(WINGS_NAME, pat, 0, qname) >= 0)
        doAction(WINGS_TYPE, Wings::RECREATE, (User*)this, (void*)qname,0,0);
    }
#if 0 //todo
    int cartnum = psql->getCountCart();
    //TODO: get the rows
    // addObjectToCart
#endif //todo
  }
#endif
}

void User::inits()
{
  setName();
  updateNames();
  initMobileObject(LASTING);
  setRenderPrior(RENDER_HIGH);
  enablePermanentMovement();	// gravity
  createVolatileNetObject(PROPS);
  //pd noh->declareObjCreation(); // we don't need because delta

  setPosition();	// from entry
  updatePosition();
  makeSolid();
  if (current_view) setView(current_view);
  else              setView(Render::VIEW_FIRST_PERSON);
  setCamera();
  setRtcp();
  clearKeyTab();

  // inform GUI
  addGui();

  // attach carrier & cart
  carrier = new Carrier();
  char *basket = new char[32];
  strcpy(basket, "name=\"basket\"");
  cart = new Cart(basket);

  // checks Persist objects
  checkPersist();
}

/* Creates localuser */
User::User()
{
  localuser = this;	// global
  defaults();
  inits();
}

/* Replicates an user from the network */
WObject * User::replicator(uint8_t type_id, Noid _noid, Payload *pp)
{
  return new User(type_id, _noid, pp);
}

User::User(uint8_t type_id, Noid _noid, Payload *pp)
{
  type = type_id;
  defaults();
  getMemory();		// alloc geometries

  replicateVolatileNetObject(PROPS, _noid);
  copyNoid(_noid);

  /* hack to retrieve the name and the mapping */
  noh->getProperty(/*  0 */ PROPHNAME, pp);
  noh->getProperty(/*  1 */ PROPMAPFRONT, pp);
  noh->getProperty(/*  2 */ PROPMAPBACK, pp);

  int idxgeom, idxend = 0;
  char s[1024];
  memset(s, 0, sizeof(s));

  int idxvar = pp->tellPayload();	// note begin of var
  trace(DBG_WO, "idxvar=%d[%02x]", idxvar, idxvar);
  if (((idxgeom = pp->tellStrInPayload("shape=\"box\" size=")) > 0) ||
      ((idxgeom = pp->tellStrInPayload("shape=\"man\" size=")) > 0) ||
      ((idxgeom = pp->tellStrInPayload("shape=\"guy\" size=")) > 0)
     ) {
    /* get replicated user characteristics from the network */
    trace(DBG_WO, "idxgeom=%d[%02x]", idxgeom, idxgeom);
    noh->getProperty(/* 09 */ PROPMENSURATION, pp);
    noh->getProperty(/* 10 */ PROPMAPLEFT, pp);
    noh->getProperty(/* 11 */ PROPMAPRIGHT, pp);
    noh->getProperty(/* 12 */ PROPVRE, pp);
    noh->getProperty(/* 13 */ PROPWEB, pp);
    noh->getProperty(/* 14 */ PROPSSRC, pp);
    noh->getProperty(/* 15 */ PROPRTCPNAME, pp);
    noh->getProperty(/* 16 */ PROPRTCPEMAIL, pp);
    noh->getProperty(/* 17 */ PROPMODEL, pp);
    noh->getProperty(/* 18 */ PROPFACE, pp);
    noh->getProperty(/* 19 */ PROPSEX, pp);
    noh->getProperty(/* 20 */ PROPHEAD, pp);
    noh->getProperty(/* 21 */ PROPSKIN, pp);
    noh->getProperty(/* 22 */ PROPBUST, pp);
    noh->getProperty(/* 23 */ PROPRAY, pp);
    idxend = pp->tellPayload();	// note end of properties

    if (isalpha(*model))
      sprintf(s, "%s />", mensuration);
    else
      sprintf(s, "%s yp=\"%s\" xp=\"%s\" />", mensuration, front, back);
  }
  else {
    // builtin user solid
    sprintf(s, "dim=\"%.2f %.2f %.2f\" xp=\"%s\" xn=\"%s\" />",
            DEFAULTWIDTH, DEFAULTDEPTH, DEFAULTHEIGHT, front, back);
  }
  trace(DBG_WO, "Replica: s=%s", s);
  parse()->parseSolid(s, SEP, this);

  // get the variable properties
  if (idxend > 0) {
    trace(DBG_WO, "Replica: read var props, idxend=%d", idxend);
    pp->seekPayload(idxvar);	// begin prop var
    for (int np = PROPBEGINVAR; np <= PROPENDVAR; np++) {
      noh->getProperty(np, pp);
    }
    pp->seekPayload(idxend);	// end properties
  }
  else {	// never executed
    error("never executed idxend=%d", idxend);
    uint8_t _nbprop = noh->getPropertiesNumber();
    trace(DBG_WO, "Replica: read all props");
    for (int np = PROPBEGINVAR; np < _nbprop; np++) {
      noh->getProperty(np, pp);
    }
  }

  notice("Avatar: rctpname=%s", rtcpname);
  trace(DBG_WO, "Replica: web=%s vre=%s", web, vre);
  trace(DBG_WO, "Replica: model=%s face=%s", model, face);

  initMobileObject(0);

  trace(DBG_WO, "replica: type=%s named=%s name=%s ssrc=%x rtcpname=%s email=%s", names.type, names.named, getInstance(), ssrc, rtcpname, email);

  addGui();
}

void User::getMemory()
{
  front    = new char[URL_LEN];
  back     = new char[URL_LEN];
  left     = new char[URL_LEN];
  right    = new char[URL_LEN];
  vre      = new char[URL_LEN];
  web      = new char[URL_LEN];
  model    = new char[URL_LEN];
  face     = new char[URL_LEN];
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

User::~User()
{
  //notice("User %s quits", getInstance());
  ::g.gui.removeUser(this);

  // MS. if this destructor is called for a remote user,
  // we should not declare the deletion: it's not our problem.
  if (this == localuser && noh) {
    noh->declareDeletion();
    delete noh;		// delete NetObject
    noh = NULL;
  }

  if (front)   delete front;
  if (back)    delete back;
  if (left)    delete left;
  if (right)   delete right;
  if (vre)     delete vre;
  if (web)     delete web;
  if (model)   delete model;
  if (face)    delete face;
  if (sex)     delete sex;
  if (headurl) delete headurl;
  if (skin)    delete skin;
  if (bust)    delete bust;
  if (color)   delete color;
  //BAD free if (baps)  delete baps;
}

/* Updates local user towards the network */
bool User::updateToNetwork(const Pos &oldpos)
{
  bool change = false;

  if ((pos.x != oldpos.x) || (pos.y != oldpos.y)) {
    noh->declareObjDelta(PROPXY); change = true;
  }
  if (ABSF(pos.z - oldpos.z) > DELTAZ) { // if d < 2cm => not sent
    noh->declareObjDelta(PROPZ); change = true;
  }
  if (pos.az != oldpos.az) {
    noh->declareObjDelta(PROPAZ); change = true;
  }
  return change;
}

/* Updates times array */
void User::updateTime(time_t sec, time_t usec, float *lasting)
{
  updateLasting(sec, usec, lasting);
}

/* Informs a message sent by an user */
void User::userWriting(const char *usermsg)
{
  float green[] = {0,1,0}; // green

  if (strlen(usermsg) < MESS_LEN)
    strcpy(localuser->message, usermsg);
  else {
    strncpy(localuser->message, usermsg, MESS_LEN-1);
    localuser->message[MESS_LEN-1] = '\0';
  }
  localuser->lastmess++;
  localuser->noh->declareObjDelta(PROPMSG); // msg property

  if (localuser->getBubble()) localuser->getBubble()->toDelete(); // delete previous text
  localuser->bubble = new Bubble(localuser, localuser->message, green, Bubble::BUBBLEBACK);

  Sound::playSound(KEYBOARDSND);
}

void User::userRequesting(const char *usermsg)
{
  if (strlen(usermsg) < MESS_LEN) strcpy(localuser->request, usermsg);
  else {
    strncpy(localuser->request, usermsg, MESS_LEN-1);
    localuser->request[MESS_LEN-1] = '\0';
  }
  localuser->lastmess++;
  localuser->noh->declareObjDelta(PROPMSG); // msg property
}

void User::changePosition(float lasting)
{
  pos.x += lasting * move.lspeed.v[0];
  pos.y += lasting * move.lspeed.v[1];
  pos.z += lasting * move.lspeed.v[2];
  pos.az += lasting * move.aspeed.v[0];
}

void User::setGoingup(bool flag)
{
  goingup = flag;
}

/* equations system handling permanent motions */
void User::changePermanent(float lasting)
{
  if (! pause_gravity) pos.z -= lasting * GRAVITY;
}

void User::resetBubble()
{
  bubble = NULL;
}

Bubble * User::getBubble() const
{
  return bubble;
}

void User::bulletPutHit(Bullet *pbullet, Payload *pp)
{
  pp->putPayload("c", pbullet->hit);
}

void User::dartPutHit(Dart *pdart, Payload *pp)
{
  pp->putPayload("c", pdart->hit);
}

bool User::whenIntersect(WObject *pcur, WObject *pold)
{
  // User has no control over ghost objects
  if (pcur->isBehavior(COLLIDE_GHOST)) return true;

  switch (pcur->type) {
  case BULLET_TYPE:
  case DART_TYPE:
    /* projectile intersects a stopped user: hit */
    if (hit == 0) {
      hit = 1;
      notice("%s:%s hits %s", pcur->names.type, pcur->getInstance(), getInstance());
      if (pcur->type == DART_TYPE) {
        ((Dart *)pcur)->hit = 1;
        noh->sendDelta(Dart::PROPHIT);
        ((Dart *)pcur)->hit = 0;
        Sound::playSound(OUILLESND);
      }
      else if (pcur->type == BULLET_TYPE) {
        ((Bullet *)pcur)->hit = 1;
        noh->sendDelta(Bullet::PROPHIT);
        ((Bullet *)pcur)->hit = 0;
        Sound::playSound(RUPSSND);
      }
    }
    pold->copyPositionAndBB(pcur);
    pcur->toDelete();	// delete projectile
    break;
  case BALL_TYPE:
    pcur->pos.x += Ball::SHIFT;	// ball shifts
    pcur->pos.y += Ball::SHIFT;
    pcur->updatePositionAndGrid(pold);
    break;
  case THING_TYPE:
  case USER_TYPE:
    /* user intersects an other user: slide */
    pcur->pos.x += GOTHROUGH; // step to shift
    pcur->pos.y += GOTHROUGH;
    pcur->updatePositionAndGrid(pold);
    break;
  case STEP_TYPE:
    return false;	//TODO
  default:
    pcur->updatePositionAndGrid(pold);
    //dax pold->copyPositionAndBB(pcur);
  }
  return true;
}

void User::setRayDirection(GLint wx, GLint wy)
{
  const GLfloat *white = Color::white;

  GLfloat ex = pos.x + near; // +near
  GLfloat ey = pos.y;
  GLfloat ez = pos.z + height/2 - 0.10;		// top head - 10 cm
#if 0 //dax
  V3 d;
  ::g.render.clickDirection(wx, wy, &d);	// gluUnproject

  GLfloat tx = ex - d.V_X;
  GLfloat ty = ey - d.V_Y;
  GLfloat tz = ez - d.V_Z;
#else
  GLint vp[4];

  glGetIntegerv(GL_VIEWPORT, vp);
  GLfloat tx = (GLfloat) FAR;
  GLfloat ty = vp[2]/2 - (GLfloat) wx;
  GLfloat tz = vp[3]/2 - (GLfloat) wy;
  if (ty < 0) ty = MAX(ty, -FAR); else ty = MIN(ty, FAR);
  if (tz < 0) tz = MAX(tz, -FAR); else tz = MIN(tz, FAR);
#endif
  //error("eye: %.2f %.2f %.2f, target: %.2f %.2f %.2f", ex,ey,ez,tx,ty,tz);
 
  Draw::ray(&(getSolid()->ray_dlist), ex, ey, ez, tx, ty, tz, white, 0x3333);

  ray = newV3(tx, ty, tz);
  noh->declareObjDelta(User::PROPRAY); // publishes ray property to network
}

bool User::hasHead()
{
  return (bool) head;
}

void User::specialAction(int action_id, void *d, time_t s, time_t u)
{
  WObject *o = NULL;

  if (carrier && carrier->isTaking()) o = carrier;  // carrier
  else                                o = this;	    // user

  if (isAction(o->type, action_id)) doAction(o->type, action_id, o, d, s, u);
}

void User::createBullet(User *user, void *d, time_t s, time_t u)
{
  if (isAction(BULLET_TYPE, Bullet::CREATE)) doAction(BULLET_TYPE, Bullet::CREATE, user, d, s, u);
}

void User::createDart(User *user, void *d, time_t s, time_t u)
{
  if (isAction(DART_TYPE, Dart::CREATE)) doAction(DART_TYPE, Dart::CREATE, user, d, s, u);
}

void User::defaultZoom(User *user, void *d, time_t s, time_t u)
{
  fovy = User::FOVY;
  near = User::NEAR;
  ::g.render.cameraProjection(fovy, near, User::FAR);
}

void User::increaseZoom(User *user, void *d, time_t s, time_t u)
{
  fovy -= 1;
  if (fovy <= 1.0) fovy = 1;
  ::g.render.cameraProjection(fovy, near, User::FAR);
}

void User::decreaseZoom(User *user, void *d, time_t s, time_t u)
{
  fovy += 1;
  if (fovy >= 75.0) fovy = 75;
  ::g.render.cameraProjection(fovy, near, User::FAR);
}

void User::setZoom(User *user, void *d, time_t s, time_t u)
{
  int *fovy = (int*) d;

  if (*fovy <= 0) *fovy = 1;
  if (*fovy >= 76) *fovy = 75;
  ::g.render.cameraProjection(*fovy, near, User::FAR);
}

void User::setRoll(User *user, void *d, time_t s, time_t u)
{
  float *angle = (float*) d;
  user->pos.ax = DEG2RAD(*angle);
}

void User::increaseRoll(User *user, void *d, time_t s, time_t u)
{
  user->pos.ax += M_PI/180;
  user->pos.z += 0.05;
  if (user->pos.ax >= M_PI_2) user->pos.ax = M_PI_2;
  user->updatePosition();
}

void User::decreaseRoll(User *user, void *d, time_t s, time_t u)
{
  user->pos.ax -= M_PI/180;
  user->pos.z += 0.05;
  if (user->pos.ax <= -M_PI_2) user->pos.ax = -M_PI_2;
  user->updatePosition();
}

void User::flyaway(User *user, void *d, time_t s, time_t u)
{
  pause_gravity = true;
  user->pos.ay -= M_PI/8;
  user->pos.z += 5;
  user->updatePosition();
}

void User::toland(User *user, void *d, time_t s, time_t u)
{
  pause_gravity = false;
  if (::g.render.getViewMode() == Render::VIEW_FIRST_PERSON)
    user->pos.ay = 0;
  else
    user->pos.ay += M_PI/8;
  user->updatePosition();
}

void User::setPitch(User *user, void *d, time_t s, time_t u)
{
  float *angle = (float*) d;
  ::g.render.setPitch(-DEG2RAD(*angle));
}

void User::increasePitch(User *user, void *d, time_t s, time_t u)
{
  user->pos.ay -= M_PI/180;
  user->pos.z += 0.05;
  if (user->pos.ay <= -M_PI_2) user->pos.ay = -M_PI_2;
  user->updatePosition();
}

void User::decreasePitch(User *user, void *d, time_t s, time_t u)
{
  user->pos.ay += M_PI/180;
  user->pos.z += 0.05;
  if (user->pos.ay >= M_PI_2) user->pos.ay = M_PI_2;
  user->updatePosition();
}

void User::defaultLinearSpeed(User *user, void *d, time_t s, time_t u)
{
  user->lspeed = User::LSPEED;
}

void User::decreaseLinearSpeed(User *user, void *d, time_t s, time_t u)
{
  if (user->lspeed > 1.0) user->lspeed -= 1;
}

void User::increaseLinearSpeed(User *user, void *d, time_t s, time_t u)
{
  user->lspeed += 1;
}

void User::setLspeed(User *user, void *d, time_t s, time_t u)
{
  float *ls = (float *) d;
  user->lspeed = *ls;
}

void User::defaultAngularSpeed(User *user, void *d, time_t s, time_t u)
{
  user->aspeed = User::ASPEED;
}

void User::decreaseAngularSpeed(User *user, void *d, time_t s, time_t u)
{
  if (user->aspeed > 0.3) user->aspeed -= 0.2;
}

void User::increaseAngularSpeed(User *user, void *d, time_t s, time_t u)
{
  user->aspeed += 0.2;
}

void User::setAspeed(User *user, void *d, time_t s, time_t u)
{
  float *as = (float *) d;
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

void User::firstPersonView(User *user, void *d, time_t s, time_t u)
{
  user->setView(Render::VIEW_FIRST_PERSON);
}

void User::thirdPersonViewFar(User *user, void *d, time_t s, time_t u)
{
  user->setView(Render::VIEW_THIRD_PERSON_FAR);
}

void User::thirdPersonView_RotL(User *user, void *d, time_t s, time_t u)
{
  if (::g.render.getViewMode() == Render::VIEW_THIRD_PERSON)
    ::g.render.thirdPerson_xRot += M_PI/18;
}

void User::thirdPersonView_RotR(User *user, void *d, time_t s, time_t u)
{
  if (::g.render.getViewMode() == Render::VIEW_THIRD_PERSON)
    ::g.render.thirdPerson_xRot -= M_PI/18;
}

void User::thirdPersonView_RotU(User *user, void *d, time_t s, time_t u)
{
  Render* render = &::g.render;
  if (render->getViewMode() == Render::VIEW_THIRD_PERSON)
    if (render->thirdPerson_xRot + M_PI_2/4 < M_PI_2)
      render->thirdPerson_yRot += M_PI/18;
}

void User::thirdPersonView_RotD(User *user, void *d, time_t s, time_t u)
{
  Render* render = &::g.render;
  if (render->getViewMode() == Render::VIEW_THIRD_PERSON)
    if (render->thirdPerson_xRot + M_PI_2/4 > -M_PI_2/4)
      render->thirdPerson_yRot -= M_PI/18;
}

void User::thirdPersonView_Near(User *user, void *d, time_t s, time_t u)
{
  Render* render = &::g.render;
  if (render->getViewMode() == Render::VIEW_THIRD_PERSON)
    if (render->thirdPerson_Near > -1.5)
      render->thirdPerson_Near -= 0.1;
}

void User::thirdPersonView_Far(User *user, void *d, time_t s, time_t u)
{
  Render* render = &::g.render;
  if (render->getViewMode() == Render::VIEW_THIRD_PERSON)
    render->thirdPerson_Near += 0.1;
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

void User::setGravity(bool flag)
{
  pause_gravity = flag;
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
  pp->getPayload("s", pu->named());
  trace(DBG_WO, "get_username=%s", pu->named());
}

void User::u_get_xy(WObject *po, Payload *pp)
{
  get_xy(po, pp);
  if (po == localuser) {
    notice("Something's pushing me!");
    po->updateCamera(po->pos);
  }
}

void User::u_get_z(WObject *po, Payload *pp)
{
  get_z(po, pp);
  if (po == localuser) {
    notice("Something's lifting me!");
    po->updateCamera(po->pos);
  }
}

void User::u_get_ax(WObject *po, Payload *pp)
{
  get_ax(po, pp);
  if (po == localuser) {
    notice("Something's twisting me!");
    po->updateCamera(po->pos);
  }
}

void User::u_get_ay(WObject *po, Payload *pp)
{
  get_ay(po, pp);
  if (po == localuser) {
    notice("Something's twisting me!");
    po->updateCamera(po->pos);
  }
}

void User::u_get_az(WObject *po, Payload *pp)
{
  get_az(po, pp);
  if (po == localuser) {
    notice("Something's twisting me!");
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
      ::g.gui.writeMessage("chat", pu->getInstance(), msg);
      pu->lastmess = mess;
      if (pu->getBubble()) pu->getBubble()->toDelete();	// delete previous text
      pu->bubble = new Bubble(pu, msg, blue, Bubble::BUBBLEFRONT);
    }
  }
}

void User::get_infos(User *pu, Payload *pp)
{ if (pu) pp->getPayload("s", pu->names.infos); }

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

void User::get_model(User *pu, Payload *pp)
{ if (pu) pp->getPayload("s", pu->model); }

void User::get_face(User *pu, Payload *pp)
{ if (pu) pp->getPayload("s", pu->face); }

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
        //trace(DBG_FORCE, "get_ray: %.2f,%.2f,%.2f -> %.2f,%.2f,%.2f", ex, ey, ez, tx, ty, tz);
        Draw::ray(&pu->getSolid()->ray_dlist, ex,ey,ez, tx,ty,tz, blue, 0);
      }
      else {
        pu->getSolid()->ray_dlist = 0;
        pu->ray = newV3(0, 0, 0);
      }
    }
  }
}

void User::put_username(User *pu, Payload *pp)
{ if (pu) pp->putPayload("s", pu->named()); }

void User::put_msg(User *pu, Payload *pp)
{ if (pu) pp->putPayload("ds", pu->lastmess, pu->message); }

void User::put_infos(User *pu, Payload *pp)
{ if (pu) pp->putPayload("s", pu->names.infos); }

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

void User::put_model(User *pu, Payload *pp)
{ if (pu) pp->putPayload("s", pu->model); }

void User::put_face(User *pu, Payload *pp)
{ if (pu) pp->putPayload("s", pu->face); }

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
    if (pu->ray.v[0])
      trace(DBG_WO, "put_ray: %.2f,%.2f,%.2f -> %.2f,%.2f,%.2f", pu->pos.x, pu->pos.y, pu->pos.z + pu->height/2 - 0.10, pu->ray.v[0], pu->ray.v[1], pu->ray.v[2]);
  }
}

/** user functions initialization */
void User::funcs()
{
  getPropertyFunc(USER_TYPE, PROPHNAME, WO_PAYLOAD get_username);
  getPropertyFunc(USER_TYPE, PROPMENSURATION, WO_PAYLOAD get_mensuration);
  getPropertyFunc(USER_TYPE, PROPMAPFRONT, WO_PAYLOAD get_mapfront);
  getPropertyFunc(USER_TYPE, PROPMAPBACK, WO_PAYLOAD get_mapback);
  getPropertyFunc(USER_TYPE, PROPMAPLEFT, WO_PAYLOAD get_mapleft);
  getPropertyFunc(USER_TYPE, PROPMAPRIGHT, WO_PAYLOAD get_mapright);
  getPropertyFunc(USER_TYPE, PROPXY, WO_PAYLOAD u_get_xy);
  getPropertyFunc(USER_TYPE, PROPZ, WO_PAYLOAD u_get_z);
  getPropertyFunc(USER_TYPE, PROPAZ, WO_PAYLOAD u_get_az);
  getPropertyFunc(USER_TYPE, PROPAY, WO_PAYLOAD u_get_ay);
  getPropertyFunc(USER_TYPE, PROPAX, WO_PAYLOAD u_get_ax);
  getPropertyFunc(USER_TYPE, PROPMSG, WO_PAYLOAD get_msg);
  getPropertyFunc(USER_TYPE, PROPVRE, WO_PAYLOAD get_vre);
  getPropertyFunc(USER_TYPE, PROPWEB, WO_PAYLOAD get_web);
  getPropertyFunc(USER_TYPE, PROPSSRC, WO_PAYLOAD get_ssrc);
  getPropertyFunc(USER_TYPE, PROPRTCPNAME, WO_PAYLOAD get_rtcpname);
  getPropertyFunc(USER_TYPE, PROPRTCPEMAIL, WO_PAYLOAD get_rtcpemail);
  getPropertyFunc(USER_TYPE, PROPMODEL, WO_PAYLOAD get_model);
  getPropertyFunc(USER_TYPE, PROPFACE, WO_PAYLOAD get_face);
  getPropertyFunc(USER_TYPE, PROPSEX, WO_PAYLOAD get_sex);
  getPropertyFunc(USER_TYPE, PROPHEAD, WO_PAYLOAD get_head);
  getPropertyFunc(USER_TYPE, PROPSKIN, WO_PAYLOAD get_skin);
  getPropertyFunc(USER_TYPE, PROPBUST, WO_PAYLOAD get_bust);
  getPropertyFunc(USER_TYPE, PROPRAY, WO_PAYLOAD get_ray);

  putPropertyFunc(USER_TYPE, PROPHNAME, WO_PAYLOAD put_username);
  putPropertyFunc(USER_TYPE, PROPMENSURATION, WO_PAYLOAD put_mensuration);
  putPropertyFunc(USER_TYPE, PROPMAPFRONT, WO_PAYLOAD put_mapfront);
  putPropertyFunc(USER_TYPE, PROPMAPBACK, WO_PAYLOAD put_mapback);
  putPropertyFunc(USER_TYPE, PROPMAPLEFT, WO_PAYLOAD put_mapleft);
  putPropertyFunc(USER_TYPE, PROPMAPRIGHT, WO_PAYLOAD put_mapright);
  putPropertyFunc(USER_TYPE, PROPXY, WO_PAYLOAD put_xy);
  putPropertyFunc(USER_TYPE, PROPZ, WO_PAYLOAD put_z);
  putPropertyFunc(USER_TYPE, PROPAZ, WO_PAYLOAD put_az);
  putPropertyFunc(USER_TYPE, PROPAY, WO_PAYLOAD put_ay);
  putPropertyFunc(USER_TYPE, PROPAX, WO_PAYLOAD put_ax);
  putPropertyFunc(USER_TYPE, PROPMSG, WO_PAYLOAD put_msg);
  putPropertyFunc(USER_TYPE, PROPVRE, WO_PAYLOAD put_vre);
  putPropertyFunc(USER_TYPE, PROPWEB, WO_PAYLOAD put_web);
  putPropertyFunc(USER_TYPE, PROPSSRC, WO_PAYLOAD put_ssrc);
  putPropertyFunc(USER_TYPE, PROPRTCPNAME, WO_PAYLOAD put_rtcpname);
  putPropertyFunc(USER_TYPE, PROPRTCPEMAIL, WO_PAYLOAD put_rtcpemail);
  putPropertyFunc(USER_TYPE, PROPMODEL, WO_PAYLOAD put_model);
  putPropertyFunc(USER_TYPE, PROPFACE, WO_PAYLOAD put_face);
  putPropertyFunc(USER_TYPE, PROPSEX, WO_PAYLOAD put_sex);
  putPropertyFunc(USER_TYPE, PROPHEAD, WO_PAYLOAD put_head);
  putPropertyFunc(USER_TYPE, PROPSKIN, WO_PAYLOAD put_skin);
  putPropertyFunc(USER_TYPE, PROPBUST, WO_PAYLOAD put_bust);
  putPropertyFunc(USER_TYPE, PROPRAY, WO_PAYLOAD put_ray);

  setActionFunc(USER_TYPE, UserAction::UA_BULLET, WO_ACTION createBullet, "");
  setActionFunc(USER_TYPE, UserAction::UA_DART, WO_ACTION createDart, "");
  setActionFunc(USER_TYPE, UserAction::UA_FOVYDEF, WO_ACTION defaultZoom, "");
  setActionFunc(USER_TYPE, UserAction::UA_FOVYLESS, WO_ACTION increaseZoom, "");
  setActionFunc(USER_TYPE, UserAction::UA_FOVYMORE, WO_ACTION decreaseZoom, "");
  setActionFunc(USER_TYPE, UserAction::UA_FOVYSET, WO_ACTION setZoom, "");
  setActionFunc(USER_TYPE, UserAction::UA_LSPEEDDEF, WO_ACTION defaultLinearSpeed, "");
  setActionFunc(USER_TYPE, UserAction::UA_LSPEEDLESS, WO_ACTION decreaseLinearSpeed, "");
  setActionFunc(USER_TYPE, UserAction::UA_LSPEEDMORE, WO_ACTION increaseLinearSpeed, "");
  setActionFunc(USER_TYPE, UserAction::UA_ASPEEDDEF, WO_ACTION defaultAngularSpeed, "");
  setActionFunc(USER_TYPE, UserAction::UA_ASPEEDLESS, WO_ACTION decreaseAngularSpeed, "");
  setActionFunc(USER_TYPE, UserAction::UA_ASPEEDMORE, WO_ACTION increaseAngularSpeed, "");
  setActionFunc(USER_TYPE, UserAction::UA_PAUSE, WO_ACTION pause, "");
  setActionFunc(USER_TYPE, UserAction::UA_PAUSEON, WO_ACTION pauseOn, "");
  setActionFunc(USER_TYPE, UserAction::UA_PAUSEOFF, WO_ACTION pauseOff, "");
  setActionFunc(USER_TYPE, UserAction::UA_SETLSPEED, WO_ACTION setLspeed, "");
  setActionFunc(USER_TYPE, UserAction::UA_SETASPEED, WO_ACTION setAspeed, "");
  setActionFunc(USER_TYPE, UserAction::UA_SWITCHVIEW, WO_ACTION switchView, "");
  setActionFunc(USER_TYPE, UserAction::UA_MAPVIEW, WO_ACTION mapView, "");
  setActionFunc(USER_TYPE, UserAction::UA_FIRSTVIEW, WO_ACTION firstPersonView, "");
  setActionFunc(USER_TYPE, UserAction::UA_THIRDVIEWFAR, WO_ACTION thirdPersonViewFar, "");
  setActionFunc(USER_TYPE, UserAction::UA_TPVIEWROTL, WO_ACTION thirdPersonView_RotL, "");
  setActionFunc(USER_TYPE, UserAction::UA_TPVIEWROTR, WO_ACTION thirdPersonView_RotR, "");
  setActionFunc(USER_TYPE, UserAction::UA_TPVIEWROTU, WO_ACTION thirdPersonView_RotU, "");
  setActionFunc(USER_TYPE, UserAction::UA_TPVIEWROTD, WO_ACTION thirdPersonView_RotD, "");
  setActionFunc(USER_TYPE, UserAction::UA_TPVIEWROTN, WO_ACTION thirdPersonView_Near, "");
  setActionFunc(USER_TYPE, UserAction::UA_TPVIEWROTF, WO_ACTION thirdPersonView_Far, "");
  setActionFunc(USER_TYPE, UserAction::UA_SETROLL, WO_ACTION setRoll, "");
  setActionFunc(USER_TYPE, UserAction::UA_SETPITCH, WO_ACTION setPitch, "");
  setActionFunc(USER_TYPE, UserAction::UA_PITCHMORE, WO_ACTION increasePitch, "");
  setActionFunc(USER_TYPE, UserAction::UA_PITCHLESS, WO_ACTION decreasePitch, "");
  setActionFunc(USER_TYPE, UserAction::UA_ROLLMORE, WO_ACTION increaseRoll, "");
  setActionFunc(USER_TYPE, UserAction::UA_ROLLLESS, WO_ACTION decreaseRoll, "");
  setActionFunc(USER_TYPE, UserAction::UA_FLYAWAY, WO_ACTION flyaway, "");
  setActionFunc(USER_TYPE, UserAction::UA_TOLAND, WO_ACTION toland, "");
}
