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
#include "mirage.hpp"
#include "user.hpp"	// user
#include "move.hpp"	// gotoFront
#include "mysql.hpp"	// VRSql
#include "timer.hpp"	// rate
#include "flare.hpp"	// Flare
#include "solid.hpp"	// setFlary


const OClass Mirage::oclass(MIRAGE_TYPE, "Mirage", Mirage::creator);

const float Mirage::ASPEED = -0.2;

// local
static float x0x0y0y0, maxx, maxy, minx, miny;


WObject * Mirage::creator(char *l)
{
  return new Mirage(l);
}

void Mirage::defaults()
{
  turn     = false;
  roll     = false;
  tilt     = false;
  orbit    = false;
  circular = false;
  persist  = true;
  aspeed   = ASPEED;
  azspeed  = ASPEED;
  ayspeed  = ASPEED;
  axspeed  = ASPEED;
  centrex  = 0.0;
  centrey  = 0.0;
  anglerot = 0.0;
  radius   = 0.0;
  flares   = 0;
  scale    = 1;
  for (int i=0; i<3 ; i++) color[i] = 1;
}

void Mirage::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse (l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if (!stringcmp(l, "aspeed")) {
      l = parse()->parseFloat(l, &aspeed, "aspeed");
      azspeed = ayspeed = axspeed = aspeed;
    }
    else if (!stringcmp(l, "azspeed")) l = parse()->parseFloat(l, &azspeed, "azspeed");
    else if (!stringcmp(l, "ayspeed")) l = parse()->parseFloat(l, &ayspeed, "ayspeed");
    else if (!stringcmp(l, "axspeed")) l = parse()->parseFloat(l, &axspeed, "axspeed");
    else if (!stringcmp(l, "radius")) l = parse()->parseFloat(l, &radius, "radius");
    else if (!stringcmp(l, "flare")) l = parse()->parseUInt8(l, &flares, "flare");
    else if (!stringcmp(l, "scale")) l = parse()->parseFloat(l, &scale, "scale");
    else if (!stringcmp(l, "color")) l = parse()->parseVector3f(l, color, "color");
    else if (!stringcmp(l, "mode=")) {
      char modestr[16];
      l = parse()->parseString(l, modestr, "mode");
      if (!stringcmp(modestr, "turn") || !stringcmp(modestr, "self")) turn = true;
      if (!stringcmp(modestr, "roll")) roll = true;
      if (!stringcmp(modestr, "tilt")) tilt = true;
      else if (!stringcmp(modestr, "refresh") || !stringcmp(modestr, "volatile")) persist = false;
      else if (!stringcmp(modestr, "orbit")) {
        orbit = true;
        x0x0y0y0 = pos.x*pos.x + pos.y*pos.y;
        maxx = maxy = (float) sqrt(x0x0y0y0);
        minx = miny = -maxx;
        persist = false;
      }
      else if (!stringcmp(modestr, "circular")) {
        circular = true;
        centrex = pos.x;
        centrey = pos.y;
        initialaz = pos.az;
        anglerot = pos.az;
        persist = false;
      }
    }
  }
  end_while_parse(l);
}

/** Sets name */
void Mirage::setName()
{
  sprintf(names.given, "%s-%s.%d", MIRAGE_NAME, localuser->getInstance(), getNum());
  updateNames();
}

void Mirage::behavior()
{
  if (persist) enableBehavior(PERSISTENT);
  enableBehavior(COLLIDE_NEVER);
  setRenderPrior(PRIOR_MEDIUM);

  initMobileObject(1);
  if (turn || roll || tilt || orbit || circular || flares)
    enablePermanentMovement();
}

/** Creation from xml file */
Mirage::Mirage(char *l)
{
  parser(l);
  behavior();

  if (flares && solid) {
    flare = new Flare(flares, scale, color);
    solid->setFlary(true);
  }
}

/** Created by user via Gui addobj */
Mirage::Mirage(WObject *user, char *geom)
{
  defaults();
  setName();
  setOwner();

#if HAVE_MYSQL
  psql = VRSql::getVRSql();
  if (psql && explicitName()) {
    psql->insertRow(this);
    psql->updatePos(this);
    psql->updateGeom(this, geom);
    psql->updateOwner(this);
  }
#endif
  parse()->parseSolids(geom, SEP, this);

  behavior();
  enableBehavior(DYNAMIC);	// addobj

  /* position */
  pos.x = user->pos.x + 0.7;
  pos.y = user->pos.y;
  pos.z = user->pos.z + 0.5;
  updatePosition();
}

/** Recreated by world via MySql */
Mirage::Mirage(World *pw, void *d, time_t s, time_t u)
{
  char *str = (char *) d;       // string
  if (!str) return;

  strcpy(names.given, str);
  strcpy(names.type, typeName());     // need names.type for MySql

  /* local creation */
  defaults();

  // we don't know anything about the geometry except from MySql
  geometry = new char[BUFSIZ];
#if HAVE_MYSQL
  psql = VRSql::getVRSql();
  if (psql && explicitName()) {
    psql->getGeom(this);
    psql->getOwner(this);
    psql->getPos(this);
  }
#endif
  if (geometry && isprint(*geometry)) {	//FIXME: when object comes from Cart
    parse()->parseSolids(geometry, SEP, this);
  }
  else warning("Mirage: %s no geometry available", names.given);

  enableBehavior(DYNAMIC);  // palette
  behavior();
}

/* system of equations handling permanent motion */
void Mirage::changePermanent(float lasting)
{
  if (turn) {
    pos.az -= lasting * azspeed;
    pos.az -= M_2PI * (float) floor(pos.az / M_2PI);
  }
  if (roll) {
    pos.ay -= lasting * ayspeed;
    pos.ay -= M_2PI * (float) floor(pos.ay / M_2PI);
  }
  if (tilt) {
    pos.ax -= lasting * axspeed;
    pos.ax -= M_2PI * (float) floor(pos.ax / M_2PI);
  }
  if (orbit) {
    float sx, sy, dx, dy;
    if (pos.x >= maxx || pos.y <= miny) {
      sx = +1; sy = +1;
    }
    else if (pos.y >= maxy || pos.x <= minx) {
      sx = -1; sy = -1;
    }
    else if ((pos.y >= 0 && pos.x <= 0) || (pos.y <= 0 && pos.x <= 0)) {
      sx = -1; sy = -1;
    }
    else {
      sx = +1; sy = +1;
    }
    dy = sy / ::g.timer.rate();
    pos.y += dy;
    dx = (float) sqrt(fabs(x0x0y0y0 - pos.y*pos.y));
    pos.x = sx * dx;
  }
  if (circular) {
    anglerot -= (lasting * aspeed);
    anglerot -= (M_2PI * (float) floor(anglerot / M_2PI));
    pos.x = centrex + (radius * cos(anglerot));
    pos.y = centrey + (radius * sin(anglerot));
    pos.az = anglerot + initialaz;
  }
  updatePosition();
}

void Mirage::quit()
{
  if (persist) savePersistency();
}

void Mirage::recreate(World *w, void *d, time_t s, time_t u)
{
  new Mirage(w, d, s, u);
}

void Mirage::destroy(Mirage *mirage, void *d, time_t s, time_t u)
{
  if (mirage->isBehavior(DYNAMIC))
    mirage->removeFromScene();
}

void Mirage::funcs()
{
  setActionFunc(MIRAGE_TYPE, APPROACH, WO_ACTION gotoFront, "Approach");
  setActionFunc(MIRAGE_TYPE, MOVE, WO_ACTION moveObject, "Move");
  setActionFunc(MIRAGE_TYPE, DESTROY, WO_ACTION destroy, "Destroy");
  setActionFunc(MIRAGE_TYPE, RECREATE, WO_ACTION recreate, "");
}
