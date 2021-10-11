//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2012 Philippe Dax
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
#include "slider.hpp"
#include "move.hpp"	// gotoFront


const OClass Slider::oclass(SLIDER_TYPE, "Slider", Slider::creator);

// local
static uint16_t oid = 0;


/* creation from a file */
WObject * Slider::creator(char *l)
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
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if      (!stringcmp(l, "incrx")) l = parse()->parseFloat(l, &incrx, "incrx");
    else if (!stringcmp(l, "incry")) l = parse()->parseFloat(l, &incry, "incry");
    else if (!stringcmp(l, "incrz")) l = parse()->parseFloat(l, &incrz, "incrz");
  }
  end_while_parse(l);
}

void Slider::behavior()
{
  initMobileObject(1);
  createPermanentNetObject(PROPS, ++oid);
}

Slider::Slider(char *l)
{
  parser(l);
  behavior();
}

bool Slider::isMoving()
{
  return (state != INACTIVE);
}

void Slider::updateTime(time_t sec, time_t usec, float *lasting)
{
  *lasting = diffTime(sec, usec);
  if (*lasting > move.ttl) {
    move.sec = sec;
    move.usec = usec;
  }
}

void Slider::changePosition(float lasting)
{
  if (! capted) return;

  // check if user out
  if (interAABB(capted, this) == 0) {
    //error("user out %s", names.instance);
    capted = NULL;
    state = INACTIVE;
    return;
  }
  //error("user in %s", names.instance);
  // progression
  capted->pos.x += incrx * cos(pos.az);
  capted->pos.y += incry * sin(pos.az);
  capted->pos.z += incrz;
  capted->pos.az = pos.az;
  capted->updatePosition();
}

bool Slider::whenIntersect(WObject *pcur, WObject *pold)
{
  //error("in %s", names.instance);
  capted = pcur;
  enableImposedMovement();
  state = ACTIVE;
  return true;
}

void Slider::quit()
{
  oid = 0;
  savePersistency();
}

void Slider::funcs()
{
  setActionFunc(SLIDER_TYPE, 0, _Action gotoFront, "Approach");
  setActionFunc(SLIDER_TYPE, 1, _Action pause_cb, "Pause/Continue");
}
