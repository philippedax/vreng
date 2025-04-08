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
// waterfall.cpp
//
// Waterfall effect
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "waterfall.hpp"
#include "world.hpp"	// ground


const OClass Waterfall::oclass(WATERFALL_TYPE, "Waterfall", Waterfall::creator);


/** Creation from a file */
Object * Waterfall::creator(char *l)
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
  points = false;	// lines
  ground = World::current()->ground;
  for (int i=0; i<3; i++) color[i] = 1;	// white
}

void Waterfall::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if      (! stringcmp(l, "number")) l = parseUInt16(l, &number, "number");
    else if (! stringcmp(l, "flow"))   l = parseFloat(l, &flow, "flow");
    else if (! stringcmp(l, "speed"))  l = parseFloat(l, &speed, "speed");
    else if (! stringcmp(l, "ground")) l = parseFloat(l, &ground, "ground");
    else if (! stringcmp(l, "size"))   l = parseUInt8(l, &pt_size, "size");
    else if (! stringcmp(l, "points")) l = parseBool(l, &points, "points");
    else if (! stringcmp(l, "color")) {
      l = parseVector3f(l, color, "color");
      onecolor = true;
    }
    else {
      parse()->errorAtLine(l);
      l = parse()->nextToken();
      break;
    }
  }
  end_while_parse(l);
}

void Waterfall::geometry()
{
  char s[128];

  sprintf(s, "geom shape=\"none\" />");
  parseSolid(s);
}

Waterfall::Waterfall(char *l)
{
  parser(l);
  behaviors();
  geometry();
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
