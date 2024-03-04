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
  travolator = false;
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
      else if (! stringcmp(modestr, "travolator")) { travolator = true; mobile = true; }
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

  if (travolator) {
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
  if (travolator) {
    nsteps = int( ceil(length / MIN(sx, sy)) );
    size = length;
  }
  else {  // escalator stair spiral
    if (height) {
      if (escalator) {
        //height += sz;  // add the top step
      }
    }
    else if (length && pos.ax) {
      height = length * tan(pos.ax);
    }
    nsteps = int( ceil(height / sz) );
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
      if (travolator) {		// travolator
        npos.x = pos.x - (sx * n);
        npos.y = pos.y - (sy * n);
        npos.z = pos.z;
        Step *step = new Step(npos, ipos, "travolator", geomsolid, mobile, size, speed, dir);
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
  createNetObj(PROPS, ++oid);
}

Step::Step(char *l)
{
  parser(l);
  if (stair || escalator || travolator || spiral) {
    build();	// build the structure
  }
  else {
    inits();	// one step
  }
}

Step::Step(Pos& npos, Pos& _ipos, const char *typname, const char *geom, bool _mobile, float _size, float _speed, int _dir)
{
  pos = npos;
  pos.x = npos.x;
  pos.y = npos.y;
  pos.z = npos.z;

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
    length = _size;  // travolator
  }
  else {
    height = _size;  // stair, escalator
  }

  inits();
  forceNames(typname);

  if (mobile) {    // escalator or travolator
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

void Step::updateTime(time_t sec, time_t usec, float *dt)
{
  updateLasting(sec, usec, dt);
}

void Step::changePermanent(float lasting)
{
  if (! mobile) return;
  if (state == INACTIVE) return;	// not running

  // escalator and travolator only

  float sx = 1 * pos.bbs.v[0];  // step width
  float sy = 1 * pos.bbs.v[1];  // step depth
  float sz = 1 * pos.bbs.v[2];  // step height

  if (dir > 0) { 				// escalator upwards
    //echo("h=%.1f %s", height, objectName());
    pos.x += (lasting * move.lspeed.v[2] * sin(pos.az));
    pos.y += (lasting * move.lspeed.v[2] * cos(pos.az));
    pos.z += (lasting * move.lspeed.v[2]);
    if (pos.z >= (ipos.z + height - sz)) {	// rewind step
      //echo("+ %.2f", pos.z);
      pos = ipos;
      pos.z = ipos.z - sz; //orig - sz;
      if (pos.z < 0) {
        //echo("- %.2f", pos.z);
        pos.z = 0;
      }
      //echo("  %.2f", pos.z);
    }
    if (stuck) {				// user follows up this step
      localuser->pos.x = pos.x;
      localuser->pos.y = pos.y;
      localuser->pos.z = pos.z + localuser->height/2;
      if (pos.z >= (ipos.z + height - sz)) {	// user stops when reaches the top
        localuser->pos.x += (sin(pos.az) * sx);
        localuser->pos.y += (cos(pos.az) * sy);
        localuser->pos.z += sz;
        stuck = false;
      }
      localuser->updatePosition();
    }
  }
  else if (dir < 0) {				// escalator downwards
    pos.x -= lasting * move.lspeed.v[2] * sin(pos.az);
    pos.y -= lasting * move.lspeed.v[2] * cos(pos.az);
    pos.z -= lasting * move.lspeed.v[2];
    if (pos.z < (ipos.z - height + sz)) {	// rewind step
      pos = ipos;
    }
    if (stuck) {				// user follows down this step
      localuser->pos.x = pos.x;
      localuser->pos.y = pos.y;
      localuser->pos.z = pos.z + localuser->height/2;
      if (pos.z <= (ipos.z - height + sz)) {	// user stops when reaches the bottom
        localuser->pos.x -= (sin(pos.az) * sx);
        localuser->pos.y -= (cos(pos.az) * sy);
        stuck = false;
      }
      localuser->updatePosition();
    }
  }
  else {					// travolator horizontal
    pos.x -= lasting * move.lspeed.v[0] * sin(pos.az);
    pos.y -= lasting * move.lspeed.v[1] * cos(pos.az);
    if (pos.x >= (ipos.x + length - sx)) {	// rewind step
      pos = ipos;
    }
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
      if (mobile) {	// escalator | travolator
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
    //echo("travolator pause: size=%d", travList.size());
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
  getPropFunc(STEP_TYPE, PROPXY, _Payload get_xy);
  getPropFunc(STEP_TYPE, PROPZ, _Payload get_z);
  getPropFunc(STEP_TYPE, PROPAZ, _Payload get_az);
  getPropFunc(STEP_TYPE, PROPAX, _Payload get_ax);
  getPropFunc(STEP_TYPE, PROPAY, _Payload get_ay);
  getPropFunc(STEP_TYPE, PROPHNAME, _Payload get_hname);

  putPropFunc(STEP_TYPE, PROPXY, _Payload put_xy);
  putPropFunc(STEP_TYPE, PROPZ, _Payload put_z);
  putPropFunc(STEP_TYPE, PROPAZ, _Payload put_az);
  putPropFunc(STEP_TYPE, PROPAX, _Payload put_ax);
  putPropFunc(STEP_TYPE, PROPAY, _Payload put_ay);
  putPropFunc(STEP_TYPE, PROPHNAME, _Payload put_hname);

  setActionFunc(STEP_TYPE, 0, _Action pause_cb, "Pause/Running");
  setActionFunc(STEP_TYPE, 1, _Action gotoFront, "Approach");
}
