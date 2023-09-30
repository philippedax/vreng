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
WO * Travelator::creator(char *l)
{
  return new Travelator(l);
}

void Travelator::defaults()
{
  on = true;
  length = 0;
  dir = 0;		// horizontal
  speed = Step::LSPEED;
  mobile = true;
}

void Travelator::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if      (! stringcmp(l, "length")) l = parseFloat(l, &length, "length");
    else if (! stringcmp(l, "speed"))  l = parseFloat(l, &speed, "speed");
    else if (! stringcmp(l, "state"))  l = parseBool(l, &on, "state");
  }
  end_while_parse(l);
}

void Travelator::build()
{
  float sx = 2 * pos.bbs.v[0];  // step width
  float sy = 2 * pos.bbs.v[1];  // step depth

  nsteps = (int) ceil(length / MIN(sx, sy));

  travList.push_back(this);

  for (int n=0; n < nsteps; n++) {
    Pos newpos;
    newpos.az = pos.az;
    newpos.ax = pos.ax;
    newpos.ay = pos.ay;
    newpos.x = pos.x - sin(pos.az) * (sx * n);
    newpos.y = pos.y - cos(pos.az) * (sy * n);
    newpos.z = pos.z;

    nextstep = new Step(newpos, pos, "travelator", geomsolid, true, length, speed, dir);
    travList.push_back(nextstep);
  }

  enablePermanentMovement(speed);
}

void Travelator::inits()
{
  initMobileObject(1);
  createPermanentNetObject(PROPS, ++oid);
}

Travelator::Travelator(char *l)
{
  travList.clear();
  parser(l);
  inits();
  build();
  if (on)
    running();
}


void Travelator::quit()
{
  travList.clear();
  oid = 0;
}

void Travelator::funcs()
{
  setActionFunc(TRAVELATOR_TYPE, 0, _Action pause_cb, "Pause/Continue");
  setActionFunc(TRAVELATOR_TYPE, 1, _Action gotoFront, "Approach");
}
