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
#include "stair.hpp"
#include "move.hpp"	// gotoFront


const OClass Stair::oclass(STAIR_TYPE, "Stair", Stair::creator);

// local
static uint16_t oid = 0;


/* creation from a file */
WObject * Stair::creator(char *l)
{
  return new Stair(l);
}

void Stair::defaults()
{
  dir = 1;
  height = 0;
  length = 0;
  mobile = false;
}

void Stair::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if      (! stringcmp(l, "height")) l = parse()->parseFloat(l, &height, "height");
    else if (! stringcmp(l, "length")) l = parse()->parseFloat(l, &length, "length");
  }
  end_while_parse(l);
}

void Stair::build()
{
  float sx = pos.bbs.v[0];  // step width
  float sy = pos.bbs.v[1];  // step depth
  float sz = MIN(sx, sy);

  if (height) height += sz;	// add the top step
  else if (length && pos.ax) {  // stair defined by its length and its angle
    height = length * tan(pos.ax);
    pos.ax = 0;
  }

  nsteps = (int) ceil(height / sz);

  for (int n=0; n <= nsteps; n++) {
    Pos newpos;
    newpos.az = pos.az;
    newpos.ax = newpos.ay = 0;
    newpos.x = pos.x + (sin(pos.az) * sx * n);
    newpos.y = pos.y + (cos(pos.az) * sy * n);
    newpos.z = pos.z + (sz * n);

    nextstep = new Step(newpos, pos, geometry, false, height, 0, dir);
  }
}

void Stair::behavior()
{
  initMobileObject(1);
  createPermanentNetObject(PROPS, ++oid);
}

Stair::Stair(char *l)
{
  parser(l);
  behavior();
  build();
}

void Stair::quit()
{
  oid = 0;
}

void Stair::funcs()
{
  setActionFunc(STAIR_TYPE, 0, WO_ACTION gotoFront, "Approach");
}
