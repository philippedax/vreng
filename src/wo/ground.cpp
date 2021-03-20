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
#include "ground.hpp"
#include "world.hpp"	// setGround
#include "user.hpp"	// USER_TYPE
#include "ball.hpp"	// BALL_TYPE
#include "cauldron.hpp"	// CAULDRON_TYPE
#include "step.hpp"	// STEP_TYPE
#include "guide.hpp"	// GUIDE_TYPE


const OClass Ground::oclass(GROUND_TYPE, "Ground", Ground::creator);


/* creation from a file */
WObject * Ground::creator(char *l)
{
  return new Ground(l);
}

void Ground::parser(char *l)
{
  l = tokenize(l);
  l = parse()->parseAttributes(l, this);
}

Ground::Ground(char *l)
{
  parser(l);

  initStillObject();
  //dax pos.z = 0;
  World::current()->setGround(pos.z + pos.bbsize.v[2]);
  //dax error("ground: z=%.2f bb=%.2f gr=%.2f",pos.z,pos.bbsize.v[2],pos.z+pos.bbsize.v[2]);
}

Ground::Ground(WObject *user, char *geom)
{
  parse()->parseSolid(geom, SEP, this);

  initMobileObject(0);

  pos.x = user->pos.x;
  pos.y = user->pos.y;
  pos.z = user->pos.z - localuser->height/2 - 0.1;
  updatePosition();
}

/* Intersection with an object */
bool Ground::whenIntersect(WObject *pcur, WObject *pold)
{
  switch (pcur->type) {

  case BALL_TYPE:
    pold->setLasting(0);
    pold->disablePermanentMovement();
    pold->copyPositionAndBB(pcur);
#if 0
    if (pcur->origz - pcur->pos.z > BALL_RADIUS * 2) {
      pcur->pos.z = MAX((pcur->origz / 4), BALL_RADIUS * 2);
      pcur->origz = pcur->pos.z;
      pcur->ttl /= 4;
      pcur->updatePositionAndGrid(pold);
    }
#endif
    break;

  case CAULDRON_TYPE:
    pold->setLasting(0);
    pold->disablePermanentMovement();
    pold->copyPosAndBB(pcur->pos);
    break;

  case STEP_TYPE:
  case GUIDE_TYPE:
    return false;

  default:
    pold->setLasting(0);
    pold->disablePermanentMovement();
    pold->copyPosAndBB(pcur->pos);
  }
  return true;
}

void Ground::quit()
{
  savePersistency();
}

void Ground::funcs()
{
#if 0
  setActionFunc(GROUND_TYPE, 0, WO_ACTION moveObject, "Move");
#endif
}
