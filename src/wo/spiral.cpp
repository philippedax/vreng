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
#include "spiral.hpp"
#include "move.hpp"	// gotoFront


const OClass Spiral::oclass(SPIRAL_TYPE, "Spiral", Spiral::creator);

// local
static uint16_t oid = 0;


/* creation from a file */
WObject * Spiral::creator(char *l)
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
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if (!stringcmp(l, "height")) l = parse()->parseFloat(l, &height, "height");
  }
  end_while_parse(l);
}

void Spiral::build()
{
  float sx = pos.bbsize.v[0];  // step width
  float sy = pos.bbsize.v[1];  // step depth
  float sz = pos.bbsize.v[2];  // step height

  nsteps = (int) ceil(height / sz);

  for (int n=0; n <= nsteps; n++) {
    Pos newpos;
    newpos.az = pos.az;
    newpos.ax = newpos.ay = 0;
    float deltaspiral = atan(sy / sx);
    newpos.x = pos.x + (sx * (cos(deltaspiral * n) - 1));
    newpos.y = pos.y + (sy * (sin(deltaspiral * n) - 1));
    newpos.z = pos.z + (sz * n);
    newpos.az = pos.az + (deltaspiral * n);

    nextstep = new Step(newpos, pos, geometry, false, height, 0, dir);
  }
}

void Spiral::behavior()
{
  enableBehavior(PERSISTENT);
  setRenderPrior(RENDER_NORMAL);

  initializeStillObject();
}

void Spiral::inits()
{
  mobile = false;
  build();
}

Spiral::Spiral(char *l)
{
  parser(l);
  behavior();
  inits();
}

void Spiral::quit()
{
  oid = 0;
  clearList();
  flushMySqlPosition();
}

void Spiral::funcs()
{
  setActionFunc(SPIRAL_TYPE, 0, O_ACTION gotoFront, "Approach");
}
