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
// travolator.cpp
//
// Travolator handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "travolator.hpp"
#include "move.hpp"	// gotoFront


const OClass Travolator::oclass(TRAVOLATOR_TYPE, "Travolator", Travolator::creator);

// local
static uint16_t oid = 0;


/* creation from a file */
Object * Travolator::creator(char *l)
{
  return new Travolator(l);
}

void Travolator::defaults()
{
  on = true;
  length = 0;
  dir = 0;		// horizontal
  speed = Step::LSPEED;
  mobile = true;
  stuck = false;
}

void Travolator::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if      (! stringcmp(l, "length")) l = parseFloat(l, &length, "length");
    else if (! stringcmp(l, "speed"))  l = parseFloat(l, &speed, "speed");
    else if (! stringcmp(l, "on"))     l = parseBool(l, &on, "on");
  }
  end_while_parse(l);
}

void Travolator::build()
{
  uint8_t nsteps = 0;
  float sx = 2 * pos.bbs.v[0];  // step width
  float sy = 2 * pos.bbs.v[1];  // step depth

  nsteps = int( ceil(length / MIN(sx, sy)) );

  for (int n=0; n < nsteps; n++) {
    Pos npos = pos;
    npos.x = pos.x - sin(pos.az) * (sx * n);
    npos.y = pos.y - cos(pos.az) * (sy * n);

    Step *step = new Step(npos, pos, objectName(), geomsolid, true, length, speed, 0);
    travList.push_back(step);
  }
  permanentMovement(speed);
}

void Travolator::inits()
{
  mobileObject(1);
}

Travolator::Travolator(char *l)
{
  travList.clear();
  parser(l);
  inits();
  build();
  if (on) {
    running();
  }
}

void Travolator::quit()
{
  travList.clear();
  oid = 0;
}

void Travolator::funcs()
{
  setActionFunc(TRAVOLATOR_TYPE, 0, _Action pause_cb, "Pause/Continue");
  setActionFunc(TRAVOLATOR_TYPE, 1, _Action gotoFront, "Approach");
}
