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
#include "snow.hpp"
#include "world.hpp"	// getGround
#include "cloud.hpp"	// Cloud


const OClass Snow::oclass(SNOW_TYPE, "Snow", Snow::creator);

//local
static Snow *psnow = NULL;	// singleton

/* creation from a file */
WObject * Snow::creator(char *l)
{
  return new Snow(l);
}

void Snow::defaults()
{
  system = SNOW;
  number = DEF_NUM;
  flow = DEF_FLOW;
  speed = DEF_SPEED;
  pt_size = DEF_PTSIZE;
  points = true;
  ground = World::current()->getGround();
  pcloud = NULL;
  for (int i=0; i<3; i++) color[i] = 1;	// white
}

void Snow::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    else if (!stringcmp(l, "number")) l = parse()->parseUInt16(l, &number, "number");
    else if (!stringcmp(l, "flow")) l = parse()->parseFloat(l, &flow, "flow");
    else if (!stringcmp(l, "speed")) l = parse()->parseFloat(l, &speed, "speed");
    else if (!stringcmp(l, "ground")) l = parse()->parseFloat(l, &ground, "ground");
    else if (!stringcmp(l, "color")) l = parse()->parseVector3f(l, color, "color");
    else if (!stringcmp(l, "size")) l = parse()->parseUInt8(l, &pt_size, "size");
  }
  end_while_parse(l);
}

Snow::Snow(char *l)
{
  parser(l);
  behavior();
  inits();
}

Snow::Snow(Cloud *cloud, void *d, time_t s, time_t u)
{
  defaults();
  behavior();
  inits();
  pcloud = cloud;
  mycolor = true;
  psnow = this;
  pt_size = 2;
}

void Snow::changePermanent(float dt)
{
  if (state == INACTIVE) return;

  if (pcloud) {
    pos.x = pcloud->pos.x;
    pos.y = pcloud->pos.y;
    pos.z = pcloud->pos.z + 3;
    pos.az = pcloud->pos.az;
  }
  regenerate(dt);
}

/* Creation: this method is invisible: called by cloud */
void Snow::start_cb(Cloud *cloud, void *d, time_t s, time_t u)
{
  new Snow(cloud, d, s, u);
}

void Snow::stop_cb(Cloud *cloud, void *d, time_t s, time_t u)
{
  psnow->state = INACTIVE;
}

void Snow::funcs()
{
  // called by cloud
  setActionFunc(SNOW_TYPE, START, WO_ACTION start_cb, "");
  setActionFunc(SNOW_TYPE, STOP, WO_ACTION stop_cb, "");
}
