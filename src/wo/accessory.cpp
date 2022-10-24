//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2012 Philippe Dax
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
#include "accessory.hpp"
#include "user.hpp"	// localuser


const OClass Accessory::oclass(ACCESSORY_TYPE, "Accessory", Accessory::creator);

const float Accessory::LSPEED = 0.5; // 1/2 ms

// local
static uint16_t oid = 0;


/* creation from a file */
WObject * Accessory::creator(char *l)
{
  return new Accessory(l);
}

void Accessory::defaults()
{
  speed = LSPEED;
  slowx = slowy = slowz = 50;
  shiftx = shifty = shiftz = 0;
}

void Accessory::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if      (!stringcmp(l, "speed")) l = parse()->parseFloat(l, &speed, "speed");
    else if (!stringcmp(l, "slowx")) l = parse()->parseFloat(l, &slowx, "slowx");
    else if (!stringcmp(l, "slowy")) l = parse()->parseFloat(l, &slowy, "slowy");
    else if (!stringcmp(l, "slowz")) l = parse()->parseFloat(l, &slowz, "slowz");
    else if (!stringcmp(l, "shiftx")) l = parse()->parseFloat(l, &shiftx, "shiftx");
    else if (!stringcmp(l, "shifty")) l = parse()->parseFloat(l, &shifty, "shifty");
    else if (!stringcmp(l, "shiftz")) l = parse()->parseFloat(l, &shiftz, "shiftz");
  }
  end_while_parse(l);
}

void Accessory::behavior()
{
  //dax1 enableBehavior(PERSISTENT);
}

void Accessory::init()
{
  dx = dy = dz = 0;
  dax = day = daz = 0;
  ttl = MAXFLOAT;
  following = false;

  initMobileObject(1);
  enablePermanentMovement(speed);
  createPermanentNetObject(PROPS, ++oid);
}

Accessory::Accessory(char *l)
{
  parser(l);
  behavior();
  init();
}

void Accessory::changePermanent(float lasting)
{
  if (! following)  return;

  // distances
  float distx = localuser->pos.x + shiftx - pos.x;
  float disty = localuser->pos.y + shifty - pos.y;
  float distz = localuser->pos.z + shiftz - pos.z;
  float dx = ABSF(distx / slowx);
  float dy = ABSF(disty / slowy);
  float dz = ABSF(distz / slowz);

  // progression
  if (distx > 0)
    pos.x += dx;
  else
    pos.x -= dx;
  if (disty > 0)
    pos.y += dy;
  else
    pos.y -= dy;
  if (distz > 0)
    pos.z += dz;
  else
    pos.z -= dz;

  updatePosition();
}

void Accessory::quit()
{
  oid = 0;
  savePersistency();
}

/* wear */
void Accessory::follow()
{
  if (following)  return;

  following = true;
  // save initial position
  ox = pos.x; oy = pos.y; oz = pos.z;
  oax = pos.ax; oay = pos.ay; oaz = pos.az;
}

/* takeoff */
void Accessory::takeoff()
{
  if (! following)  return;

  following = false;
  restorePosition();  // restore original position
}

/* drop */
void Accessory::drop()
{
  if (! following)  return;

  following = false;
}

/* follow_cb: indirectly called by user */
void Accessory::follow_cb(Accessory *accessory, void *d, time_t s, time_t u)
{
  accessory->follow();
}

/* takeoff_cb: indirectly called by user */
void Accessory::takeoff_cb(Accessory *accessory, void *d, time_t s, time_t u)
{
  accessory->takeoff();
}

/* drop_cb: indirectly called by user */
void Accessory::drop_cb(Accessory *accessory, void *d, time_t s, time_t u)
{
  accessory->drop();
}

void Accessory::funcs()
{
  setActionFunc(ACCESSORY_TYPE, 0, _Action follow_cb, "Follow");
  setActionFunc(ACCESSORY_TYPE, 1, _Action takeoff_cb, "Takeoff");
  setActionFunc(ACCESSORY_TYPE, 2, _Action drop_cb, "Drop");
}
