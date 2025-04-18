//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2009 Philippe Dax
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
// projectile.cpp
//
// Projectile handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "projectile.hpp"
#include "matvec.hpp"   // V3 M4
#include "user.hpp"	// USER_TYPE
#include "netobj.hpp"   // netop


const OClass Projectile::oclass(PROJECTILE_TYPE, "Projectile", NULL);


void Projectile::timing(time_t s, time_t us, float *dt)
{
  if (! lasting(s, us, dt)) {
    /* the projectile has spent its time to live => it must be destroyed */
    toDelete();	// delete projectile
  }
}

/** Movement */
void Projectile::imposed(float dt)
{
  pos.x += dt * move.lspeed.v[0];
  pos.y += dt * move.lspeed.v[1];
  pos.z += dt * move.lspeed.v[2];
}

/** Updates to the network */
bool Projectile::publish(const Pos &oldpos)
{
  bool change = false;

  if ((pos.x != oldpos.x) || (pos.y != oldpos.y)) {
    netop->declareDelta(PROPXY);
    change = true;
  }
  return change;
}

/** When an intersetion occurs */
bool Projectile::intersect(Object *pcur, Object *pold)
{
  switch (pcur->type) {
  case USER_TYPE:
    // projectile intersects an user: hit
    echo("%s hited by %s", pcur->objectName(), objectName());
    pcur->pos = pold->pos;
    break;
  default:
    //dax pcur->pos = pold->pos;
    echo("tchok");
    break;
  }
  toDelete();	// delete projectile
  return true;
}

void Projectile::wallsIntersect(Object *pold, V3 *normal)
{
  enableBehavior(COLLIDE_ONCE);
  bounceTrajectory(pold, normal);
}

void Projectile::funcs()
{}
