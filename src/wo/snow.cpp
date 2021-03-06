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


//local
static Snow *psnow = NULL;	// singleton

void Snow::defaults()
{
  system = SNOW;
  number = DEF_NUM;
  flow = DEF_FLOW;
  speed = 0.5; //DEF_SPEED / 2;
  pt_size = DEF_PTSIZE;
  points = true;
  ground = World::current()->getGround();
  pcloud = NULL;
  for (int i=0; i<3; i++) color[i] = 1;	// white
}

void Snow::makeSolid()
{
  char s[128];
  sprintf(s, "solid shape=\"none\" />");
  parse()->parseSolid(s, SEP, this);
}

Snow::Snow(Cloud *cloud, void *d, time_t s, time_t u)
{
  defaults();
  behavior();
  makeSolid();
  inits();
  pcloud = cloud;
  mycolor = true;
  psnow = this;
  pt_size = 2;
  state = ACTIVE;
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
