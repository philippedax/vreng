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
// rain.cpp
//
// Rain effect
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "rain.hpp"
#include "world.hpp"	// ground
#include "cloud.hpp"	// Cloud
#include "sound.hpp"	// playSound


const OClass Rain::oclass(RAIN_TYPE, "Rain", NULL);

//local
static Rain *prain = NULL;	// singleton


void Rain::defaults()
{
  system = RAIN;
  number = DEF_NUM;
  flow = DEF_FLOW;
  speed = DEF_SPEED;
  pt_size = 1;  // width
  ground = World::current()->ground;
  pcloud = NULL;
  sound = false;
  for (int i=0; i<3; i++) color[i] = .5;  // grey
}

void Rain::geometry()
{
  char s[128];

  sprintf(s, "solid shape=\"none\" />");
  parseSolid(s);
}

Rain::Rain(Cloud *cloud, void *d, time_t s, time_t u)
{
  defaults();
  behaviors();
  geometry();
  inits();
  points = false;
  pcloud = cloud;
  prain = this;
  onecolor = true;
  state = ACTIVE;
  //sound_continue();
}

void Rain::changePermanent(float dt)
{
  if (state == INACTIVE) return;

  if (pcloud) {
    pos.x = pcloud->pos.x;
    pos.y = pcloud->pos.y;
    pos.z = pcloud->pos.z + 3;
    pos.az = pcloud->pos.az;
  }
  regenerate(dt);  // new particles
}

void Rain::sound_continue(int sig)
{
  signal(SIGUSR2, sound_continue);
  Sound::playSound(RAINSND);
}

void Rain::sound_continue()
{
  if (sound) {
    sound = false;
    signal(SIGUSR2, SIG_IGN);
  }
  else {
    sound = true;
    signal(SIGUSR2, sound_continue);
    Sound::playSound(RAINSND);
  }
}

/* Creation: this method is invisible: called by cloud */
void Rain::start_cb(Cloud *cloud, void *d, time_t s, time_t u)
{
  new Rain(cloud, d, s, u);
}

void Rain::stop_cb(Cloud *cloud, void *d, time_t s, time_t u)
{
  prain->state = INACTIVE;
  signal(SIGUSR2, SIG_IGN);
}

void Rain::sound_cb(Cloud *cloud, void *d, time_t s, time_t u)
{
  prain->sound_continue();
}

void Rain::funcs()
{
  // called by cloud
  setActionFunc(RAIN_TYPE, START, _Action start_cb, "");
  setActionFunc(RAIN_TYPE, STOP, _Action stop_cb, "");
  setActionFunc(RAIN_TYPE, SOUND, _Action sound_cb, "");
}
