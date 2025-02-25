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
// spiral.cpp
//
// Spiral stairs
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "spiral.hpp"
#include "move.hpp"	// gotoFront


const OClass Spiral::oclass(SPIRAL_TYPE, "Spiral", Spiral::creator);

// local
static uint16_t oid = 0;


/** Creation from a file */
Object * Spiral::creator(char *l)
{
  return new Spiral(l);
}

void Spiral::defaults()
{
  dir = 1;
  height = 0;
  speed = 0;
}

void Spiral::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if (! stringcmp(l, "height")) l = parseFloat(l, &height, "height");
    else {
      parse()->errorAtLine(l);
      l = parse()->nextToken();
      break;
    }
  }
  end_while_parse(l);
}

void Spiral::build()
{
  uint8_t nsteps = 0;
  float sx = pos.bbs.v[0];	// step width
  float sy = pos.bbs.v[1];	// step depth
  float sz = 2 * pos.bbs.v[2];	// step height

  nsteps = int( ceil(height /2 / sz) );

  for (int n=0; n < nsteps; n++) {
    Pos npos;
    npos.az = pos.az;
    npos.ax = npos.ay = 0;
    float delta = atan(sy / sx);
    npos.x = pos.x + (sx * (cos(delta * n) - 1));
    npos.y = pos.y + (sy * (sin(delta * n) - 1));
    npos.z = pos.z + (sz * n * 2);
    npos.az = pos.az + (delta * n);

    new Step(npos, pos, "spiral", geomsolid, false, height, 0, 1);
  }
}

void Spiral::inits()
{
  stillObject();

  mobile = false;
  build();
}

Spiral::Spiral(char *l)
{
  parser(l);
  inits();
}

void Spiral::quit()
{
  oid = 0;
}

void Spiral::funcs()
{
  setActionFunc(SPIRAL_TYPE, 0, _Action gotoFront, "Approach");
}
