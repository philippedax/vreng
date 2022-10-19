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
#include "waterfall.hpp"
#include "world.hpp"	// getGround


const OClass Waterfall::oclass(WATERFALL_TYPE, "Waterfall", Waterfall::creator);


/* creation from a file */
WObject * Waterfall::creator(char *l)
{
  return new Waterfall(l);
}

void Waterfall::defaults()
{
  system = WATERFALL;
  number = DEF_NUM;
  flow = DEF_FLOW;
  speed = DEF_SPEED;
  pt_size = DEF_PTSIZE;
  points = true;
  ground = World::current()->getGround();
  for (int i=0; i<3; i++) color[i] = 1;	// white
}

void Waterfall::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    else if (!stringcmp(l, "number")) l = parse()->parseUInt16(l, &number, "number");
    else if (!stringcmp(l, "flow"))   l = parse()->parseFloat(l, &flow, "flow");
    else if (!stringcmp(l, "speed"))  l = parse()->parseFloat(l, &speed, "speed");
    else if (!stringcmp(l, "ground")) l = parse()->parseFloat(l, &ground, "ground");
    else if (!stringcmp(l, "color"))  l = parse()->parseVector3f(l, color, "color");
    else if (!stringcmp(l, "size"))   l = parse()->parseUInt8(l, &pt_size, "size");
  }
  end_while_parse(l);
}

Waterfall::Waterfall(char *l)
{
  parser(l);
  behavior();
  inits();
}

void Waterfall::pause(Waterfall *waterfall, void *d, time_t s, time_t u)
{
  if (waterfall->state == ACTIVE) waterfall->state = INACTIVE;
  else waterfall->state = ACTIVE;
}

void Waterfall::funcs()
{
  setActionFunc(WATERFALL_TYPE, 0, _Action pause, "Switch");
}
