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
#include "vreng.hpp"
#include "door.hpp"
#include "move.hpp"	// gotoFront
#include "user.hpp"	// USER-TYPE
#include "netobj.hpp"	// NetObject
#include "sound.hpp"	// playSound


const OClass Door::oclass(DOOR_TYPE, "Door", Door::creator);

// local
static uint16_t oid = 0;


WObject * Door::creator(char *l)
{
  return new Door(l);
}

void Door::parser(char *l)
{
  mecanism = NOMECANISM;
  aopen = xopen = zopen = 0;
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if      (!stringcmp(l, "open")) {
      l = parse()->parseFloat(l, &aopen, "open");
      mecanism = ANGULAR;
    }
    else if (!stringcmp(l, "close")) {
      l = parse()->parseFloat(l, &aclose, "close");
      mecanism = ANGULAR;
    }
    else if (!stringcmp(l, "xopen")) {
      l = parse()->parseFloat(l, &xopen, "xopen");
      mecanism = SLIDING;
    }
    else if (!stringcmp(l, "xclose")) {
      l = parse()->parseFloat(l, &xclose, "xclose");
      mecanism = SLIDING;
    }
    else if (!stringcmp(l, "zopen")) {
      l = parse()->parseFloat(l, &zopen, "zopen");
      mecanism = VERTICAL;
    }
    else if (!stringcmp(l, "zclose")) {
      l = parse()->parseFloat(l, &zclose, "zclose");
      mecanism = VERTICAL;
    }
    else if (!stringcmp(l, "speed")) {
      l = parse()->parseFloat(l, &speed, "speed");
    }
  }
  end_while_parse(l);
}

/* Constructor */
Door::Door(char *l)
{
  parser(l);

  switch (mecanism) {
  case ANGULAR:
    state = (aclose == pos.az) ? CLOSED : OPENED;
    break;
  case SLIDING:
    state = (xclose == pos.x) ? CLOSED : OPENED;
    break;
  case VERTICAL:
    state = (zclose == pos.z) ? CLOSED : OPENED;
    break;
  }

  /* calls persistency MySql server to know the door state */
  getPersistency(state);

  switch (mecanism) {
  case ANGULAR:
    pos.az = (state & CLOSED) ? aclose : aopen;
    trace(DBG_WO, "Door: sql state: %d %.2f", state, pos.az);
    break;
  case SLIDING:
    state = (xclose == pos.x) ? CLOSED : OPENED;
    break;
  case VERTICAL:
    state = (zclose == pos.z) ? CLOSED : OPENED;
    break;
  }

  V3 bbs, bbc;
  getRelativeBB(bbc, bbs);
  size.v[0] = bbs.v[0];
  center.v[0] = pos.x;	// hinge
  center.v[1] = pos.y;
  center.v[2] = pos.z;
  pos.x += size.v[0] * cos(pos.az);
  pos.y += size.v[0] * sin(pos.az);
  //dax trace(DBG_FORCE, "pos=(%.2f,%.2f,%.2f) center=(%.2f,%.2f,%.2f) size=%.2f", pos.x, pos.y, pos.z, center.v[0], center.v[1], center.v[2], size.v[0]);

  enableBehavior(PERSISTENT);	// after init because mysql already done
  setRenderPrior(RENDER_NORMAL);

  initializeMobileObject(1);
  createPermanentNetObject(PROPS, ++oid);
}

void Door::updateTime(time_t s, time_t us, float *lasting)
{
  switch (mecanism) {
  case ANGULAR:
    if (move.aspeed.v[0] == speed)
      move.ttl = MIN(ABSF(deltaAngle(pos.az, aopen) / move.aspeed.v[0]), move.ttl);
    else
      move.ttl = MIN(ABSF(deltaAngle(pos.az, aclose) / move.aspeed.v[0]), move.ttl);
    break;
  case SLIDING:
    if (move.lspeed.v[0] == speed)
      move.ttl = MIN(ABSF((pos.x - xopen) / move.lspeed.v[0]), move.ttl);
    else
      move.ttl = MIN(ABSF((pos.x - xclose) / move.lspeed.v[0]), move.ttl);
    break;
  case VERTICAL:
    if (move.lspeed.v[2] == speed)
      move.ttl = MIN(ABSF((pos.z - zopen) / move.lspeed.v[2]), move.ttl);
    else
      move.ttl = MIN(ABSF((pos.z - zclose) / move.lspeed.v[2]), move.ttl);
    break;
  }

  if (! updateLasting(s, us, lasting) && state == CLOSED)
    Sound::playSound(DOORCLOSESND);
}

void Door::changePosition(float lasting)
{
  switch (mecanism) {
  case ANGULAR:
    pos.az += lasting * move.aspeed.v[0];
    pos.x = center.v[0] + size.v[0] * cos(pos.az);
    pos.y = center.v[1] + size.v[0] * sin(pos.az);
    //pos.z = center.v[2];
    break;
  case SLIDING:
    //pos.x = center.v[0] + size.v[0] * cos(pos.az);
    //pos.y = center.v[1] + size.v[0] * sin(pos.az);
    pos.x += lasting * move.lspeed.v[0];
    break;
  case VERTICAL:
    pos.z += lasting * move.lspeed.v[2];
    break;
  }
}

/* Updates the network */
bool Door::updateToNetwork(const Pos &oldpos)
{
  bool change = false;

  if ((pos.x != oldpos.x) || (pos.y != oldpos.y)) {
    noh->declareObjDelta(PROPXY);
    change = true;
  }
  if (pos.az != oldpos.az) {
    noh->declareObjDelta(PROPAZ);
    change = true;
  }
  return change;
}

/* Intersects with an object */
bool Door::whenIntersect(WObject *pcur, WObject *pold)
{
  switch (pcur->type) {
  case USER_TYPE:
    projectPosition(pcur, pold);
  case DOOR_TYPE:
    break;
  default:
    pold->copyPositionAndBB(pcur);
    break;
  }
  return true;
}

void Door::open()
{
  if (state == Door::OPENED || state == Door::LOCKED)
    return;
  clearV3(move.lspeed);
  clearV3(move.aspeed);
  switch (mecanism) {
  case Door::ANGULAR:
    move.aspeed.v[0] = speed;
    initImposedMovement(ABSF(deltaAngle(pos.az, aopen) / move.aspeed.v[0]));
    break;
  case Door::SLIDING:
    move.lspeed.v[0] = speed;
    initImposedMovement(ABSF((pos.x - xopen) / move.lspeed.v[0]));
    break;
  case Door::VERTICAL:
    move.lspeed.v[2] = speed;
    initImposedMovement(ABSF((pos.z - zopen) / move.lspeed.v[2]));
    break;
  }
  Sound::playSound(DOOROPENSND);
  state = OPENED;
  pos.moved = true;	// has moved
  updatePersistency(state);
}

void Door::close()
{
  if (state & Door::CLOSED)
    return;
  clearV3(move.lspeed);
  clearV3(move.aspeed);
  switch (mecanism) {
  case Door::ANGULAR:
    move.aspeed.v[0] = -speed;
    initImposedMovement(ABSF(deltaAngle(pos.az, aclose) / move.aspeed.v[0]));
    break;
  case Door::SLIDING:
    move.lspeed.v[0] = -speed;
    initImposedMovement(ABSF((pos.x - xclose) / move.lspeed.v[0]));
    break;
  case Door::VERTICAL:
    move.lspeed.v[2] = -speed;
    initImposedMovement(ABSF((pos.z - zclose) / move.lspeed.v[2]));
    break;
  }
  state = CLOSED;
  pos.moved = true;	// has moved
  updatePersistency(state);
}

void Door::lock()
{
  if (state == Door::LOCKED)
    return;
  switch (state) {
  case Door::UNLOCKED:
  case Door::CLOSED:
    state = Door::LOCKED;
    pos.moved = true;	// has moved
    updatePersistency(state);
  default:
    break;
  }
}

void Door::unlock()
{
  if (state == Door::OPENED || state == Door::UNLOCKED)
    return;
  switch (state) {
  case Door::LOCKED:
    state = Door::UNLOCKED;
    pos.moved = true;	// has moved
    updatePersistency(state);
  default:
    break;
  }
}

void Door::quit()
{
  oid = 0;
  quitPersistency();
}

void Door::open_cb(Door *door, void *d, time_t s, time_t u) { door->open(); }
void Door::close_cb(Door *door, void *d, time_t s, time_t u) { door->close(); }
void Door::lock_cb(Door *door, void *d, time_t s, time_t u) { door->lock(); }
void Door::unlock_cb(Door *door, void *d, time_t s, time_t u) { door->unlock(); }

void Door::funcs()
{
  getPropertyFunc(DOOR_TYPE, PROPXY, WO_PAYLOAD get_xy);
  getPropertyFunc(DOOR_TYPE, PROPZ, WO_PAYLOAD get_z);
  getPropertyFunc(DOOR_TYPE, PROPAZ, WO_PAYLOAD get_az);
  getPropertyFunc(DOOR_TYPE, PROPHNAME, WO_PAYLOAD get_hname);

  putPropertyFunc(DOOR_TYPE, PROPXY, WO_PAYLOAD put_xy);
  putPropertyFunc(DOOR_TYPE, PROPZ, WO_PAYLOAD put_z);
  putPropertyFunc(DOOR_TYPE, PROPAZ, WO_PAYLOAD put_az);
  putPropertyFunc(DOOR_TYPE, PROPHNAME, WO_PAYLOAD put_hname);

  setActionFunc(DOOR_TYPE, 0, WO_ACTION open_cb, "Open");
  setActionFunc(DOOR_TYPE, 1, WO_ACTION close_cb, "Close");
  setActionFunc(DOOR_TYPE, 2, WO_ACTION unlock_cb, "Unlock");
  setActionFunc(DOOR_TYPE, 3, WO_ACTION lock_cb, "Lock");
  setActionFunc(DOOR_TYPE, 4, WO_ACTION gotoFront, "Approach");
}
