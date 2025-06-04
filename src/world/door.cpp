//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2009 Philippe Dax
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
// door.cpp
//
// Door handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "door.hpp"
#include "matvec.hpp"   // V3 M4
#include "move.hpp"	// goFront
#include "user.hpp"	// USER-TYPE
#include "netobj.hpp"	// NetObj
#include "sound.hpp"	// playSound


const OClass Door::oclass(DOOR_TYPE, "Door", Door::creator);

// local
static uint16_t oid = 0;


/** Creates from vre file */
Object * Door::creator(char *l)
{
  return new Door(l);
}

void Door::parser(char *l)
{
  mecanism = NOMECANISM;
  aopen = xopen = zopen = 0;
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if      (! stringcmp(l, "open")) {
      l = parseFloat(l, &aopen, "open");
      mecanism = ANGULAR;
    }
    else if (! stringcmp(l, "close")) {
      l = parseFloat(l, &aclose, "close");
      mecanism = ANGULAR;
    }
    else if (! stringcmp(l, "xopen")) {
      l = parseFloat(l, &xopen, "xopen");
      mecanism = SLIDING;
    }
    else if (! stringcmp(l, "xclose")) {
      l = parseFloat(l, &xclose, "xclose");
      mecanism = SLIDING;
    }
    else if (! stringcmp(l, "zopen")) {
      l = parseFloat(l, &zopen, "zopen");
      mecanism = VERTICAL;
    }
    else if (! stringcmp(l, "zclose")) {
      l = parseFloat(l, &zclose, "zclose");
      mecanism = VERTICAL;
    }
    else if (! stringcmp(l, "speed")) {
      l = parseFloat(l, &speed, "speed");
    }
    else {
      parse()->errorAtLine(l);
      l = parse()->nextToken();
      break;
    }
  }
  end_while_parse(l);
}

/** Constructor */
Door::Door(char *l)
{
  parser(l);

  /* calls persistency VSql server to know the door state */
  getPersist(state);
  //echo("Door: sql state: %d", state);

  switch (mecanism) {
  case ANGULAR:
    state = (aclose == pos.az) ? CLOSED : OPENED;
    pos.az = (state & CLOSED) ? aclose : aopen;
    //echo("Door: sql state: %d %.2f", state, pos.az);
    break;
  case SLIDING:
    state = (xclose == pos.x) ? CLOSED : OPENED;
    break;
  case VERTICAL:
    state = (zclose == pos.z) ? CLOSED : OPENED;
    break;
  }

  V3 bbs, bbc;
  getRelBB(bbc, bbs);
  size.v[0] = bbs.v[0];
  center.v[0] = pos.x;	// hinge
  center.v[1] = pos.y;
  center.v[2] = pos.z;
  pos.x += size.v[0] * cos(pos.az);
  pos.y += size.v[0] * sin(pos.az);
  //dax echo("pos=(%.2f,%.2f,%.2f) center=(%.2f,%.2f,%.2f) size=%.2f", pos.x, pos.y, pos.z, center.v[0], center.v[1], center.v[2], size.v[0]);

  enableBehavior(PERSISTENT);

  mobileObject(1);
  createNetObj(PROPS, ++oid);
}

void Door::timing(time_t s, time_t us, float *last)
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

  if (! lasting(s, us, last) && state == CLOSED)
    Sound::playSound(DOORCLOSESND);
}

void Door::imposed(float dt)
{
  switch (mecanism) {
  case ANGULAR:
    pos.az += dt * move.aspeed.v[0];
    pos.x = center.v[0] + size.v[0] * cos(pos.az);
    pos.y = center.v[1] + size.v[0] * sin(pos.az);
    break;
  case SLIDING:
    pos.x += dt * move.lspeed.v[0];
    break;
  case VERTICAL:
    pos.z += dt * move.lspeed.v[2];
    break;
  }
}

/** Updates the network */
bool Door::publish(const Pos &oldpos)
{
  bool change = false;

  if ((pos.x != oldpos.x) || (pos.y != oldpos.y)) {
    netop->declareDelta(PROPXY);
    change = true;
  }
  if (pos.az != oldpos.az) {
    netop->declareDelta(PROPAZ);
    change = true;
  }
  return change;
}

/** Intersects with an object */
bool Door::intersect(Object *pcur, Object *pold)
{
  switch (pcur->type) {
  case USER_TYPE:
    projectPosition(pcur, pold);
  case DOOR_TYPE:
    break;
  default:
    pcur->pos = pold->pos;
    break;
  }
  return true;
}

void Door::open()
{
  if (state == Door::OPENED || state == Door::LOCKED)  return;

  clearV3(move.lspeed);
  clearV3(move.aspeed);
  switch (mecanism) {
  case Door::ANGULAR:
    move.aspeed.v[0] = speed;
    imposedMovement(ABSF(deltaAngle(pos.az, aopen) / move.aspeed.v[0]));
    break;
  case Door::SLIDING:
    move.lspeed.v[0] = speed;
    imposedMovement(ABSF((pos.x - xopen) / move.lspeed.v[0]));
    break;
  case Door::VERTICAL:
    move.lspeed.v[2] = speed;
    imposedMovement(ABSF((pos.z - zopen) / move.lspeed.v[2]));
    break;
  }
  Sound::playSound(DOOROPENSND);
  state = OPENED;
  updatePersist();
}

void Door::close()
{
  if (state & Door::CLOSED)  return;

  clearV3(move.lspeed);
  clearV3(move.aspeed);
  switch (mecanism) {
  case Door::ANGULAR:
    move.aspeed.v[0] = -speed;
    imposedMovement(ABSF(deltaAngle(pos.az, aclose) / move.aspeed.v[0]));
    break;
  case Door::SLIDING:
    move.lspeed.v[0] = -speed;
    imposedMovement(ABSF((pos.x - xclose) / move.lspeed.v[0]));
    break;
  case Door::VERTICAL:
    move.lspeed.v[2] = -speed;
    imposedMovement(ABSF((pos.z - zclose) / move.lspeed.v[2]));
    break;
  }
  state = CLOSED;
  updatePersist();
}

void Door::lock()
{
  if (state == Door::LOCKED)  return;

  switch (state) {
  case Door::UNLOCKED:
  case Door::CLOSED:
    state = Door::LOCKED;
    updatePersist();
  default:
    break;
  }
}

void Door::unlock()
{
  if (state == Door::OPENED || state == Door::UNLOCKED)  return;

  switch (state) {
  case Door::LOCKED:
    state = Door::UNLOCKED;
    updatePersist();
  default:
    break;
  }
}

Door::~Door()
{
  oid = 0;
  savePersist();
}

void Door::open_cb(Door *door, void *d, time_t s, time_t u)
{
  door->open();
}

void Door::close_cb(Door *door, void *d, time_t s, time_t u)
{
  door->close();
}

void Door::lock_cb(Door *door, void *d, time_t s, time_t u)
{
  door->lock();
}

void Door::unlock_cb(Door *door, void *d, time_t s, time_t u)
{
  door->unlock();
}

void Door::funcs()
{
  getPropFunc(DOOR_TYPE, PROPXY, _Payload get_xy);
  getPropFunc(DOOR_TYPE, PROPZ, _Payload get_z);
  getPropFunc(DOOR_TYPE, PROPAZ, _Payload get_az);
  getPropFunc(DOOR_TYPE, PROPHNAME, _Payload get_hname);

  putPropFunc(DOOR_TYPE, PROPXY, _Payload put_xy);
  putPropFunc(DOOR_TYPE, PROPZ, _Payload put_z);
  putPropFunc(DOOR_TYPE, PROPAZ, _Payload put_az);
  putPropFunc(DOOR_TYPE, PROPHNAME, _Payload put_hname);

  setActionFunc(DOOR_TYPE, 0, _Action open_cb, "Open");
  setActionFunc(DOOR_TYPE, 1, _Action close_cb, "Close");
  setActionFunc(DOOR_TYPE, 2, _Action unlock_cb, "Unlock");
  setActionFunc(DOOR_TYPE, 3, _Action lock_cb, "Lock");
  setActionFunc(DOOR_TYPE, 4, _Action goFront, "Approach");
}
