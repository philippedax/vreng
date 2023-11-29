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
#include "move.hpp"	// gotoFront

using namespace std;

const OClass Step::oclass(STEP_TYPE, "Step", Step::creator);

const float Step::JUMP = 0.10;
const float Step::LSPEED = 0.5;	// 1/2 ms

// local
static uint16_t oid = 0;


/* creation from a file */
WO * Step::creator(char *l)
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
  stuck = false;
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
      if      (! stringcmp(modestr, "escalator"))  { escalator = true; mobile = true; }
      else if (! stringcmp(modestr, "travelator")) { travelator = true; mobile = true; }
      else if (! stringcmp(modestr, "stair"))      { stair = true; }
      else if (! stringcmp(modestr, "spiral"))     { stair = true; spiral = true; }
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

void Step::build()
{
  uint8_t nsteps = 0;
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
        height += sz;  // add the top step
      }
    }
    else if (length && pos.ax) {
      height = length * tan(pos.ax);
    }
    nsteps = (int) ceil(height / sz);
    //echo("nsteps: %d", nsteps);
    size = height;
  }

  ipos = pos;		// save initial step position for other steps

  for (int n=0; n < nsteps; n++) {
    Pos npos = pos;

    if (spiral) {		// spiral
      float delta = atan(sy / sx);
      npos.x = pos.x + (sx * (cos(delta * n) - 1));
      npos.y = pos.y + (sy * (sin(delta * n) - 1));
      npos.z = pos.z + (sz * n);
      npos.az = pos.az + (delta * n);
      Step *step = new Step(npos, ipos, "spiral", geomsolid, mobile, size, speed, dir);
      stairList.push_back(step);
    }
    else {
      if (travelator) {		// travelator
        npos.x = pos.x - (sx * n);
        npos.y = pos.y - (sy * n);
        npos.z = pos.z;
        Step *step = new Step(npos, ipos, "travelator", geomsolid, mobile, size, speed, dir);
        travList.push_back(step);
      }
      else {			// escalator
        npos.x = pos.x + (sx * n);
        npos.y = pos.y + (sy * n);
        npos.z = pos.z + dir*(sz * n);
        Step *step = new Step(npos, ipos, "escalator", geomsolid, mobile, size, speed, dir);
        escaList.push_back(step);
      }
    }
  }
  if (mobile) {
    enablePermanentMovement(speed);
  }
}

void Step::inits()
{
  initMobileObject(1);
  createPermanentNetObject(PROPS, ++oid);
}

Step::Step(char *l)
{
  parser(l);
  if (stair || escalator || travelator || spiral) {
    build();	// build the structure
  }
  else {
    inits();	// one step
  }
}

Step::Step(Pos& npos, Pos& _ipos, const char *name, const char *geom, bool _mobile, float _size, float _speed, int _dir)
{
  pos = npos;

  char *s = new char[strlen(geom)];
  strcpy(s, geom);
  parseSolid(s);
  delete[] s;

  mobile = _mobile;
  speed = _speed;
  ipos = _ipos;
  stuck = false;
  dir = _dir;
  if (dir == 0) {
    length = _size;  // travelator
  }
  else {
    height = _size;  // stair, escalator
  }

  inits();
  forceNames(name);

  if (mobile) {    // escalator or travelator
    enablePermanentMovement(speed);
    //dax state = ACTIVE;	// only one step !!!
  }
}

/* creation from Gui addobj */
Step::Step(WO *user, char *geom)
{
  parseSolid(geom);

  inits();
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
  if (state == INACTIVE) return;	// not running
  // only escalator and travelator

  float sx = 2 * pos.bbs.v[0];  // step width
  float sy = 2 * pos.bbs.v[1];  // step depth
  float sz = 2 * pos.bbs.v[2];  // step height

  if (dir > 0) { 				// escalator upwards
    pos.x += lasting * move.lspeed.v[0] * sin(pos.az);
    pos.y += lasting * move.lspeed.v[1] * cos(pos.az);
    pos.z += lasting * move.lspeed.v[2];
    if (stuck) {				// user follows up this step
      localuser->pos.x = pos.x;
      localuser->pos.y = pos.y;
      localuser->pos.z = pos.z + localuser->height/2;
      //localuser->pos.z += pos.z;
      if (pos.z >= (ipos.z + height -sz)) {	// user stops when reaches the top
        localuser->pos.x += (sin(pos.az) * sx);
        localuser->pos.y += (cos(pos.az) * sy);
        localuser->pos.z += sz;
        stuck = false;
      }
      localuser->updatePosition();
    }
    if (pos.z >= (ipos.z + height - sz)) {	// rewind step
      //echo("+ %.2f %s", pos.z,getInstance());
      pos = ipos;
      pos.z = ipos.z ; //orig - sz;
    }
  }
  else if (dir < 0) {				// escalator downwards
    pos.x -= lasting * move.lspeed.v[0] * sin(pos.az);
    pos.y -= lasting * move.lspeed.v[1] * cos(pos.az);
    pos.z -= lasting * move.lspeed.v[2];
    if (stuck) {				// user follows down this step
      localuser->pos.x = pos.x;
      localuser->pos.y = pos.y;
      localuser->pos.z = pos.z + localuser->height/2;
      //localuser->pos.z += pos.z;
      if (pos.z <= (ipos.z - height +sz)) {	// user stops when reaches the bottom
        localuser->pos.x -= (sin(pos.az) * sx);
        localuser->pos.y -= (cos(pos.az) * sy);
        stuck = false;
      }
      localuser->updatePosition();
    }
    if (pos.z < (ipos.z - height + sz)) {	// rewind step
      pos = ipos;
    }
  }
  else {					// travelator horizontal
    pos.x -= lasting * move.lspeed.v[0] * sin(pos.az);
    pos.y -= lasting * move.lspeed.v[1] * cos(pos.az);
    if (stuck) {				// user follows this step
      localuser->pos.x = pos.x;
      localuser->pos.y = pos.y;
      if (pos.x >= (ipos.x + length - sx)) {	// user stops when reaches the end
        localuser->pos.x -= (sin(pos.az) * sx);
        localuser->pos.y -= (cos(pos.az) * sy);
        stuck = false;
      }
      localuser->updatePosition();
    }
    if (pos.x >= (ipos.x + length - sx)) {	// rewind step
      pos = ipos;
    }
  }
  updatePositionAndGrid(pos);
}

bool Step::publish(const Pos &oldpos)
{
  return publishPos(oldpos, PROPXY, PROPZ, PROPAZ, PROPAX, PROPAY);
}

/* object intersects: up */
bool Step::whenIntersect(WO *pcur, WO *pold)
{
  switch (pcur->type) {
    case USER_TYPE:
      if (mobile) {	// escalator | travelator
        if (! stuck) {
          stuck = true;
          return true;
        }
        return false;
      }
      else {		// normal case
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
      return false;	// ignore this collision
      break;
    default:
      pold->copyPositionAndBB(pcur);
      break;
  }
  return true;
}

bool Step::whenIntersectOut(WO *pcur, WO *pold)
{
  if (pcur->type == USER_TYPE) {
    stuck = false;
    return true;
  }
  return false;
}

void Step::quit()
{
  stairList.clear();
  escaList.clear();
  travList.clear();
  oid = 0;
}

void Step::running()
{
  if (dir) {
    for (vector<Step*>::iterator it = escaList.begin(); it != escaList.end(); it++) {
      (*it)->state = ACTIVE;
    }
  }
  else {
    for (vector<Step*>::iterator it = travList.begin(); it != travList.end(); it++) {
      (*it)->state = ACTIVE;
    }
  }
}

void Step::pause()
{
  if (dir) {
    //echo("escalator pause: size=%d", escaList.size());	// escaList is empty!
    for (vector<Step*>::iterator it = escaList.begin(); it != escaList.end(); it++) {
      if ((*it)->state & ACTIVE) {
        (*it)->state = INACTIVE;
      }
      else {
        (*it)->state = ACTIVE;
      }
    }
  }
  else {
    //echo("travelator pause: size=%d", travList.size());
    for (vector<Step*>::iterator it = travList.begin(); it != travList.end(); it++) {
      if ((*it)->state & ACTIVE) {
        (*it)->state = INACTIVE;
      }
      else {
        (*it)->state = ACTIVE;
      }
    }
  }
}

void Step::pause_cb(Step *step, void *d, time_t s, time_t u)
{
  step->pause();
}

void Step::funcs()
{
  setActionFunc(STEP_TYPE, 0, _Action pause_cb, "Pause/Running");
  setActionFunc(STEP_TYPE, 1, _Action gotoFront, "Approach");
}
