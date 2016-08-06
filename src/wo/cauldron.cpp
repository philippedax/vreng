//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2009 Philippe Dax
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
#include "cauldron.hpp"
#include "user.hpp"	// USER_TYPE
#include "ball.hpp"	// BALL_TYPE
#include "water.hpp"	// WATER_TYPE
#include "step.hpp"	// STEP_TYPE
#include "move.hpp"	// GRAVITY


const OClass Cauldron::oclass(CAULDRON_TYPE, "Cauldron", Cauldron::creator);
const float Cauldron::DELTAZ = 0.02;	// 2 cm


/* creation from a file */
WObject * Cauldron::creator(char *l)
{
  return new Cauldron(l);
}

void Cauldron::parser(char *l)
{
  l = tokenize(l);
  l = parse()->parseAttributes(l, this);
}

Cauldron::Cauldron(char *l)
{
  parser(l);

  setRenderPrior(RENDER_HIGH);

  initializeMobileObject(MAXFLOAT);
  enablePermanentMovement();
}

void Cauldron::changePermanent(float lasting)
{
  pos.z -= lasting * GRAVITY / 4;
}

bool Cauldron::whenIntersect(WObject *pcur, WObject *pold)
{
  switch (pcur->type) {
  case USER_TYPE:
    pcur->pos.z += DELTAZ;
    break;
  case BALL_TYPE:
    srand((uint32_t) time(NULL));
    if (pcur->pos.z > pos.z + DELTAZ) {
      pcur->pos.x += ((float) drand48() * 2 -1) * Ball::RADIUS / 2;
      pcur->pos.y += ((float) drand48() * 2 -1) * Ball::RADIUS / 2;
    }
    else pcur->pos.z += DELTAZ;	// jump upon cauldron
    break;
  case STEP_TYPE:
    return false;
  default:
    pold->copyPositionAndBB(pcur);
    return true;
  }
  pcur->updatePositionAndGrid(pold);
  return true;
}

/* Creates a ball */
void Cauldron::create_cb(Cauldron *cauldron, void *data, time_t sec, time_t usec)
{
  if (isAction(BALL_TYPE, Ball::CREATE))
    doAction(BALL_TYPE, Ball::CREATE, cauldron, data, sec, usec);
}

void Cauldron::funcs()
{
  setActionFunc(CAULDRON_TYPE, 0, WO_ACTION create_cb, "New ball");
}
