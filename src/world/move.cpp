//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2011 Philippe Dax
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
// move.cpp
//
// Move handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "object.hpp"
#include "move.hpp"
#include "matvec.hpp"   // V3 M4
#include "user.hpp"	// localuser
#include "carrier.hpp"	// keyEvent
#include "netobj.hpp"	// isResponsible
#include "guy.hpp"	// Guy
#include "human.hpp"	// Human

#define MIN_MOVES	20	// orig: 20
#define RATIO_GOTO	4	// orig: 4


/**
 * A key has changed
 * - key_id = 0..MAXKEYS-1
 * - 'true' for a Key Press and 'false' for a Key Release
 * - sec: seconds, usec: micro-seconds
 */
void changeKey(int k_id, bool pressed, time_t sec, time_t usec)
{
  if (k_id >= MAXKEYS || k_id < 0) {
    error("changeKey: invalid k_id = %d", k_id);
    return;
  }
  if (localuser) {
    if (localuser->kpressed[k_id] != pressed) {
      localuser->kpressed[k_id] = pressed;
      if (pressed) {
        localuser->kpstart_s[k_id] = sec;
        localuser->kpstart_u[k_id] = usec;
      }
      else {
        localuser->kpdur_s[k_id] = sec - localuser->kpstart_s[k_id];
        localuser->kpdur_u[k_id] = usec - localuser->kpstart_u[k_id];
      }
    }
  }
}

/** Clears keys times array */
void Object::clearKeyTab()
{
  for (int k=0; k < MAXKEYS; k++) {
    kpressed[k] = false;
    kpstart_s[k] = kpstart_u[k] = 0;
    kpdur_s[k] = kpdur_u[k] = 0;
  }
}

/** Updates the keydifftime arrays */
void Object::updateKeys(time_t sec, time_t usec)
{
  for (int k=0; k < MAXKEYS; k++) {
    if (kpressed[k]) {
      kpdur_s[k] = sec - kpstart_s[k];
      kpdur_u[k] = usec - kpstart_u[k];
      kpstart_s[k] = sec;
      kpstart_u[k] = usec;
    }
  }
}

/** Fills times's array for each user movement direction */
void User::updateTime(float lastings[])
{
  for (int k=0; k < MAXKEYS; k++) {
    lastings[k] = static_cast<float>(kpdur_s[k]) + static_cast<float>(kpdur_u[k]) / 1e6;
    kpdur_s[k] = kpdur_u[k] = 0;
  }
}

/** Tests if moving */
bool Object::testMoving()
{
  return (move.ttl > 0.0005);
}

/** Waits end of movement */
void * endMovement(void *arg)
{
  if (localuser) {
    uint32_t behave = localuser->behavior;	// save current behavior
    localuser->disableGravity();		// avoid to fall into the void
    localuser->enableBehavior(COLLIDE_NEVER);	// temporaly disabled
    sleep((uint32_t) localuser->move.ttl);	// waits
    localuser->enableGravity();			// restore gravity
    localuser->behavior = behave;		// restablish previous behavior
#if HAVE_LIBPTHREAD
    pthread_exit(NULL);
#endif
  }
  return NULL;
}

/** Updates time lasting */
bool Object::updateLasting(time_t sec, time_t usec, float *lasting)
{
  *lasting = diffTime(sec, usec);
  if (move.ttl < 0.0005) return false;
  if (move.next) {
    //echo("next1: ttl=%.1f nocol=%d", move.ttl, move.nocol);
    move = *(move.next);	// copy next move into current - segfault occurs FIXME!
    //echo("next2: ttl=%.1f nocol=%d", move.ttl, move.nocol);
    //delete[] move.next;	// delete next, now obsoleted
    move.next = NULL;
    move.sec = sec;
    move.usec = usec;
    move.perm_sec = sec;
    move.perm_usec = usec;
    if (move.nocol) {
#if HAVE_LIBPTHREAD
      pthread_t tid;
      pthread_create(&tid, NULL, endMovement, (void *) NULL);
#else
      if (! fork()) {
        endMovement(NULL);
        exit(0);
      }
#endif
      move.nocol = false;
    }
    return true;
  }
  if (*lasting < move.ttl) {
    move.ttl -= *lasting;
    move.sec = sec;
    move.usec = usec;
    return true;
  }

  *lasting = 0;
  stopMovement();
  return false;
}

/** Modifies user position in one direction */
void Object::changePositionOneDir(uint8_t move_key, float lasting)
{
  if (carrier && carrier->underControl()) {  // Manipulator
    echo("onedir: k=%d", move_key);
    carrier->mouseEvent(move_key, lasting);
    //dax carrier->keyEvent(move_key, lasting); //arrow keys
    return;
  }

  // Navigator
  switch (move_key) {
    case KEY_FW:  // move forward
      pos.x += lasting * lspeed * cos(pos.az);
      pos.y += lasting * lspeed * sin(pos.az);
      break;
    case KEY_BW:  // move backward
      pos.x -= lasting * lspeed * cos(pos.az);
      pos.y -= lasting * lspeed * sin(pos.az);
      break;
    case KEY_MR:  // move right
      pos.x += lasting * lspeed * sin(pos.az);
      pos.y -= lasting * lspeed * cos(pos.az);
      break;
    case KEY_ML:  // move left
      pos.x -= lasting * lspeed * sin(pos.az);
      pos.y += lasting * lspeed * cos(pos.az);
      break;
    case KEY_RI:  // turn right
      pos.az -= lasting * aspeed;
      pos.az -= M_2PI * static_cast<float>(floor(pos.az / M_2PI));
      break;
    case KEY_LE:  // turn left
      pos.az += lasting * aspeed;
      pos.az -= M_2PI * static_cast<float>(floor(pos.az / M_2PI));
      break;
    case KEY_MD:  // roll down
       pos.ay = MIN(pos.ay + lasting * aspeed, M_2PI_5);
       break;
    case KEY_MU:  // roll up
      pos.ay = MAX(pos.ay - lasting * aspeed, -M_2PI_5);
      break;
    case KEY_HO:  // stand up
      pos.ay = pos.ax = 0;
      break;
    case KEY_UP:  // move up
      pos.z += lasting * lspeed;
      break;
    case KEY_DO:  // move down
      pos.z -= lasting * lspeed;
      break;
    case KEY_TL:  // tilt left
      pos.ax = pos.ax + lasting * aspeed;
      break;
    case KEY_TR:  // tilt right
      pos.ax = pos.ax - lasting * aspeed;
      break;
    }
}

/** Does the movement for each direction */
void User::changePosition(const float lastings[])
{
  for (int k=0; (k < MAXKEYS); k++) {
    if (lastings[k] > MIN_KEYLASTING) {
      changePositionOneDir(k, lastings[k] * (kpressed[KEY_SP]+1));
    }
  }

  if (this == localuser) {
    float a = MIN((pos.z - 1) * (M_PI/18), (M_PI_4+M_PI_4/2));

    if (pos.z > 2) {		// hight >2 m
      // flying
      if (humanoid) {
        localuser->pos.ay = -a;
      }
      if (human) {
        human->pos.ay = -a;
      }
      if (guy) {
        guy->pos.ax = -a;	//
        guy->setAniming(true);
        guy->setFlying(true);
      }
    }
    else if (pos.z > 1) {	// >1 m
      if (humanoid) {
        localuser->pos.ay = -a/2;
      }
      if (human) {
        human->pos.ay = -a/2;
      }
      if (guy) {
        guy->pos.ax = -a/2;
        guy->setAniming(true);
        guy->setFlying(true);
      }
    }
    else if (pos.z < 1) {	// near the ground
      if (humanoid) {
        localuser->pos.ay = 0;
      }
      if (human) {
        human->pos.ay = 0;
      }
      if (guy) {
        if (guy->isFlying()) {
          guy->pos.ax = 0;
          guy->setFlying(false);
          guy->setAniming(false);
        }
        else if (guy->isAnim()) {
          guy->setAniming(false);	// stop anim
        }
        else {
          guy->setAniming(true);	// anim a little bit
        }
      }
    }
  }
}

/** Sets max lasting */
void Object::setLasting(float maxlast)
{
  setMaxLastings(type, maxlast);
}

/** Gets max lasting */
float Object::getLasting() const
{
  return getMaxLastings(type);
}

/** Returns delta time */
float Object::diffTime(time_t sec, time_t usec)
{
  return static_cast<float>((sec - move.sec)) + (static_cast<float>((usec - move.usec) / 1e6));
}

/** Initializes an imposed movement */
void Object::imposedMovement(float ttl)
{
  if (move.sec == 0 && move.usec == 0) {
    struct timeval t;

    gettimeofday(&t, NULL);
    move.sec = t.tv_sec;
    move.usec = t.tv_usec;
  }
  move.ttl = (ttl < 0) ? -ttl : ttl;
  move.next = NULL;
  move.nocol = false;	// with collision
}

/** Stops an imposed movement */
void Object::stopMovement()
{
  move.ttl = 0;
  move.sec = 0;
  move.usec = 0;
  if (move.perm_sec)  move.perm_sec  = 0;
  if (move.perm_usec) move.perm_usec = 0;
  move.next = NULL;
  move.nocol = false;
  for (Move *pm = move.next; pm ; pm = pm->next) {
    delete[] pm;
  }
}

/** Sets uniform linear speed */
void Object::linearSpeed(float lspeed)
{
  move.lspeed.v[0] = lspeed;
  move.lspeed.v[1] = lspeed;
  move.lspeed.v[2] = lspeed;
}

/** Enables a permanent movement */
void Object::permanentMovement()
{
  struct timeval t;

  gettimeofday(&t, NULL);
  move.perm_sec = t.tv_sec;
  move.perm_usec = t.tv_usec;
  move.next = NULL;
}

void Object::permanentMovement(float speed)
{
  linearSpeed(speed);
  permanentMovement();
}

/** Elementary user movement */
void User::elemUserMovement(const float tabdt[])
{
  Object *wo = new Object();
  copyPositionAndBB(wo);	// keep pos for intersection

  changePosition(tabdt);

  if (checkPosition()) {	// sanity check
    echo("pos: %.1f %.1f %.1f", pos.x, pos.y, pos.z);
  }
  updatePosition();
  checkVicinity(wo);
  delete wo;
}

/** User general movement */
void User::userMovement(time_t sec, time_t usec)
{
  float keylastings[MAXKEYS];

  copyPositionAndBB(pos);	// keep pos for network
  updateKeys(sec, usec);
  updateTime(keylastings);

  float lasting = -1.;
  for (int k=0; k < MAXKEYS; k++) {
    if (keylastings[k] > lasting) {
      lasting = keylastings[k];
    }
  }

  if (lasting > MIN_LASTING) {	// user is moving
    float maxlast = getLasting();
    maxlast = maxlast ? maxlast : 1;
    int nbmoves = int( (lasting / maxlast) );
    float tabdt[MAXKEYS];

    for (int m=0; m <= nbmoves; m++) {
      for (int k=0; k < MAXKEYS; k++) {
        if (keylastings[k] > maxlast) {
          tabdt[k] = maxlast;
          keylastings[k] -= maxlast;
        }
        else {
          tabdt[k] = keylastings[k];  // last movement
          keylastings[k] = 0;
        }
      }
      elemUserMovement(tabdt);
    }
    publish(pos);
    updatePositionAndGrid(pos);
    updateCamera(pos);
  }
}

/** Elementary imposed movement for an object */
void Object::elemImposedMovement(float dt)
{
  changePosition(dt);		// handled by each object

  updatePosition();

  if (! isBehavior(COLLIDE_NEVER)) {
    Object *wo = new Object();
    copyPositionAndBB(wo);	// keep pos for intersection
    checkVicinity(wo);
    delete wo;
  }
}

/** Object imposed movement for an object */
void Object::imposedMovements(time_t sec, time_t usec)
{
  if (! isValid()) {
    error("imposedMovements: %s type=%d invalid", name.given, type);
    return;
  }
  if (! isMoving() && ! move.manip) return;	// no moving

  copyPositionAndBB(pos);		// keep pos for network

  float lasting = -1;
  updateTime(sec, usec, &lasting);	// handled by each object only for imposed movements

  move.next = NULL;

  if (lasting > MIN_LASTING) {
    float maxlast = getLasting();
    maxlast = maxlast ? maxlast : 1;
    // spliting movement in m elementary movements
    float tabdt = 0.;
    int nbmoves = int( (lasting / maxlast) );

    for (int m=0; m <= nbmoves; m++) {
      if (lasting > maxlast) {
        tabdt = maxlast;
        lasting -= maxlast;
      }
      else {
        tabdt = lasting;
        lasting = 0;
      }
      elemImposedMovement(tabdt);
      if (isBehavior(NO_ELEMENTARY_MOVE)) {
        return;		// do movement only once
      }
    }
  }

  if (netop && netop->isResponsible()) {
    publish(pos);	// handled by each object
  }
  updatePositionAndGrid(pos);
}

/** Elementary permanent movement for an object */
void Object::elemPermanentMovement(float dt)
{
  if (isBehavior(COLLIDE_NEVER)) {
    changePermanent(dt);	// handled by each object
    update3D(pos);
    return;
  }
  Object *wo = new Object();
  copyPositionAndBB(wo);	// keep pos for intersection

  changePermanent(dt);		// handled by each object

  if (this == localuser) {
    localuser->checkPosition();	// check out-of-bounds
  }
  updatePosition();
  checkVicinity(wo);
  delete wo;
}

/** Object permanent movement - called by world */
void Object::permanentMovements(time_t sec, time_t usec)
{
  if (! isValid()) {
    error("permanentMovements: type=%d invalid", type);
    return;
  }

  if (move.perm_sec > 0) {	// is permanent movement activated ?
    copyPositionAndBB(pos);

    float lasting = static_cast<float>((sec - move.perm_sec)) + static_cast<float>((usec - move.perm_usec) / 1e6);
    move.perm_sec = sec;
    move.perm_usec = usec;
    move.next = NULL;

    if (lasting > 0 /* MIN_LASTING */) {
      float maxlast = getLasting();
      maxlast = maxlast ? maxlast : 1;
      float tabdt = 0.;
      int nbmoves = int( (lasting / maxlast) );
      nbmoves = MIN(nbmoves, MIN_MOVES);

      for (int m=0; m <= nbmoves; m++) {
        if (lasting > maxlast) {
          tabdt = maxlast;
          lasting -= maxlast;
        }
        else {
          tabdt = lasting;
          lasting = 0;
        }

        elemPermanentMovement(tabdt);

        if (isBehavior(NO_ELEMENTARY_MOVE)) {
          return;	// do movement only once
        }
      }
    }
    updateTime(sec, usec, &lasting);	// never called FIXME!

    if (netop && netop->isResponsible()) {
      publish(pos);			// handled by each object
    }

    updatePositionAndGrid(pos);
    if (this == localuser) {
      updateCamera(pos);
    }
  }
}

/** Moves an object */
void Object::moveObject(Object *po, void *d, time_t s, time_t u)
{
  if (! po->carrier) {
    po->carrier = new Carrier();
    po->carrier->take(po);
    po->move.manip = true;
  }
  po->enableBehavior(NO_ELEMENTARY_MOVE); 	// carrier
  po->imposedMovement(5); 			// carrier
  localuser->carrier->take(po);
}

/** Moves the user towards the object */
void Object::moveUserToObject(float sgn, float lttl, float attl)
{
  if (! localuser) return;

  int sign = (sgn >= 0) ? 1 : -1;
  float d = sign * MAX(pos.bbs.v[0], pos.bbs.v[1]) * RATIO_GOTO;
  float dx = pos.x - (d * sin(pos.az)) - localuser->pos.x;
  float dy = pos.y + (d * cos(pos.az)) - localuser->pos.y;
  float dz = ABSF(pos.z - localuser->pos.z);
  float da = (deltaAngle(pos.az - 0, localuser->pos.az + M_PI_2));
  da = MIN(da, M_PI);
  da = MAX(da, -M_PI);
  //echo("a=%.2f o=%.2f u=%.2f", da, pos.az, localuser->pos.az);
  //echo("d=%.1f %.1f,%.1f,%.1f %.2f", d,dx,dy,dz,da);

  // first movement: user turns in direction of the object
  localuser->move.ttl = attl;
  clearV3(localuser->move.lspeed);
  clearV3(localuser->move.aspeed);
  localuser->move.aspeed.v[0] = da / attl;
  localuser->imposedMovement(attl);		// start movement
  localuser->move.nocol = true;

  // second movement: user translation towards the object
  localuser->move.next = new Move[1];
  localuser->move.next->ttl = lttl;
  localuser->move.next->lspeed.v[0] = dx / lttl;
  localuser->move.next->lspeed.v[1] = dy / lttl;
  localuser->move.next->lspeed.v[2] = dz / lttl;
  localuser->move.next->nocol = true;
  localuser->move.next->next = NULL;
  clearV3(localuser->move.aspeed);
}

/** Moves the user at the point x,y,z */
void gotoXYZ(float gox, float goy, float goz, float az)
{
  if (! localuser) return;

  float dx = gox - (sin(az)) - localuser->pos.x;
  float dy = goy + (cos(az)) - localuser->pos.y;
  float dz = goz - localuser->pos.z;
  float da = deltaAngle(az, localuser->pos.az + 1.57);
  float attl = 0.5;  // 1/2 sec
  float lttl = sqrt(dx*dx + dy*dy + dz*dz) / User::LSPEED;

  localuser->move.ttl = attl;
  clearV3(localuser->move.lspeed);
  clearV3(localuser->move.aspeed);
  localuser->move.aspeed.v[0] = da / attl;
  localuser->imposedMovement(attl);	// start orientation

  localuser->move.next = new Move[1];	// build 2nd movement eg. translation
  localuser->move.next->ttl = lttl;	// needed for pthreaded endMovement
  clearV3(localuser->move.lspeed);
  clearV3(localuser->move.aspeed);
  localuser->move.next->lspeed.v[0] = dx / lttl;
  localuser->move.next->lspeed.v[1] = dy / lttl;
  localuser->move.next->lspeed.v[2] = dz / lttl;
  localuser->move.next->nocol = true;
  localuser->move.next->next = NULL;
}

/** Moves the user in front of the object */
void gotoFront(Object *po, void *d, time_t s, time_t u)
{
  po->moveUserToObject(1, 5, .5);	// lttl: 5sec attl: 1/2sec
}

/** Moves the user behind the object */
void gotoBehind(Object *po, void *d, time_t s, time_t u)
{
  po->moveUserToObject(-1, 4, 1);
}
