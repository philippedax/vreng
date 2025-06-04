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
// stair.cpp
//
// Stairs building
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "stair.hpp"
#include "move.hpp"	// goFront


const OClass Stair::oclass(STAIR_TYPE, "Stair", Stair::creator);

// local
static uint16_t oid = 0;


/* creation from a file */
Object * Stair::creator(char *l)
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
    l = parseAttributes(l);
    if (!l) break;
    if      (! stringcmp(l, "height")) l = parseFloat(l, &height, "height");
    else if (! stringcmp(l, "length")) l = parseFloat(l, &length, "length");
    else {
      parse()->errorAtLine(l);
      l = parse()->nextToken();
      break;
    }
  }
  end_while_parse(l);
}

void Stair::build()
{
  uint8_t nsteps = 0;
  float sx = pos.bbs.v[0];  // step width
  float sy = pos.bbs.v[1];  // step depth
  //float sz = 2 * pos.bbs.v[2];  // step height
  float sz = MIN(sx, sy);

  if (height) height += sz;	// add the top step
  else if (length && pos.ax) {  // stair defined by its length and its angle
    height = length * tan(pos.ax);
    pos.ax = 0;
  }

  nsteps = int( ceil(height / sz) );

  for (int n=0; n < nsteps; n++) {
    Pos npos;
    npos.az = pos.az;
    npos.ax = npos.ay = 0;
    npos.x = pos.x + (sin(pos.az) * sx * n);
    npos.y = pos.y + (cos(pos.az) * sy * n);
    npos.z = pos.z + (sz * n *1);

    new Step(npos, pos, "stair", geom, false, height, 0, 1);
  }
}

void Stair::inits()
{
  mobileObject(1);
}

Stair::Stair(char *l)
{
  parser(l);
  inits();
  build();
}

Stair::~Stair()
{
  oid = 0;
}

void Stair::funcs()
{
  setActionFunc(STAIR_TYPE, 0, _Action goFront, "Approach");
}
