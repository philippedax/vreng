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
#include "wobject.hpp"
#include "move.hpp"
#include "world.hpp"
#include "user.hpp"	// localuser
#include "carrier.hpp"	// keyEvent
#include "netobj.hpp"	// isResponsible
#include "guy.hpp"	// Guy
#include "human.hpp"	// Human

#define MIN_MOVES	20	// orig: 20
#define RATIO_GOTO	6	// orig: 4


/**
 * notify a key has changed
 * key_id = 0..MAXKEYS-1
 * TRUE for a Key Press and FALSE for a Key Release
 * sec: seconds, usec: micro-seconds
 */
void changeKey(int k_id, bool pressed, time_t sec, time_t usec)
{
  if (k_id >= MAXKEYS || k_id < 0) {
    warning("changeKey: invalid k_id = %d", k_id);
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

/* clear keys times array */
void User::clearKeyTab()
{
  for (int k=0; k < MAXKEYS; k++) {
    kpressed[k] = false;
    kpstart_s[k] = kpstart_u[k] = 0;
    kpdur_s[k] = kpdur_u[k] = 0;
  }
}

/* update the keydifftime arrays */
void User::updateKeys(time_t sec, time_t usec)
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

bool WObject::testMoving()
{
  return (move.ttl > 0.0005);
}

/* wait end of movement */
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

bool WObject::updateLasting(time_t sec, time_t usec, float *lasting)
{
  *lasting = diffTime(sec, usec);
  if (*lasting < move.ttl) {
    move.ttl -= *lasting;
    move.sec = sec;
    move.usec = usec;
    return true;
  }
  else {
    if (move.next) {
      move = *(move.next);	// copy next motion into current
      delete[] move.next;	// delete next, now obsoleted
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
    *lasting = 0;
    stopImposedMovement();
    return false;
  }
}

/* modify user position in one direction */
void User::changePositionOneDir(int move_type, float lasting)
{
  if (carrier && carrier->isTaking()) {  // Manipulator
    carrier->mouseEvent(move_type, lasting);
    carrier->keyEvent(move_type, lasting); //arrow keys
  }
  else {                                 // Navigator
    switch (move_type) {
      case KEY_AV:  // move forward left
        pos.x += lasting * lspeed * cos(pos.az);
        pos.y += lasting * lspeed * sin(pos.az);
        break;
      case KEY_AR:  // move backward right
        pos.x -= lasting * lspeed * cos(pos.az);
        pos.y -= lasting * lspeed * sin(pos.az);
        break;
      case KEY_SD:  // move forward right
        pos.x += lasting * lspeed * sin(pos.az);
        pos.y -= lasting * lspeed * cos(pos.az);
        break;
      case KEY_SG:  // move backward left
        pos.x -= lasting * lspeed * sin(pos.az);
        pos.y += lasting * lspeed * cos(pos.az);
        break;
      case KEY_DR:  // turn right
        pos.az -= lasting * aspeed;
        pos.az -= M_2PI * (float) floor(pos.az / M_2PI);
        break;
      case KEY_GA:  // turn left
        pos.az += lasting * aspeed;
        pos.az -= M_2PI * (float) floor(pos.az / M_2PI);
        break;
       case KEY_MT:  // roll backward
         pos.ay = MIN(pos.ay + lasting * aspeed, M_2PI_5);
         break;
       case KEY_DE:  // roll forward
         pos.ay = MAX(pos.ay - lasting * aspeed, -M_2PI_5);
         break;
       case KEY_HZ:  // stand up
         pos.ay = pos.ax = 0;
         break;
       case KEY_JU:  // move up
         pos.z += lasting * lspeed;
         break;
       case KEY_JD:  // move down
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
}

/* fill times's array for each user movement direction */
void User::updateTime(float lastings[])
{
  for (int k=0; k < MAXKEYS; k++) {
    lastings[k] = (float) kpdur_s[k] + (float) kpdur_u[k] / 1e6;
    kpdur_s[k] = kpdur_u[k] = 0;
  }
}

/* do the movement for each direction */
void User::changePosition(const float lastings[])
{
  for (int k=0; (k < MAXKEYS); k++) {
    if (lastings[k] > MIN_KEYLASTING) {
      changePositionOneDir(k, lastings[k] * (kpressed[KEY_VI]+1));
    }
  }

  // Fly
  //dax float ground = World::current()->getGround();
  //dax float h = pos.z - ground;  // vertical distance
  float a = MIN((pos.z - 1) * (M_PI/18), (M_PI_4+M_PI_4/2));
  if (this == localuser) {
    if (pos.z > 2) {	// >2 m
      if (human) {
        human->pos.ay = -a;
      }
      if (guy) {
        guy->pos.ax = -a;	//orig ay
        guy->setAniming(true);
        guy->setFlying(true);
      }
    }
    else if (pos.z < 1) { // near the ground
      if (human) {
        human->pos.ay = 0;
      }
      if (guy) {
        guy->pos.ax = 0;  //orig ay
        guy->setAniming(false);
        guy->setFlying(false);
      }
    }
  }
}

void WObject::setLasting(float maxlast)
{
  setMaxLastings(type, maxlast);
}

float WObject::getLasting() const
{
  return getMaxLastings(type);
}

float WObject::diffTime(time_t sec, time_t usec)
{
  return (float) (sec - move.sec) + ((float) (usec - move.usec) / 1e6);
}

void WObject::enableImposedMovement()
{
  struct timeval t;

  gettimeofday(&t, NULL);
  move.sec = t.tv_sec;
  move.usec = t.tv_usec;
}

void WObject::initImposedMovement(float ttl)
{
  enableImposedMovement();
  move.ttl = (ttl < 0) ? -ttl : ttl;
  move.next = NULL;
  move.nocol = false;	// with collision
}

void WObject::stopImposedMovement()
{
  move.ttl = 0;
  move.sec = 0;
  move.usec = 0;
  move.nocol = false;
  for (Move *pm = move.next; pm ; pm = pm->next) {
    delete[] pm;
  }
}

void WObject::enablePermanentMovement()
{
  struct timeval t;

  gettimeofday(&t, NULL);
  move.perm_sec = t.tv_sec;
  move.perm_usec = t.tv_usec;
}

void WObject::enablePermanentMovement(float speed)
{
  move.lspeed.v[0] = speed;
  move.lspeed.v[1] = speed;
  move.lspeed.v[2] = speed;
  enablePermanentMovement();
}

void WObject::disablePermanentMovement()
{
  move.perm_sec = 0;
  move.perm_usec = 0;
}

void WObject::checkVicinity(WObject *pold)
{
  OList *vicinity = getVicinity(pold);
  if (vicinity) {
    generalIntersect(pold, vicinity);	// handled by each object
    vicinity->remove();
  }
}

void User::elemUserMovement(const float tabdt[])
{
  WObject *po = new WObject();
  copyPositionAndBB(po);	// keep oldpos for intersection
  changePosition(tabdt);
  updatePosition();
  checkVicinity(po);
  delete po;
}

/* user general movement */
void User::userMovement(time_t sec, time_t usec)
{
  Pos oldpos = pos;
  float keylastings[MAXKEYS];

  copyPosAndBB(oldpos);  // keep oldpos for network
  updateKeys(sec, usec);
  updateTime(keylastings);

  float lasting = -1.;
  for (int k=0; k < MAXKEYS; k++) {
    if (keylastings[k] > lasting) {
      lasting = keylastings[k];
    }
  }

  if (lasting > MIN_LASTING) {  // user is moving
    float maxlast = getLasting();
    maxlast = maxlast ? maxlast : 1;
    int nbmoves = (int) (lasting / maxlast);
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
    updateToNetwork(oldpos);
    updatePositionAndGrid(oldpos);
    updateCamera(pos);
  }
}

void WObject::elemImposedMovement(float dt)
{
  changePosition(dt);  // handled by each object
  updatePosition();

  if (! isBehavior(COLLIDE_NEVER)) {
    WObject *pold = new WObject();
    copyPositionAndBB(pold);	// keep oldpos for intersection
    checkVicinity(pold);
    delete pold;
  }
}

/* object imposed movement */
void WObject::imposedMovement(time_t sec, time_t usec)
{
  if (! isValid()) {
    error("imposedMovement: type=%d invalid", type);
    return;
  }
  if (World::current()->isDead()) return;
  if (! isMoving()) return;	// no moving

  Pos oldpos = pos;
  copyPosAndBB(oldpos);		// keep oldpos for network

  float lasting = -1;
  updateTime(sec, usec, &lasting);  // handled by each object only for imposed movements

  if (lasting > MIN_LASTING) {
    float maxlast = getLasting();
    maxlast = maxlast ? maxlast : 1;
    // spliting movement in m elementary movements
    float tabdt = 0.;
    int nbmoves = (int) (lasting / maxlast);

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

  if (noh && noh->isResponsible()) {
    updateToNetwork(oldpos);	// handled by each object
  }
  updatePositionAndGrid(oldpos);
}

void WObject::elemPermanentMovement(float dt)
{
  if (isBehavior(COLLIDE_NEVER)) {
    changePermanent(dt);	// handled by each object
    update3D(pos);
    return;
  }
  WObject *pold = new WObject();
  copyPositionAndBB(pold);	// keep oldpos for intersection
  changePermanent(dt);		// handled by each object
  updatePosition();
  checkVicinity(pold);
  delete pold;
}

/* object permanent movement */
void WObject::permanentMovement(time_t sec, time_t usec)
{
  if (! isValid()) {
    error("permanentMovement: type=%d invalid", type);
    return;
  }
  if (World::current()->isDead()) return;

  if (move.perm_sec > 0) {	// is permanent movement activated ?
    Pos oldpos = pos;
    copyPosAndBB(oldpos);

    float lasting = (float)(sec - move.perm_sec) + (float)(usec - move.perm_usec) / 1e6;
    move.perm_sec = sec;
    move.perm_usec = usec;

    if (lasting > MIN_LASTING) {
      float maxlast = getLasting();
      maxlast = maxlast ? maxlast : 1;
      float tabdt = 0.;
      int nbmoves = (int) (lasting / maxlast);
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
    //dax8 updateTime(sec, usec, &lasting);	// see clock.cpp never called FIXME!

    if (noh && noh->isResponsible()) {
      updateToNetwork(oldpos);		// handled by each object
    }

    updatePositionAndGrid(oldpos);
    if (this == localuser) {
      updateCamera(pos);
    }
  }
}

/* Moves the user towards the object */
void WObject::moveUserToObject(float sgn, float lttl, float attl)
{
  if (! localuser) return;

  int sign = (sgn >= 0) ? 1 : -1;
  float d = sign * MAX(pos.bbs.v[0], pos.bbs.v[1]) * RATIO_GOTO;
  //float dx = pos.x - (d * sin(pos.az)) - localuser->pos.x;
  //float dy = pos.y + (d * cos(pos.az)) - localuser->pos.y;
  float dx = pos.x - localuser->pos.x;
  float dy = pos.y - localuser->pos.y;
  float dz = ABSF(pos.z - localuser->pos.z);
  float da = (deltaAngle(pos.az - 0, localuser->pos.az + M_PI_2));
  da = MIN(da, M_PI);
  da = MAX(da, -M_PI);
  //error("a=%.2f o=%.2f u=%.2f", da, pos.az, localuser->pos.az);
  //error("d=%.1f %.1f,%.1f,%.1f %.2f", d,dx,dy,dz,da);

  // first movement: user turns in direction of the object
  localuser->move.ttl = attl;
  clearV3(localuser->move.lspeed);
  clearV3(localuser->move.aspeed);
  localuser->move.aspeed.v[0] = da / attl;
  localuser->initImposedMovement(attl);	// init orientation
  localuser->move.nocol = true;

  // second movement: user translates towards the object
  localuser->move.next = new Move[1];
  localuser->move.next->ttl = lttl;
  localuser->move.next->lspeed.v[0] = dx / lttl;
  localuser->move.next->lspeed.v[1] = dy / lttl;
  localuser->move.next->lspeed.v[2] = dz / lttl;
  localuser->move.next->nocol = true;
  localuser->move.next->next = NULL;
  //localuser->initImposedMovement(lttl);	// init translation
  clearV3(localuser->move.aspeed);
}

/* Move the user at the point x,y,z */
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
  localuser->initImposedMovement(attl);	// start orientation

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

/* Moves the user in front of the object */
void gotoFront(WObject *po, void *d, time_t s, time_t u)
{
  po->moveUserToObject(1, 5, .5);	// lttl: 5sec attl: 1/2sec
}

/* Moves the user behind the object */
void gotoBehind(WObject *po, void *d, time_t s, time_t u)
{
  po->moveUserToObject(-1, 4, 1);
}
