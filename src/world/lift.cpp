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
// lift.cpp
//
// Lift handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "lift.hpp"
#include "move.hpp"	// goFront
#include "user.hpp"	// localuser
#include "netobj.hpp"   // netop


const OClass Lift::oclass(LIFT_TYPE, "Lift", Lift::creator);

const int Lift::DEF_TOP = 1;		// first floor
const int Lift::DEF_BOT = 0;		// background
const float Lift::DEF_SPEED = 1;	// 1 m/s
const float Lift::DEF_HEIGHT = 3;	// 3 m
const float Lift::DEF_STEP = 0.05;	// 5 cm
const float Lift::DENSITY = 10.0;

// local
static uint16_t oid = 0;


Object * Lift::creator(char *l)
{
  return new Lift(l);
}

void Lift::defaults()
{
  speed = DEF_SPEED;
  height = DEF_HEIGHT;
  top = DEF_TOP;
  bot = DEF_BOT;
}

void Lift::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if      (! stringcmp(l, "height")) l = parseFloat(l, &height, "height");
    else if (! stringcmp(l, "speed"))  l = parseFloat(l, &speed, "speed");
    else if (! stringcmp(l, "top"))    l = parseInt(l, &top, "top");
    else if (! stringcmp(l, "bot"))    l = parseInt(l, &bot, "bot");
    else {
      parse()->errorAtLine(l);
      l = parse()->nextToken();
      break;
    }
  }
  end_while_parse(l);
}

Lift::Lift(char *l)
{
  parser(l);

  state = NONE;		// lift is empty
  mobileObject(0);
  createNetObj(PROPS, ++oid);

  dest = pos.z;		// initial position
}

/** Updates towards the network */
bool Lift::publish(const Pos &oldpos)
{
  bool change = false;

  if (pos.z != oldpos.z) {
    netop->declareDelta(PROPZ);
    change = true;
  }
  return change;
}

void Lift::timing(time_t sec, time_t usec, float *dt)
{
  lasting(sec, usec, dt);
}

void Lift::imposed(float dt)
{
  if (ABSF(dest - pos.z) <= 0.1) return;  // same level

  int sign = (dest > pos.z) ? 1 : -1;
  pos.z += sign * dt * speed;
  if (state == CARRYING) {
    localuser->pos.z += sign * dt * speed;	// user follows the lift
    //dax localuser->updateGrid(localuser->pos);
    localuser->updatePos();
    localuser->updateCamera(pos);
  }
}

void Lift::up()
{
  dest = MIN(pos.z + height, height * top);
  if ((pos.z + 0.1) >= top) return;
  imposedMovement((dest - pos.z) / speed);
}

void Lift::down()
{
  dest = MAX(pos.z - height, height * bot);
  if ((pos.z - 0.1) <= bot) return;
  imposedMovement((dest - pos.z) / speed);
}

void Lift::call()
{
  if (! state) {	// call only from outside
    dest = localuser->pos.z - localuser->height/2 - Lift::DEF_STEP;
    imposedMovement((dest - pos.z) / speed);
  }
}

void Lift::up_cb(Lift *lift, void *d, time_t s, time_t u)
{
  lift->up();
}

void Lift::down_cb(Lift *lift, void *d, time_t s, time_t u)
{
  lift->down();
}

void Lift::call_cb(Lift *lift, void *d, time_t s, time_t u)
{
  lift->call();
}

/** somebody intersects */
bool Lift::intersect(Object *pcur, Object *pold)
{
  switch (pcur->type) {
  case USER_TYPE:
    if (! state) {	// user comes in
      state = CARRYING;
      if (pcur->pos.z >= pold->pos.z) {	// same level
        pcur->pos.z += DEF_STEP;	// moving up
        pcur->updateGrid(pold);
      }
      else {	// inside from up
        pcur->pos = pold->pos;
      }
    }
    else {	// user already inside
      pcur->pos = pold->pos;
    }
    break;
  default:
    pcur->pos = pold->pos;
    break;
  }
  return true;
}

bool Lift::intersectOut(Object *pcur, Object *pold)
{
  if (state == CARRYING) {
    pcur->updateGrid(pold);
    state = NONE;
  }
  return true;
}

Lift::~Lift()
{
  oid = 0;
}

void Lift::funcs()
{
  getPropFunc(LIFT_TYPE, PROPZ, _Payload get_z);
  getPropFunc(LIFT_TYPE, PROPHNAME, _Payload get_hname);

  putPropFunc(LIFT_TYPE, PROPZ, _Payload put_z);
  putPropFunc(LIFT_TYPE, PROPHNAME, _Payload put_hname);

  setActionFunc(LIFT_TYPE, 0, _Action up_cb, "Up");
  setActionFunc(LIFT_TYPE, 1, _Action down_cb, "Down");
  setActionFunc(LIFT_TYPE, 2, _Action call_cb, "Call");
  setActionFunc(LIFT_TYPE, 3, _Action goFront, "Approach");
}
