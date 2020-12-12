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
#include "travelator.hpp"
#include "move.hpp"	// gotoFront


const OClass Travelator::oclass(TRAVELATOR_TYPE, "Travelator", Travelator::creator);

// local
static uint16_t oid = 0;


/* creation from a file */
WObject * Travelator::creator(char *l)
{
  return new Travelator(l);
}

void Travelator::defaults()
{
  length = 0;
  dir = 0;
  speed = Step::LSPEED;
  mobile = true;
}

void Travelator::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if      (! stringcmp(l, "length")) l = parse()->parseFloat(l, &length, "length");
    else if (! stringcmp(l, "speed"))  l = parse()->parseFloat(l, &speed, "speed");
  }
  end_while_parse(l);
}

void Travelator::build()
{
  float sx = pos.bbsize.v[0];  // step width
  float sy = pos.bbsize.v[1];  // step depth
  float ss = MIN(sx, sy);

  nsteps = (int) ceil(length / ss);
  trace(DBG_WO, "travellator: nsteps = %d", nsteps);

  for (int n=0; n <= nsteps; n++) {
    Pos newpos;
    newpos.az = pos.az;
    newpos.ax = newpos.ay = 0;
    //FIXME: wrong position
    newpos.x = pos.x - (sin(pos.az) * sx * n);
    newpos.y = pos.y - (cos(pos.az) * sy * n);
    newpos.z = pos.z;

    nextstep = new Step(newpos, pos, geometry, mobile, length, speed, dir);
  }

  disableBehavior(PERSISTENT);
  enablePermanentMovement(speed);
}

void Travelator::behavior()
{
  enableBehavior(PERSISTENT);
  setRenderPrior(PRIOR_MEDIUM);

  initMobileObject(1);
  createPermanentNetObject(PROPS, ++oid);
}

Travelator::Travelator(char *l)
{
  parser(l);
  behavior();
  build();
}

void Travelator::quit()
{
  oid = 0;
  clearList();
  savePersistency();
}

void Travelator::funcs()
{
  setActionFunc(TRAVELATOR_TYPE, 0, WO_ACTION gotoFront, "Approach");
  setActionFunc(TRAVELATOR_TYPE, 1, WO_ACTION pause_cb, "Pause/Continue");
  setActionFunc(TRAVELATOR_TYPE, 2, WO_ACTION stop_cb, "Stop/Restart");
}
