//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://www.vreng.enst.fr/
//
// Copyright (C) 1997-2021 Philippe Dax
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
#include "vreng.hpp"
#include "carousel.hpp"
#include "x3d.hpp"	// X3d
#include "user.hpp"	// USER_TYPE


const OClass Carousel::oclass(CAROUSEL_TYPE, CAROUSEL_NAME, Carousel::creator);


WO * Carousel::creator(char *l)
{
  return new Carousel(l);
}

void Carousel::defaults()
{
  x3d = NULL;
  V3 dim = {0.2, 0.2, 0.2};	// minimum size of BB
  scale = .5;
}

/** parser */
void Carousel::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if      (! stringcmp(l, "url="))   l = parseUrl(l, names.url);
    else if (! stringcmp(l, "scale=")) l = parseFloat(l, &scale, "scale");
    else if (! stringcmp(l, "dim="))   l = parseVector3fv(l, &dim, "dim");
  }
  end_while_parse(l);
}

void Carousel::geometry()
{
  char s[128];

  sprintf(s, "solid shape=\"bbox\" dim=\"%.2f %.2f %.2f\" />", dim.v[0], dim.v[1], dim.v[2]);
  parseSolid(s);
}

void Carousel::behaviors()
{
  enableBehavior(SPECIFIC_RENDER);
}

void Carousel::inits()
{
  x3d = new X3d(names.url);

  initMobileObject(0);
}

/** constructor */
Carousel::Carousel(char *l)
{
  parser(l);
  geometry();
  behaviors();
  inits();
}

void Carousel::render()
{
  glPushMatrix();
   glTranslatef(pos.x, pos.y, pos.z);
   glScalef(scale, scale, scale);

   x3d->render();

  glPopMatrix();
}

bool Carousel::whenIntersect(WO *pcur, WO *pold)
{
  switch (pcur->type) {
  case USER_TYPE:
    return false;	// no collide
  default:
    pold->copyPositionAndBB(pcur);
    return true;	// collide
  }
}

void Carousel::start(Carousel *carousel, void *d, time_t s, time_t u)
{
  carousel->x3d->anim = true;
}

void Carousel::pause(Carousel *carousel, void *d, time_t s, time_t u)
{
  carousel->x3d->anim = false;
}

void Carousel::stop(Carousel *carousel, void *d, time_t s, time_t u)
{
  carousel->x3d->anim = false;
  carousel->x3d->resetAnimations();
}

/**< Sets flashy the X3d object */
void Carousel::setFlashy(Carousel *carousel, void *d, time_t s, time_t u)
{
  carousel->x3d->setFlashy();
}

void Carousel::resetFlashy(Carousel *carousel, void *d, time_t s, time_t u)
{
  carousel->x3d->resetFlashy();
}

void Carousel::funcs()
{
  setActionFunc(CAROUSEL_TYPE, 0, _Action start, "Start");
  setActionFunc(CAROUSEL_TYPE, 1, _Action pause, "Pause");
  setActionFunc(CAROUSEL_TYPE, 2, _Action stop, "Stop");
  setActionFunc(CAROUSEL_TYPE, 3, _Action setFlashy, "Flash");
  setActionFunc(CAROUSEL_TYPE, 4, _Action resetFlashy, "Reset");
}
