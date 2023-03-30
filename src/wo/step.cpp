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
#include "ball.hpp"	// BALL_TYPE
#include "move.hpp"	// gotoFront

using namespace std;

const OClass Step::oclass(STEP_TYPE, "Step", Step::creator);

list<Step*> Step::stepList;	// steps list

const float Step::JUMP = 0.10;
const float Step::LSPEED = 0.5;	// 1/2 ms

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
    l = parseAttributes(l);
    if (!l) break;
    if (! stringcmp(l, "mode=")) {
      char modestr[16];
      l = parseString(l, modestr, "mode");
      if      (! stringcmp(modestr, "escalator")) { escalator = true; mobile = true; }
      else if (! stringcmp(modestr, "travelator")) { travelator = true; mobile = true; }
      else if (! stringcmp(modestr, "stair")) stair = true;
      else if (! stringcmp(modestr, "spiral")) { stair = true; spiral = true; }
    }
    else if (! stringcmp(l, "dir=")) {
      char modestr[16];
      l = parseString(l, modestr, "dir");
      if      (! stringcmp(modestr, "up"))   dir = 1;	// up
      else if (! stringcmp(modestr, "down")) dir = -1;	// down
    }
    else if (! stringcmp(l, "height")) l = parseFloat(l, &height, "height");
    else if (! stringcmp(l, "length")) l = parseFloat(l, &length, "length");
    else if (! stringcmp(l, "speed"))  l = parseFloat(l, &speed, "speed");
  }
  end_while_parse(l);

  if (travelator) {
    height = 0;
    dir = 0;
  }
}

void Step::makeSolid()
{
  char s[128];

  sprintf(s, "%s", geometry);		// step geometry duplicated
  parseSolid(s);
}

void Step::build()
{
  float size = 0;
  float sx = 2 * pos.bbs.v[0];  // step width
  float sy = 2 * pos.bbs.v[1];  // step depth
  float sz = 2 * pos.bbs.v[2];  // step height

  // steps are interlaced (sx sy sz)
  if (travelator) {
    nsteps = (int) ceil(length / MIN(sx, sy));
    size = length;
  }
  else {  // escalator stair spiral
    if (height) {
      if (escalator) {
        //dax height += sz;  // add the top step
      }
    }
    else if (length && pos.ax) {
      height = length * tan(pos.ax);
    }
    nsteps = (int) ceil(height / sz);
    //echo("nsteps: %d", nsteps);
    size = height;
  }

  initialpos = pos;  // save initial step position for other steps
  stepList.push_back(this);

  for (int n=0; n < nsteps; n++) {
    Pos newpos;
    newpos.az = pos.az;
    newpos.ax = pos.ax;
    newpos.ay = pos.ay;

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
        newpos.x = pos.x - (sx * n);
        newpos.y = pos.y - (sy * n);
        newpos.z = pos.z;
      }
      else {  // escalator stair
        newpos.x = pos.x + (sx * n);
        newpos.y = pos.y + (sy * n);
        newpos.z = pos.z + dir*(sz * n);
      }
    }
    nextstep = new Step(newpos, initialpos, "step", geometry, mobile, size, speed, dir);
    stepList.push_back(nextstep);
  }

  if (mobile) {
    enablePermanentMovement(speed);
  }
}

void Step::behavior()
{
  initMobileObject(1);
  createPermanentNetObject(PROPS, ++oid);
}

Step::Step(char *l)
{
  parser(l);
  behavior();
  if (stair || escalator || travelator || spiral) {
    build();	// build the structure
  }
}

Step::Step(Pos& newpos, Pos& _initialpos, const char *name, const char *_geom, bool _mobile, float _size, float _speed, int _dir)
{
  pos = newpos;

  char *s = new char[strlen(_geom)];
  strcpy(s, _geom);
  parseSolid(s);

  mobile = _mobile;
  dir = _dir;
  if (dir == 0) {
    length = _size;  // travelator
  }
  else {
    height = _size;  // stair, escalator
  }
  speed = _speed;
  initialpos = _initialpos;

  initMobileObject(1);
  forceNames(name);

  if (mobile) {    // escalator or travelator
    enablePermanentMovement(speed);
    state = ACTIVE;	// only one step !!!
  }
}

/* creation from Gui addobj */
Step::Step(WObject *user, char *geom)
{
  parseSolid(geom);

  behavior();
  enableBehavior(DYNAMIC);

  /* position in front of localuser */
  pos.x = user->pos.x + 0.7;
  pos.y = user->pos.y;
  pos.z = user->pos.z + 0.5;
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

  float sx = 2 * pos.bbs.v[0];  // step width
  float sy = 2 * pos.bbs.v[1];  // step depth
  float sz = 1 * pos.bbs.v[2];  // step height

  if (dir > 0) {
    //
    // escalator upwards
    //
    pos.x += lasting * move.lspeed.v[0] * sin(pos.az);
    pos.y += lasting * move.lspeed.v[1] * cos(pos.az);
    pos.z += lasting * move.lspeed.v[2];
    if (pos.z > (initialpos.z + height /*- sz*/)) {	// switch step
      //echo("+ %.1f %s", pos.z,getInstance());
      pos.x = initialpos.x - (sin(pos.az) * sx);
      pos.y = initialpos.y - (cos(pos.az) * sy);
      pos.z = initialpos.z; //orig - sz;
    }
    if (stuck) {	// user follows up this step
      localuser->pos.x = pos.x;
      localuser->pos.y = pos.y;
      localuser->pos.z = pos.z + localuser->height/2;
      if (pos.z >= (initialpos.z + height - sz)) {	// user stops at top
        localuser->pos.x += (sin(pos.az) * sx);
        localuser->pos.y += (cos(pos.az) * sy);
        localuser->pos.z += sz;
        stuck = false;
      }
      localuser->updatePosition();
    }
  }
  else if (dir < 0) {
    //
    // escalator downwards
    //
    pos.x -= lasting * move.lspeed.v[0] * sin(pos.az);
    pos.y -= lasting * move.lspeed.v[1] * cos(pos.az);
    pos.z -= lasting * move.lspeed.v[2];
    if (pos.z < (initialpos.z - height + sz)) {	// switch step
      pos.x = initialpos.x;
      pos.y = initialpos.y;
      pos.z = initialpos.z;
    }
    if (stuck) {	// user follows down this step
      localuser->pos.x = pos.x;
      localuser->pos.y = pos.y;
      localuser->pos.z = pos.z + localuser->height/2;
      if (pos.z <= (initialpos.z - height + sz)) {	// user stops at bottom
        localuser->pos.x -= (sin(pos.az) * sx);
        localuser->pos.y -= (cos(pos.az) * sy);
        stuck = false;
      }
      localuser->updatePosition();
    }
  }
  else {
    //
    // travelator horizontal
    //
    pos.x -= lasting * move.lspeed.v[0] * sin(pos.az);
    pos.y -= lasting * move.lspeed.v[1] * cos(pos.az);
    if (stuck) {	// user follows this step
      localuser->pos.x = pos.x;
      localuser->pos.y = pos.y;
      if (pos.x >= (initialpos.x + length - sx)) {	// user stops end
        localuser->pos.x -= (sin(pos.az) * sx);
        localuser->pos.y -= (cos(pos.az) * sy);
        stuck = false;
      }
      localuser->updatePosition();
    }
    if (pos.x >= (initialpos.x + length - sx)) {	// switch step
      pos.x = initialpos.x;
      pos.y = initialpos.y;
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
          if (pos.bbs.v[2] < JUMP/8) pcur->pos.z += pos.bbs.v[2];
          else pcur->pos.z += JUMP;  // up
          pcur->updatePositionAndGrid(pold);
        }
      }
      break;
    case STEP_TYPE:
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
  stepList.clear();
  oid = 0;
}

void Step::pause()
{
  for (list<Step*>::iterator it = stepList.begin(); it != stepList.end(); it++) {
    if ((*it)->state & ACTIVE)
      (*it)->state = INACTIVE;
    else
      (*it)->state = ACTIVE;
  }
}

void Step::pause_cb(Step *step, void *d, time_t s, time_t u)
{
  step->pause();
}

#if 0 //notused
void Step::stop_cb(Step *step, void *d, time_t s, time_t u)
{
  if (step->mobile == true) step->mobile = false;
  else                      step->mobile = true;
  if (step->state & ACTIVE) step->state = INACTIVE;
  else                      step->state = ACTIVE;
}

void Step::destroy_cb(Step *step, void *d, time_t s, time_t u)
{
  if (step->isBehavior(DYNAMIC)) {
    step->removeFromScene();
  }
}
#endif //notused

void Step::funcs()
{
  setActionFunc(STEP_TYPE, 0, _Action pause_cb, "Pause/Continue");
  setActionFunc(STEP_TYPE, 1, _Action gotoFront, "Approach");
  //setActionFunc(STEP_TYPE, 1, _Action stop_cb, "Stop/Restart");
}
