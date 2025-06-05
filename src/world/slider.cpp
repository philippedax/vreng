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
// slider.cpp
//
// Slider handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "slider.hpp"
#include "move.hpp"	// goFront


const OClass Slider::oclass(SLIDER_TYPE, "Slider", Slider::creator);

// local
static uint16_t oid = 0;


/** creation from a file */
Object * Slider::creator(char *l)
{
  return new Slider(l);
}

void Slider::defaults()
{
  mobile = false;
  incrx = incry = incrz = 0;
  state = INACTIVE;
  capted = NULL;
}

void Slider::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if      (!stringcmp(l, "incrx")) l = parseFloat(l, &incrx, "incrx");
    else if (!stringcmp(l, "incry")) l = parseFloat(l, &incry, "incry");
    else if (!stringcmp(l, "incrz")) l = parseFloat(l, &incrz, "incrz");
    else {
      parse()->errorAtLine(l);
      l = parse()->nextToken();
      break;
    }
  }
  end_while_parse(l);
}

void Slider::behaviors()
{
  mobileObject(1);
}

Slider::Slider(char *l)
{
  parser(l);
  behaviors();
}

bool Slider::isMoving()
{
  return (state != INACTIVE);
}

void Slider::timing(time_t sec, time_t usec, float *lasting)
{
  *lasting = deltaTime(sec, usec);
  if (*lasting > move.ttl) {
    move.sec = sec;
    move.usec = usec;
  }
}

void Slider::imposed(float lasting)
{
  if (! capted) return;

  // check if user out
  if (interAABB(capted, this) == 0) {
    //echo("user out %s", objectName());
    capted = NULL;
    state = INACTIVE;
    return;
  }
  //echo("user in %s", objectName());
  // progression
  capted->pos.x += incrx * cos(pos.az);
  capted->pos.y += incry * sin(pos.az);
  capted->pos.z += incrz;
  capted->pos.az = pos.az;
  capted->updatePos();
}

bool Slider::intersect(Object *pcur, Object *pold)
{
  capted = pcur;
  imposedMovement(MAXFLOAT);
  state = ACTIVE;
  return true;
}

/** Quits - destructor */
Slider::~Slider()
{
  oid = 0;
}

void Slider::funcs()
{
  getPropFunc(SLIDER_TYPE, PROPXY, _Payload get_xy);
  getPropFunc(SLIDER_TYPE, PROPZ, _Payload get_z);
  getPropFunc(SLIDER_TYPE, PROPAZ, _Payload get_az);
  getPropFunc(SLIDER_TYPE, PROPAX, _Payload get_ax);
  getPropFunc(SLIDER_TYPE, PROPAY, _Payload get_ay);
  getPropFunc(SLIDER_TYPE, PROPHNAME, _Payload get_hname);

  putPropFunc(SLIDER_TYPE, PROPXY, _Payload put_xy);
  putPropFunc(SLIDER_TYPE, PROPZ, _Payload put_z);
  putPropFunc(SLIDER_TYPE, PROPAZ, _Payload put_az);
  putPropFunc(SLIDER_TYPE, PROPAX, _Payload put_ax);
  putPropFunc(SLIDER_TYPE, PROPAY, _Payload put_ay);
  putPropFunc(SLIDER_TYPE, PROPHNAME, _Payload put_hname);

  setActionFunc(SLIDER_TYPE, 0, _Action goFront, "Approach");
  setActionFunc(SLIDER_TYPE, 1, _Action pause_cb, "Pause/Continue");
}
