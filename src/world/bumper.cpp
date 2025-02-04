//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2012 Philippe Dax
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
// bumper.cpp
//
// Bumper handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "bumper.hpp"
#include "move.hpp"	// gotoFront
#include "user.hpp"	// localuser
#include "ball.hpp"	// BALL_TYPE


const OClass Bumper::oclass(BUMPER_TYPE, "Bumper", Bumper::creator);

// local
static uint16_t oid = 0;


/* creation from a file */
Object * Bumper::creator(char *l)
{
  return new Bumper(l);
}

void Bumper::defaults()
{
  forcex = forcey = forcez = 1;
  capted = NULL;
}

void Bumper::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if      (!stringcmp(l, "forcex")) l = parseFloat(l, &forcex, "forcex");
    else if (!stringcmp(l, "forcey")) l = parseFloat(l, &forcey, "forcey");
    else if (!stringcmp(l, "forcez")) l = parseFloat(l, &forcez, "forcez");
  }
  end_while_parse(l);
}

void Bumper::behaviors()
{
  mobileObject(1);
}

Bumper::Bumper(char *l)
{
  parser(l);
  behaviors();
}

bool Bumper::isMoving()
{
  return (state != INACTIVE);
}

void Bumper::timing(time_t sec, time_t usec, float *lasting)
{
  *lasting = diffTime(sec, usec);
  if (*lasting > move.ttl) {
    move.sec = sec;
    move.usec = usec;
  }
}

void Bumper::changePosition(float lasting)
{
  static uint8_t tilts = 5;

  if (lasting > move.ttl) {
    setFrame((getFrame()+1) % getFrames());  // next frame
    if (capted) {
      float dx = (capted->pos.x - pos.x) * cos(localuser->pos.az);
      float dy = (capted->pos.y - pos.y) * sin(localuser->pos.az);
      if (dx < 0) localuser->pos.x += dx * forcex;
      else        localuser->pos.x += dx * forcex;
      if (dy < 0) localuser->pos.y += dy * forcey;
      else        localuser->pos.y += dy * forcey;
      localuser->pos.z = capted->pos.z;
      capted = NULL;
    }
    tilts--;
    if (tilts == 0) {
      tilts = 5;
      state = INACTIVE;
    }
  }
}

bool Bumper::whenIntersect(Object *pcur, Object *pold)
{
  switch (pcur->type) {
    case USER_TYPE:
    case BALL_TYPE:
    {
      float dx = pcur->pos.x - pold->pos.x;
      float dy = pcur->pos.y - pold->pos.y;
      float dz = pcur->pos.z - pold->pos.z;
      pcur->pos.x = pold->pos.x - (forcex * dx);
      pcur->pos.y = pold->pos.y - (forcey * dy);
      pcur->pos.z = pold->pos.z - (forcez * dz);
      pcur->projectPosition(pold, pcur);
      capted = pcur;
      imposedMovement(MAXFLOAT);
      state = ACTIVE;
      break;
    }
    default:
      pcur->projectPosition(pcur, pold);
      break;
  }
  return true;
}

void Bumper::quit()
{
  oid = 0;
}

void Bumper::funcs()
{
  setActionFunc(BUMPER_TYPE, 0, _Action gotoFront, "Approach");
  setActionFunc(BUMPER_TYPE, 1, _Action pause_cb, "Pause/Continue");
}
