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
#include "step.hpp"
#include "user.hpp"	// localuser
#include "guide.hpp"	// GUIDE_TYPE
#include "ball.hpp"	// BALL_TYPE
#include "move.hpp"	// gotoFront

using namespace std;

const OClass Step::oclass(STEP_TYPE, "Step", Step::creator);

const float Step::JUMP = 0.10;
const float Step::LSPEED = 0.5;	// 1/2 ms
list<Step*> Step::stepList;

// local
static uint16_t oid = 0;


/* creation from a file */
WObject * Step::creator(char *l)
{
  return new Step(l);
}

void Step::defaults()
{
  mobile = false;
  escalator = false;
  travelator = false;
  stair = false;
  spiral = false;

  dir = 1;
  height = 0;
  length = 0;
  speed = LSPEED;
}

void Step::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if (!stringcmp(l, "mode=")) {
      char modestr[16];
      l = parse()->parseString(l, modestr, "mode");
      if      (! stringcmp(modestr, "escalator")) { escalator = true; mobile = true; }
      else if (! stringcmp(modestr, "travelator")) { travelator = true; mobile = true; }
      else if (! stringcmp(modestr, "stair")) stair = true;
      else if (! stringcmp(modestr, "spiral")) { stair = true; spiral = true; }
    }
    if (!stringcmp(l, "dir=")) {
      char modestr[16];
      l = parse()->parseString(l, modestr, "dir");
      if      (! stringcmp(modestr, "up"))   dir = 1;
      else if (! stringcmp(modestr, "down")) dir = -1;
    }
    else if (! stringcmp(l, "height")) l = parse()->parseFloat(l, &height, "height");
    else if (! stringcmp(l, "length")) l = parse()->parseFloat(l, &length, "length");
    else if (! stringcmp(l, "speed"))  l = parse()->parseFloat(l, &speed, "speed");
  }
  end_while_parse(l);

  if (travelator) {
    height = 0;
    dir = 0;
  }
}

void Step::addList()
{
  stepList.push_back(this);
}

void Step::clearList()
{
  stepList.clear();
}

void Step::build()
{
  float size;
  float sx = pos.bbsize.v[0];  // step width
  float sy = pos.bbsize.v[1];  // step depth
  float sz = pos.bbsize.v[2];  // step height

  char *geom = new char[strlen(geometry) + 1];
  strcpy(geom, geometry);  // step geometry to duplicate

  // steps are interlaced (sx sy sz)
  if (travelator) {
    nsteps = (int) ceil(length / sx);
    size = length;
  }
  else {  // escalator stair spiral
    if (height) {
      if (escalator) height += sz;  // add the top step
    }
    else if (length && pos.ax) {
      height = length * tan(pos.ax);
      pos.ax = 0;
    }
    nsteps = (int) ceil(height / sz);
    size = height;
  }

  firstpos = pos;  // save first step position for other steps

  for (int n=0; n <= nsteps; n++) {
    Pos newpos;
    newpos.az = pos.az;
    newpos.ax = newpos.ay = 0;

    if (spiral) {
      float deltaspiral = atan(sy / sx);
      newpos.x = pos.x + (sx * (cos(deltaspiral * n) - 1));
      newpos.y = pos.y + (sy * (sin(deltaspiral * n) - 1));
      newpos.z = pos.z + (sz * n);
      newpos.az = pos.az + (deltaspiral * n);
    }
    else {
      if (travelator) {
        //FIXME: wrong position
        newpos.x = pos.x - (sin(pos.az) * sx * n);
        newpos.y = pos.y - (cos(pos.az) * sy * n);
        newpos.z = pos.z;
      }
      else {  // escalator stair
        newpos.x = pos.x + dir*(sin(pos.az) * sx * n);
        newpos.y = pos.y + dir*(cos(pos.az) * sy * n);
        newpos.z = pos.z + dir*(sz * n);
      }
    }
    nextstep = new Step(newpos, firstpos, geom, mobile, size, speed, dir);
  }
  if (geom) delete[] geom;

  if (mobile) {
    disableBehavior(PERSISTENT);
    enablePermanentMovement(speed);
  }
}

void Step::behavior()
{
  enableBehavior(PERSISTENT);
  setRenderPrior(PRIOR_MEDIUM);

  initMobileObject(1);
  createPermanentNetObject(PROPS, ++oid);
}

Step::Step(char *l)
{
  parser(l);
  behavior();
  if (stair || escalator || travelator || spiral)
    build();
  //notused addList();
}

Step::Step(Pos& newpos, Pos& _firstpos, char *_geom, bool _mobile, float _size, float _speed, int _dir)
{
  pos = newpos;

  char *geom = new char[strlen(_geom)];
  strcpy(geom, _geom);
  parse()->parseSolid(geom, SEP, this);
  if (geom) delete[] geom; //random crash in free

  mobile = _mobile;
  dir = _dir;
  if (dir == 0) length = _size;  // travelator
  else height = _size;  // stair, escalator
  speed = _speed;
  firstpos = _firstpos;
  stuck = false;

  initMobileObject(1);
  //notused addList();

  if (mobile) {    // escalator or travelator
    disableBehavior(PERSISTENT);
    enablePermanentMovement(speed);
    state = ACTIVE;
  }
}

Step::Step(WObject *user, char *geom)
{
  parse()->parseSolid(geom, SEP, this);

  behavior();

  /* position in front of localuser */
  pos.x = user->pos.x +0.4;
  pos.y = user->pos.y;
  pos.z = user->pos.z +0.5;
  updatePosition();
}

void Step::updateTime(time_t sec, time_t usec, float *lasting)
{
  updateLasting(sec, usec, lasting);
}

void Step::changePermanent(float lasting)
{
  if (! mobile) return;
  if (state == INACTIVE) return;

  float sx = pos.bbsize.v[0];  // step width
  float sy = pos.bbsize.v[1];  // step depth
  float sz = pos.bbsize.v[2];  // step height

  if (dir > 0) {  // escalator upwards
    pos.x += lasting * move.lspeed.v[0] * sin(pos.az);
    pos.y += lasting * move.lspeed.v[1] * cos(pos.az);
    pos.z += lasting * move.lspeed.v[2];
    if (stuck) {  // user follows this step
      localuser->pos.x = pos.x;
      localuser->pos.y = pos.y;
      localuser->pos.z = pos.z + localuser->height/2;
      if (pos.z >= (firstpos.z - sz + height)) {  // user stops
        localuser->pos.x += (sin(pos.az) * sx);
        localuser->pos.y += (cos(pos.az) * sy);
        localuser->pos.z += sz;
        stuck = false;
      }
      localuser->updatePosition();
    }
    if (pos.z >= (firstpos.z - sz + height)) {  // switch step
      pos.x = firstpos.x - (sin(pos.az) * sx);
      pos.y = firstpos.y - (cos(pos.az) * sy);
      pos.z = firstpos.z - sz;
    }
  }
  else if (dir < 0) {  // escalator downwards
    pos.x -= lasting * move.lspeed.v[0] * sin(pos.az);
    pos.y -= lasting * move.lspeed.v[1] * cos(pos.az);
    pos.z -= lasting * move.lspeed.v[2];
    if (stuck) {  // user follows this step
      localuser->pos.x = pos.x;
      localuser->pos.y = pos.y;
      localuser->pos.z = pos.z + localuser->height/2;
      if (pos.z <= (firstpos.z + sz - height)) {  // user stops
        localuser->pos.x -= (sin(pos.az) * sx);
        localuser->pos.y -= (cos(pos.az) * sy);
        stuck = false;
      }
      localuser->updatePosition();
    }
    if (pos.z <= (firstpos.z + sz - height)) {
      pos.x = firstpos.x;
      pos.y = firstpos.y;
      pos.z = firstpos.z;
    }
  }
  else {  // travelator horizontal
    pos.x -= lasting * move.lspeed.v[0] * sin(pos.az);
    pos.y -= lasting * move.lspeed.v[1] * cos(pos.az);
    if (stuck) {  // user follows this step
      localuser->pos.x = pos.x;
      localuser->pos.y = pos.y;
      if (pos.x >= (firstpos.x - sx + length)) {  // user stops
        localuser->pos.x -= (sin(pos.az) * sx);
        localuser->pos.y -= (cos(pos.az) * sy);
        stuck = false;
      }
      localuser->updatePosition();
    }
    if (pos.x >= (firstpos.x - sx + length)) {
      pos.x = firstpos.x;
      pos.y = firstpos.y;
    }
  }

  updatePositionAndGrid(pos);
}

bool Step::updateToNetwork(const Pos &oldpos)
{
  return updatePosToNetwork(oldpos, PROPXY, PROPZ, PROPAZ, PROPAX, PROPAY);
}

/* object intersects: up */
bool Step::whenIntersect(WObject *pcur, WObject *pold)
{
  switch (pcur->type) {

    case USER_TYPE:
      if (mobile) {  // escalator | travelator
        if (! stuck) {
          stuck = true;
          return true;
        }
        return false;
      }
      else {  // normal case
        if (pcur->pos.x == pold->pos.x && pcur->pos.y == pold->pos.y) {
          pold->copyPositionAndBB(pcur);
        }
        else {
          if (pos.bbsize.v[2] < JUMP/8) pcur->pos.z += pos.bbsize.v[2];
          else pcur->pos.z += JUMP;  // up
          pcur->updatePositionAndGrid(pold);
        }
      }
      break;

    case STEP_TYPE:
    case GUIDE_TYPE:
      return false;  // ignore this collision
      break;

    case BALL_TYPE:
      if (mobile) return false;
      else pold->copyPositionAndBB(pcur);
      break;

    default:
      pold->copyPositionAndBB(pcur);
  }
  return true;
}

bool Step::whenIntersectOut(WObject *pcur, WObject *pold)
{
  if (pcur->type == USER_TYPE) {
    stuck = false;
    return true;
  }
  return false;
}

void Step::quit()
{
  oid = 0;
  clearList();
  savePersistency();
}

void Step::pause_cb(Step *step, void *d, time_t s, time_t u)
{
  if (step->state == ACTIVE) step->state = INACTIVE;
  else                       step->state = ACTIVE;
}

void Step::stop_cb(Step *step, void *d, time_t s, time_t u)
{
  if (step->mobile == true) step->mobile = false;
  else                      step->mobile = true;
  if (step->state == ACTIVE) step->state = INACTIVE;
  else                       step->state = ACTIVE;
}

void Step::destroy_cb(Step *step, void *d, time_t s, time_t u)
{
  step->removeFromScene();
}

void Step::funcs()
{
  setActionFunc(STEP_TYPE, 0, WO_ACTION gotoFront, "Approach");
  setActionFunc(STEP_TYPE, 1, WO_ACTION pause_cb, "Pause/Continue");
  setActionFunc(STEP_TYPE, 2, WO_ACTION stop_cb, "Stop/Restart");
}
