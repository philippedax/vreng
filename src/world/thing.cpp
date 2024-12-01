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
// thing.cpp
//
// Thing handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "thing.hpp"
#include "world.hpp"	// current
#include "user.hpp"	// localuser
#include "move.hpp"	// GRAVITY
#include "sql.hpp"	// deleteRow
#include "cart.hpp"	// Cart
#include "ball.hpp"	// BALL_TYPE
#include "pref.hpp"	// g.pref.user


const OClass Thing::oclass(THING_TYPE, "Thing", Thing::creator);
const float Thing::LSPEED = 0.25;	// 0.25 m/s

// local
static uint16_t oid = 0;


/* creation from a file */
WO * Thing::creator(char *l)
{
  return new Thing(l);
}

void Thing::defaults()
{
  lspeed = LSPEED;
  state = NONE;
}

void Thing::parser(char *l)
{
  defaults();
  l = tokenize(l);
  l = parseAttributes(l);
}

void Thing::behaviors()
{
  enableBehavior(PERSISTENT);
  enableBehavior(COLLIDE_ONCE);
  enableBehavior(TAKABLE);	// cart
}

/** Sets an unique name */
void Thing::setName()
{
  if (! name.given)
    name.given = new char[OBJNAME_LEN];
  sprintf(name.given, "%s-%s%d", THING_NAME, ::g.user, getNum());
  updateNames();
}

/** Creation from XML file */
Thing::Thing(char *l)
{
  parser(l);
  behaviors();

  initMobileObject(1);
  createNetObj(PROPS, ++oid);
}

/** Created by user via the Gui addobj */
Thing::Thing(WO *user, char *_geom)
{
  defaults();
  setName();
  setOwner();

  parseSolid(_geom);

  /* position in front of the user */
  pos.x = user->pos.x + 1;
  pos.y = user->pos.y;
  pos.z = user->pos.z + 0.5;
  updatePosition();

  enableBehavior(DYNAMIC);	// dynamicaly introduced
  behaviors();

  if (! vsql) vsql = new VSql();
  if (vsql) {
    vsql->insertRow(this);
  }

  initMobileObject(1);
  createNetObj(PROPS, ++oid);
}

/** Regenerated by world via VSql */
Thing::Thing(World *pw, void *d, time_t s, time_t u)
{
  char *str = static_cast<char *>(d);       // name transmitted
  if (!str) return;

  char *p = str;
  while (*p && !isdigit(*p)) p++;
  int n = atoi(p);
  if (n) oid = n;
  strcpy(name.given, str);
  strcpy(name.type, typeName());	// need name.type for VSql

  /* local creation */
  defaults();

  // we don't know anything about the geometry except from VSql
  char *geom = new char[256];
  if (! vsql) vsql = new VSql();
  if (vsql) {
    vsql->getGeom(this);
    vsql->getOwner(this);
    vsql->getPos(this);
  }
  if (geom && isprint(*geom)) {
    parseSolid(geom);
  }
  else error("thing: %s has no geometry", name.given);

  enableBehavior(DYNAMIC);
  behaviors();

  initMobileObject(1);
  createNetObj(PROPS, oid);
}

void Thing::updateTime(time_t sec, time_t usec, float *lasting)
{
  updateLasting(sec, usec, lasting);
}

void Thing::changePermanent(float lasting)
{
  if (state == DROPPED) {
    trace1(DBG_WO, "thingChangePermanent: x=%.2f y=%.2f z=%.2f ttl=%.2f lasting=%.2f", pos.x, pos.y, pos.z, move.ttl, lasting);
    /* drop this thing by gravity */
    pos.z -= lasting * GRAVITY;

    /* test if finished */
    if (move.ttl == 0) {
      state = NONE;
    }
  }
}

bool Thing::publish(const Pos &oldpos)
{
  return publishPos(oldpos, PROPXY, PROPZ, PROPAZ, PROPAX, PROPAY);
}

void Thing::dropIntoBasket(Thing *thing, void *d, time_t s, time_t u)
{
  if (thing->vsql) {
    //FIXME: should'nt be deleted but marked as deleted
    //thing->vsql->deleteRow(thing);	// delete from the current world
    thing->state = DELETED;
    thing->updatePersist(); // mark deleted
  }
  // then add to Basket
  //trace1(DBG_FORCE, "addToCart: (%s,%s)", thing->objectName(), pref->user);
  localuser->cart->addToCart(thing);
}

bool Thing::whenIntersect(WO *pcur, WO *pold)
{
  switch (pcur->type) {
    case USER_TYPE:
      // pushes the thing
      move.lspeed.v[0] = (lspeed / 2) * cos(pcur->pos.az);
      move.lspeed.v[1] = (lspeed / 2) * sin(pcur->pos.az);
      initImposedMovement(1);	// 1 sec
      break;
    default:
      pold->copyPositionAndBB(pcur);
      break;
  }
  return true;
}

/** Creation: this method is invisible: called by the World */
void Thing::recreate(World *w, void *d, time_t s, time_t u)
{
  new Thing(w, d, s, u);
}

void Thing::destroy(Thing *thing, void *d, time_t s, time_t u)
{
  if (thing->isBehavior(DYNAMIC))
    thing->removeFromScene();
}

void Thing::quit()
{
  oid = 0;
  savePersist();
  //FIXME: flushVRsqlGeom(), flushVSqlOwner
}

void Thing::funcs()
{
  getPropFunc(THING_TYPE, PROPXY, _Payload get_xy);
  getPropFunc(THING_TYPE, PROPZ, _Payload get_z);
  getPropFunc(THING_TYPE, PROPAZ, _Payload get_az);
  getPropFunc(THING_TYPE, PROPAX, _Payload get_ax);
  getPropFunc(THING_TYPE, PROPAY, _Payload get_ay);
  getPropFunc(THING_TYPE, PROPHNAME, _Payload get_hname);

  putPropFunc(THING_TYPE, PROPXY, _Payload put_xy);
  putPropFunc(THING_TYPE, PROPZ, _Payload put_z);
  putPropFunc(THING_TYPE, PROPAZ, _Payload put_az);
  putPropFunc(THING_TYPE, PROPAX, _Payload put_ax);
  putPropFunc(THING_TYPE, PROPAY, _Payload put_ay);
  putPropFunc(THING_TYPE, PROPHNAME, _Payload put_hname);

  setActionFunc(THING_TYPE, APPROACH, _Action gotoFront, "Approach");
  setActionFunc(THING_TYPE, MOVE, _Action moveObject, "Move");
  setActionFunc(THING_TYPE, BASKET, _Action dropIntoBasket, "Basket");
  setActionFunc(THING_TYPE, DESTROY, _Action destroy, "Destroy");
  setActionFunc(THING_TYPE, RECREATE, _Action recreate, "");
}
