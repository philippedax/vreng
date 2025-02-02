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
// ground.cpp
//
// Ground handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "ground.hpp"
#include "world.hpp"	// setGround
#include "user.hpp"	// localuser


const OClass Ground::oclass(GROUND_TYPE, "Ground", Ground::creator);


/* creation from a file */
Object * Ground::creator(char *l)
{
  return new Ground(l);
}

void Ground::parser(char *l)
{
  l = tokenize(l);
  l = parseAttributes(l);
}

Ground::Ground(char *l)
{
  parser(l);

  stillObject();
  World::current()->setGround(pos.z + pos.bbs.v[2]);
  //echo("ground: z=%.2f bb=%.2f gr=%.2f",pos.z,pos.bbs.v[2],pos.z+pos.bbs.v[2]);
}

Ground::Ground(Object *user, char *geom)
{
  parseSolid(geom);

  mobileObject(0);

  pos.x = user->pos.x;
  pos.y = user->pos.y;
  pos.z = user->pos.z - localuser->height/2 - 0.1;
  updatePosition();
}

/* Intersection with an object */
bool Ground::whenIntersect(Object *pcur, Object *pold)
{
  pold->setLasting(0);
  pold->disablePermanentMovement();
  pold->copyPositionAndBB(pcur);
  return true;
}

void Ground::quit()
{}

void Ground::funcs()
{}
