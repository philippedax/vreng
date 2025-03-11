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
// wall.cpp
//
// Wall handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "wall.hpp"
#include "user.hpp"	// USER_TYPE
#include "ball.hpp"	// BALL_TYPE
#include "dart.hpp"	// DART_TYPE
#include "bullet.hpp"	// BULLET_TYPE
#include "thing.hpp"	// THING_TYPE
#include "icon.hpp"	// ICON_TYPE
#include "step.hpp"	// STEP_TYPE
#include "web.hpp"	// WEB_TYPE


const OClass Wall::oclass(WALL_TYPE, "Wall", Wall::creator);


/** creation from a file */
Object * Wall::creator(char *l)
{
  return new Wall(l);
}

void Wall::parser(char *l)
{
  l = tokenize(l);
  l = parseAttributes(l);
}

/** creation from vre file */
Wall::Wall(char *l)
{
  parser(l);

  mobileObject(0);
}

/** creation from gui addobject */
Wall::Wall(Object *user, char *geom)
{
  if (! user) return;

  setOwner();
  parseSolid(geom);

  enableBehavior(DYNAMIC);
  mobileObject(0);

  pos.x = user->pos.x + 0.7;	// in front of localuser
  pos.y = user->pos.y;
  pos.z = user->pos.z;
  updatePosition();
}

/** Intersection with an object */
bool Wall::intersect(Object *pcur, Object *pold)
{
  switch (pcur->type) {
  case USER_TYPE:
  case THING_TYPE:
  case BALL_TYPE:
    projectPosition(pcur, pold);
    break;
  case ICON_TYPE:
    // stick the icon on the wall
    doAction(ICON_TYPE, Icon::STICK, this, pcur, 0, 0);
    pold->copyPositionAndBB(pcur);
    break;
  case BULLET_TYPE:
  case DART_TYPE:
    pcur->toDelete();
    break;
  case STEP_TYPE:	// escalator
  case WEB_TYPE:
    return false;
  default:
    pold->copyPositionAndBB(pcur);
  }
  return true;
}

void Wall::destroy(Wall *po, void *d, time_t s, time_t u)
{
  if (po->isBehavior(DYNAMIC))
    po->removeFromScene();
}

void Wall::funcs()
{
  setActionFunc(WALL_TYPE, 0, _Action destroy, "Destroy");
}
