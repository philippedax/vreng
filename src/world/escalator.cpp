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
// escalator.cpp
//
// Escalator handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "escalator.hpp"
#include "move.hpp"	// goFront


const OClass Escalator::oclass(ESCALATOR_TYPE, "Escalator", Escalator::creator);

// local
static uint16_t oid = 0;


/** Creation from a file */
Object * Escalator::creator(char *l)
{
  return new Escalator(l);
}

void Escalator::defaults()
{
  on = true;
  dir = 1;	// up by default
  height = 0;
  length = 0;
  speed = Step::LSPEED;
  mobile = true;
  stuck = false;
  escaList.clear();
}

void Escalator::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if (! stringcmp(l, "dir=")) {
      char modestr[16];
      l = parseString(l, modestr, "dir");
      if      (! stringcmp(modestr, "up"))   dir = 1;
      else if (! stringcmp(modestr, "down")) dir = -1;
    }
    else if (! stringcmp(l, "height")) l = parseFloat(l, &height, "height");
    else if (! stringcmp(l, "length")) l = parseFloat(l, &length, "length");
    else if (! stringcmp(l, "speed"))  l = parseFloat(l, &speed, "speed");
    else if (! stringcmp(l, "on"))     l = parseBool(l, &on, "on");
    else {
      parse()->errorAtLine(l);
      l = parse()->nextToken();
      break;
    }
  }
  end_while_parse(l);
}

void Escalator::build()
{
  uint8_t nsteps = 0;
  float sx = 1 * pos.bbs.v[0];  // step width
  float sy = 1 * pos.bbs.v[1];  // step depth
  float sz = 2 * pos.bbs.v[2];  // step height

  if (height && dir > 0) {
    //height += sz;  // add the top step
  }
  else if (length && pos.ax) {  // escalator defined by its length and its angle
    height = length * tan(pos.ax);
  }

  nsteps = int( ceil(height / sz) );
  //echo("nsteps: %d", nsteps);

  escaList.push_back(this);

  for (int n=0; n < nsteps; n++) {
    Pos npos = pos;
    npos.x = pos.x + dir*(sin(pos.az) * sx * n);
    npos.y = pos.y + dir*(cos(pos.az) * sy * n);
    npos.z = pos.z + dir*(sz * n);

    //echo("npos=%.1f %.1f %.1f d=%d", npos.x,npos.y,npos.z,dir);
    Step *step = new Step(npos, pos, "escalator", geom, true, height, speed, dir);
    escaList.push_back(step);
  }
  permanentMovement(speed);
}

void Escalator::inits()
{
  mobileObject(1);
  createNetObj(PROPS, ++oid);
}

Escalator::Escalator(char *l)
{
  parser(l);
  inits();
  build();
  if (on) {
    running();
  }
}

Escalator::~Escalator()
{
  escaList.clear();
  oid = 0;
}

void Escalator::funcs()
{
  setActionFunc(ESCALATOR_TYPE, 0, _Action pause_cb, "Pause/Continue");
  //setActionFunc(ESCALATOR_TYPE, 1, _Action stop_cb, "Stop/Restart");
  //setActionFunc(ESCALATOR_TYPE, 2, _Action goFront, "Approach");
}
