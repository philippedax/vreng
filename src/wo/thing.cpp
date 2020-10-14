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
#include "thing.hpp"
#include "world.hpp"	// current
#include "user.hpp"	// localuser
#include "move.hpp"	// GRAVITY
#include "mysql.hpp"	// deleteRow
#include "cart.hpp"	// Cart
#include "ball.hpp"	// BALL_TYPE
#include "pref.hpp"	// g.pref.user


const OClass Thing::oclass(THING_TYPE, "Thing", Thing::creator);
const float Thing::LSPEED = 0.25;	// 0.25 m/s

// local
static uint16_t oid = 0;


/* creation from a file */
WObject * Thing::creator(char *l)
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
  l = parse()->parseAttributes(l, this);
}

void Thing::behavior()
{
  enableBehavior(PERSISTENT);
  enableBehavior(COLLIDE_ONCE);
  enableBehavior(TAKABLE);	// cart
  setRenderPrior(RENDER_HIGH);

  initializeMobileObject(1);
  createPermanentNetObj(PROPS, oid);
}

/** Sets an unique name */
void Thing::setName()
{
  sprintf(names.named, "%s-%s.%d", names.type, localuser->getInstance(), getNum());
  updateNames();
}

/** Creation from XML file */
Thing::Thing(char *l)
{
  parser(l);
  behavior();
  oid++;
}

/** Created by user via the palette */
Thing::Thing(WObject *user, char *geom)
{
  defaults();
  setName();
  setOwner();
  oid++;

  /* position in front of the user */
  pos.x = user->pos.x + 1;
  pos.y = user->pos.y;
  pos.z = user->pos.z + 0.5;
  updatePosition();

  enableBehavior(DYNAMIC);	// dynamicaly introduced
  behavior();

#if HAVE_MYSQL
  if (! psql) psql = VRSql::getVRSql();
  if (psql && explicitName()) {
    psql->insertRow(this);
    psql->updatePos(this);
    psql->updateOwner(this);
    if (geom) {
      psql->updateGeom(this, geom);
      parse()->parseSolids(geom, SEP, this);
    }
  }
#endif
}

/** Regenerated by world via MySql */
Thing::Thing(World *pw, void *d, time_t s, time_t u)
{
  char *str = (char *) d;       // name transmitted
  if (!str) return;

  char *p = str;
  while (*p && !isdigit(*p)) p++;
  int n = atoi(p);
  if (n) oid = n;
  strcpy(names.named, str);
  strcpy(names.type, typeName());	// need names.type for MySql

  /* local creation */
  defaults();

#if HAVE_MYSQL
  // we don't know anything about the geometry except from MySql
  geometry = new char[BUFSIZ];
  if (! psql)
    psql = VRSql::getVRSql();
  if (psql && explicitName()) {
    psql->getGeom(this);
    psql->getOwner(this);
    psql->getPos(this);
  }
  if (geometry && isprint(*geometry)) {
    parse()->parseSolids(geometry, SEP, this);
  }
  else warning("Thing: %s has no geometry", names.named);
#endif

  enableBehavior(DYNAMIC);
  behavior();
}

void Thing::updateTime(time_t sec, time_t usec, float *lasting)
{
  updateLasting(sec, usec, lasting);
}

#if 0 //unused
void Thing::changePermanent(float lasting)
{
  static float lastz = 0;

  if (state == DROPPED) {
    trace(DBG_WO, "thingChangePermanent: x=%.2f y=%.2f z=%.2f ttl=%.2f lasting=%.2f", pos.x, pos.y, pos.z, move.ttl, lasting);
    lastz = pos.z;
    /* drop this thing by gravity */
    pos.z -= lasting * GRAVITY;

    /* test if finished */
    if (move.ttl == 0 /* || ABSF(lastz - pos.z) < 0.001 */) {
      state = NONE;
      lastz = 0;
    }
  }
}
#endif

bool Thing::updateToNetwork(const Pos &oldpos)
{
  return updatePosToNetwork(oldpos, PROPXY, PROPZ, PROPAZ, PROPAX, PROPAY);
}

void Thing::dropIntoBasket(Thing *thing, void *d, time_t s, time_t u)
{
#if HAVE_MYSQL
  if (thing->psql) {
    //FIXME: should'nt be deleted but marked as deleted
    //thing->psql->deleteRow(thing);	// delete from the current world
    thing->state = DELETED;
    thing->updateMySqlState(thing->state); // mark deleted
  }
#endif
  // then add to Basket
  //trace(DBG_FORCE, "addToCart: (%s,%s)", thing->getInstance(), pref->user);
  localuser->cart->addToCart(thing);
}

bool Thing::whenIntersect(WObject *pcur, WObject *pold)
{
  switch (pcur->type) {
    case USER_TYPE:
      // pushes the thing
      move.lspeed.v[0] = (lspeed / 2) * Cos(pcur->pos.az);
      move.lspeed.v[1] = (lspeed / 2) * Sin(pcur->pos.az);
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
  thing->removeFromScene();
}

void Thing::quit()
{
  oid = 0;
  flushMySqlPosition();
  //FIXME: flushMysqlGeom(), flushMySqlOwner
}

void Thing::funcs()
{
  getPropertyFunc(THING_TYPE, PROPXY, WO_PAYLOAD get_xy);
  getPropertyFunc(THING_TYPE, PROPZ, WO_PAYLOAD get_z);
  getPropertyFunc(THING_TYPE, PROPAZ, WO_PAYLOAD get_az);
  getPropertyFunc(THING_TYPE, PROPAX, WO_PAYLOAD get_ax);
  getPropertyFunc(THING_TYPE, PROPAY, WO_PAYLOAD get_ay);
  getPropertyFunc(THING_TYPE, PROPHNAME, WO_PAYLOAD get_hname);

  putPropertyFunc(THING_TYPE, PROPXY, WO_PAYLOAD put_xy);
  putPropertyFunc(THING_TYPE, PROPZ, WO_PAYLOAD put_z);
  putPropertyFunc(THING_TYPE, PROPAZ, WO_PAYLOAD put_az);
  putPropertyFunc(THING_TYPE, PROPAX, WO_PAYLOAD put_ax);
  putPropertyFunc(THING_TYPE, PROPAY, WO_PAYLOAD put_ay);
  putPropertyFunc(THING_TYPE, PROPHNAME, WO_PAYLOAD put_hname);

  setActionFunc(THING_TYPE, APPROACH, WO_ACTION gotoFront, "Approach");
  setActionFunc(THING_TYPE, MOVE, WO_ACTION moveObject, "Move");
  setActionFunc(THING_TYPE, BASKET, WO_ACTION dropIntoBasket, "Basket");
  setActionFunc(THING_TYPE, DESTROY, WO_ACTION destroy, "Destroy");
  setActionFunc(THING_TYPE, RECREATE, WO_ACTION recreate, "");
}
