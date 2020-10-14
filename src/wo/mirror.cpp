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
#include "mirror.hpp"
#include "user.hpp"	// USER_TYPE


const OClass Mirror::oclass(MIRROR_TYPE, "Mirror", Mirror::creator);

// local
static uint16_t oid = 0;


WObject * Mirror::creator(char *l)
{
  return new Mirror(l);
}

void Mirror::defaults()
{
  state = false;
}

void Mirror::behavior()
{
  enableBehavior(PERSISTENT);
  enableBehavior(COLLIDE_ONCE);

  initializeMobileObject(0);
  createPermanentNetObj(PROPS, ++oid);

  setReflexive(state);
}

void Mirror::parser(char *l)
{
  defaults();
  l = tokenize(l);
  l = parse()->parseAttributes(l, this);
}

Mirror::Mirror(char *l)
{
  parser(l);
  behavior();
}

bool Mirror::updateToNetwork(const Pos &oldpos)
{
  return updatePosToNetwork(oldpos, PROPXY, PROPZ, PROPAZ, PROPAX, PROPAY);
}

bool Mirror::whenIntersect(WObject *pcur, WObject *pold)
{
  switch (pcur->type) {
  case USER_TYPE:
    projectPosition(pcur, pold);
    break;
  default:
    pold->copyPositionAndBB(pcur);
  }
  return true;
}

void Mirror::quit()
{
  oid = 0;
  flushMySqlPosition();
}

void Mirror::mirrorOn(Mirror *po, void *d, time_t s, time_t u)
{
  po->setReflexive(true);
}

void Mirror::mirrorOff(Mirror *po, void *d, time_t s, time_t u)
{
  po->setReflexive(false);
}

void Mirror::funcs()
{
  setActionFunc(MIRROR_TYPE, 0, WO_ACTION mirrorOn, "Switch-On");
  setActionFunc(MIRROR_TYPE, 1, WO_ACTION mirrorOff, "Switch-Off");
  setActionFunc(MIRROR_TYPE, 2, WO_ACTION gotoFront, "Approach");
}
