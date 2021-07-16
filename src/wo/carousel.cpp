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
#include "carousel.hpp"
#include "x3d.hpp"	// X3d
#include "user.hpp"	// USER_TYPE


const OClass Carousel::oclass(CAROUSEL_TYPE, CAROUSEL_NAME, Carousel::creator);

//local
static X3d *x3dmodel = NULL;


WObject * Carousel::creator(char *l)
{
return new Carousel(l);
}

void Carousel::defaults()
{
x3d = NULL;
dimx = dimy = dimz = 0.2;	// minimum
scale = .5;
}

/** parser */
void Carousel::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if (! stringcmp(l, "url=")) l = parse()->parseUrl(l, names.url);
    else if (! stringcmp(l, "scale=")) l = parse()->parseFloat(l, &scale, "scale");
    else if (! stringcmp(l, "dim=")) {
      l = parse()->skipEqual(l);
      l = parse()->skipQuotes(l);	// to get values
      l = parse()->parseFloat(l, &dimx);
      l = parse()->parseFloat(l, &dimy);
      l = parse()->parseFloat(l, &dimz);
    }
  }
  end_while_parse(l);
}

void Carousel::makeSolid()
{
  char s[128];

  sprintf(s, "solid shape=\"bbox\" dim=\"%.2f %.2f %.2f\" />", dimx, dimy, dimz);
  parse()->parseSolid(s, SEP, this);
}

/** constructor */
Carousel::Carousel(char *l)
{
  parser(l);
  makeSolid();

  x3dmodel = x3d = new X3d(names.url);

  enableBehavior(SPECIFIC_RENDER);
  initMobileObject(0);
}

X3d * Carousel::getx3d()
{
  return x3dmodel;
}

void Carousel::render()
{
  glPushMatrix();
   glTranslatef(pos.x, pos.y, pos.z);
   glScalef(scale, scale, scale);

   x3d->render();

  glPopMatrix();
}

bool Carousel::whenIntersect(WObject *pcur, WObject *pold)
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
  carousel->x3d->animationOn = true;
}

void Carousel::pause(Carousel *carousel, void *d, time_t s, time_t u)
{
  carousel->x3d->animationOn = false;
}

void Carousel::stop(Carousel *carousel, void *d, time_t s, time_t u)
{
  carousel->x3d->animationOn = false;
  carousel->x3d->resetAnimations();
}

/**< Sets flashy the X3d object */
void Carousel::setFlashy(Carousel *carousel, void *d, time_t s, time_t u)
{
  getx3d()->setFlashy();
}

void Carousel::resetFlashy(Carousel *carousel, void *d, time_t s, time_t u)
{
  getx3d()->resetFlashy();
}

void Carousel::funcs()
{
  setActionFunc(CAROUSEL_TYPE, 0, WO_ACTION start, "Start");
  setActionFunc(CAROUSEL_TYPE, 1, WO_ACTION pause, "Pause");
  setActionFunc(CAROUSEL_TYPE, 2, WO_ACTION stop, "Stop");
  setActionFunc(CAROUSEL_TYPE, 3, WO_ACTION setFlashy, "Flash");
  setActionFunc(CAROUSEL_TYPE, 4, WO_ACTION resetFlashy, "Reset");
}
