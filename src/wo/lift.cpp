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
#include "lift.hpp"
#include "move.hpp"	// gotoFront
#include "user.hpp"	// localuser
#include "netobj.hpp"	// NetObj


const OClass Lift::oclass(LIFT_TYPE, "Lift", Lift::creator);

const int Lift::DEF_TOP = 1;		// first floor
const int Lift::DEF_BOT = 0;		// background
const float Lift::DEF_SPEED = 1;	// 1 m/s
const float Lift::DEF_HEIGHT = 3;	// 3 m
const float Lift::DEF_STEP = 0.05;	// 5 cm
const float Lift::DENSITY = 10.0;

// local
static uint16_t oid = 0;


WObject * Lift::creator(char *l)
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
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if      (!stringcmp(l, "height")) l = parse()->parseFloat(l, &height, "height");
    else if (!stringcmp(l, "speed"))  l = parse()->parseFloat(l, &speed, "speed");
    else if (!stringcmp(l, "top"))    l = parse()->parseInt(l, &top, "top");
    else if (!stringcmp(l, "bot"))    l = parse()->parseInt(l, &bot, "bot");
  }
  end_while_parse(l);
}

Lift::Lift(char *l)
{
  parser(l);

  enableBehavior(PERSISTENT);	// after init because mysql already done
  setRenderPrior(RENDER_NORMAL);

  initializeMobileObject(0);
  createPermanentNetObj(PROPS, ++oid);

  dest = pos.z;		// initial position
}

/* Updates towards the network */
bool Lift::updateToNetwork(const Pos &oldpos)
{
  bool change = false;

  if (pos.z != oldpos.z) {
    noh->declareObjDelta(PROPZ);
    change = true;
  }
  return change;
}

void Lift::updateTime(time_t sec, time_t usec, float *lasting)
{
  updateLasting(sec, usec, lasting);
}

void Lift::changePosition(float lasting)
{
  if (ABSF(dest - pos.z) <= 0.1) return;  // same level

  int sign = (dest > pos.z) ? 1 : -1;
  pos.z += sign * lasting * speed;
  if (state == CARRYING) {
    localuser->pos.z += sign * lasting * speed;	// user follows the lift
    //dax localuser->updatePositionAndGrid(localuser->pos);
    localuser->updatePosition();
    localuser->updateCamera(pos);
  }
}

void Lift::up()
{
  dest = MIN(pos.z + height, height * top);
  if ((pos.z + 0.1) >= top) return;
  initImposedMovement((dest - pos.z) / speed);
}

void Lift::down()
{
  dest = MAX(pos.z - height, height * bot);
  if ((pos.z - 0.1) <= bot) return;
  initImposedMovement((dest - pos.z) / speed);
}

void Lift::call()
{
  if (! state) {	// call only from outside
    dest = localuser->pos.z - localuser->height/2 - Lift::DEF_STEP;
    initImposedMovement((dest - pos.z) / speed);
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

/* somebody intersects */
bool Lift::whenIntersect(WObject *pcur, WObject *pold)
{
  switch (pcur->type) {
  case USER_TYPE:
    if (! state) {	// user comes in
      state = CARRYING;
      if (pcur->pos.z >= pold->pos.z) {	// same level
        pcur->pos.z += DEF_STEP;	// moving up
        pcur->updatePositionAndGrid(pold);
      }
      else {	// inside from up
        pold->copyPositionAndBB(pcur);
      }
    }
    else {	// user already inside
      pold->copyPositionAndBB(pcur);
    }
    break;
  default:
    pold->copyPositionAndBB(pcur);
    break;
  }
  return true;
}

bool Lift::whenIntersectOut(WObject *pcur, WObject *pold)
{
  if (state == CARRYING) {
    pcur->updatePositionAndGrid(pold);
    state = NONE;
  }
  return true;
}

void Lift::quit()
{
  oid = 0;
  flushMySqlPosition();
}

void Lift::funcs()
{
  getPropertyFunc(LIFT_TYPE, PROPZ, WO_PAYLOAD get_z);
  getPropertyFunc(LIFT_TYPE, PROPHNAME, WO_PAYLOAD get_hname);

  putPropertyFunc(LIFT_TYPE, PROPZ, WO_PAYLOAD put_z);
  putPropertyFunc(LIFT_TYPE, PROPHNAME, WO_PAYLOAD put_hname);

  setActionFunc(LIFT_TYPE, 0, WO_ACTION up_cb, "Up");
  setActionFunc(LIFT_TYPE, 1, WO_ACTION down_cb, "Down");
  setActionFunc(LIFT_TYPE, 2, WO_ACTION call_cb, "Call");
  setActionFunc(LIFT_TYPE, 3, WO_ACTION gotoFront, "Approach");
}
