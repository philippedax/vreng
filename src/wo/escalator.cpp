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
#include "escalator.hpp"
#include "move.hpp"	// gotoFront


const OClass Escalator::oclass(ESCALATOR_TYPE, "Escalator", Escalator::creator);

// local
static uint16_t oid = 0;


/* creation from a file */
WObject * Escalator::creator(char *l)
{
  return new Escalator(l);
}

void Escalator::defaults()
{
  dir = 1;	// up by default
  height = 0;
  length = 0;
  speed = Step::LSPEED;
  mobile = true;
}

void Escalator::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if (! stringcmp(l, "dir=")) {
      char modestr[16];
      l = parse()->parseString(l, modestr, "dir");
      if      (! stringcmp(modestr, "up"))   dir = 1;
      else if (! stringcmp(modestr, "down")) dir = -1;
    }
    else if (! stringcmp(l, "height")) l = parse()->parseFloat(l, &height, "height");
    else if (! stringcmp(l, "length")) l = parse()->parseFloat(l, &length, "length");
    else if (! stringcmp(l, "speed"))  l = parse()->parseFloat(l, &speed, "speed");
  }
  end_while_parse(l);
}

void Escalator::build()
{
  float sx = pos.bbsize.v[0];  // step width
  float sy = pos.bbsize.v[1];  // step depth
  float sz = pos.bbsize.v[2];  // step height

  if (height && dir > 0) height += sz;  // add the top step
  else if (length && pos.ax) {  // escalator defined by its length and its angle
    height = length * tan(pos.ax);
    pos.ax = 0;
  }

  nsteps = (int) ceil(height / sz);

  for (int n=0; n <= nsteps; n++) {
    Pos newpos;
    newpos.az = pos.az;
    newpos.ax = newpos.ay = 0;
    newpos.x = pos.x + dir*(sin(pos.az) * sx * n);
    newpos.y = pos.y + dir*(cos(pos.az) * sy * n);
    newpos.z = pos.z + dir*(sz * n);

    nextstep = new Step(newpos, pos, geometry, mobile, height, speed, dir);
  }

  enablePermanentMovement(speed);
}

void Escalator::behavior()
{
  initMobileObject(1);
  createPermanentNetObject(PROPS, ++oid);
}

Escalator::Escalator(char *l)
{
  parser(l);
  behavior();
  build();
}

void Escalator::quit()
{
  oid = 0;
}

void Escalator::funcs()
{
  setActionFunc(ESCALATOR_TYPE, 0, WO_ACTION gotoFront, "Approach");
  setActionFunc(ESCALATOR_TYPE, 1, WO_ACTION pause_cb, "Pause/Continue");
  setActionFunc(ESCALATOR_TYPE, 2, WO_ACTION stop_cb, "Stop/Restart");
}
