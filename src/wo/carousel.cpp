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


const OClass Carousel::oclass(CAROUSEL_TYPE, CAROUSEL_NAME, Carousel::creator);

//local
static X3d *X3dModel = NULL;


WObject * Carousel::creator(char *l)
{
  return new Carousel(l);
}

void Carousel::defaults()
{
  x3dmodel = NULL;
  dimx=dimy=dimz =0; //useless bouning box, if not one specified in the x3d file
}

/** parser */
void Carousel::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if (!stringcmp(l, "url=")) l = parse()->parseUrl(l, names.url);
    else if (!stringcmp(l, "dim=")) {
      l = parse()->skipEqual(l);
      l = parse()->skipQuotes(l);	// to get pos.x
      l = parse()->parseFloat(l, &dimx);
      l = parse()->parseFloat(l, &dimy);
      l = parse()->parseFloat(l, &dimz);
    }
  }
  end_while_parse(l);
}

void Carousel::makeSolid()
{
  //On se debarrasse des Pbs dus a l'absence de solide en en creant un factice
  char s[128];

  sprintf(s, "solid shape=\"bbox\" dim=\"%.2f %.2f %.2f\" />",dimx,dimy,dimz);
  parse()->parseSolid(s, SEP, this);
}

/** constructor */
Carousel::Carousel(char *l)
{
  parser(l);
  makeSolid();

  x3dmodel = new X3d(names.url);
  X3dModel = x3dmodel;

  enableBehavior(SPECIFIC_RENDER);
  initMobileObject(0);
}

X3d * Carousel::current()
{
  return X3dModel;
}

void Carousel::render()
{
  glPushMatrix();
   glTranslatef(pos.x, pos.y, pos.z);

   x3dmodel->render();

  glPopMatrix();
}

bool Carousel::whenIntersect(WObject *pcur, WObject *pold)
{
  pold->copyPositionAndBB(pcur);
  return true;
}

void Carousel::start(Carousel *carousel, void *d, time_t s, time_t u)
{
  carousel->x3dmodel->animationOn = true;
}

void Carousel::pause(Carousel *carousel, void *d, time_t s, time_t u)
{
  carousel->x3dmodel->animationOn = false;
}

void Carousel::stop(Carousel *carousel, void *d, time_t s, time_t u)
{
  carousel->x3dmodel->animationOn = false;
  carousel->x3dmodel->resetAnimations();
}

/**< Sets flashy the X3d object */
void Carousel::setFlashy(Carousel *carousel, void *d, time_t s, time_t u)
{
  current()->setFlashy();
}

void Carousel::resetFlashy(Carousel *carousel, void *d, time_t s, time_t u)
{
  current()->resetFlashy();
}

void Carousel::funcs()
{
  setActionFunc(CAROUSEL_TYPE, 0, WO_ACTION start, "Start");
  setActionFunc(CAROUSEL_TYPE, 1, WO_ACTION pause, "Pause");
  setActionFunc(CAROUSEL_TYPE, 2, WO_ACTION stop, "Stop");
  setActionFunc(CAROUSEL_TYPE, 3, WO_ACTION setFlashy, "Flash");
  setActionFunc(CAROUSEL_TYPE, 4, WO_ACTION resetFlashy, "Reset");
}
