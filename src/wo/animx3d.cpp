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
#include "animx3d.hpp"


const OClass AnimX3d::oclass(ANIMX3D_TYPE, ANIMX3D_NAME, AnimX3d::creator);

//local
static X3d *X3dModel = NULL;


WObject * AnimX3d::creator(char *l)
{
  return new AnimX3d(l);
}

void AnimX3d::defaults()
{
  x3dmodel = NULL;
  dimx=dimy=dimz =0; //useless bouning box, if not one specified in the x3d file
}

/** parser */
void AnimX3d::parser(char *l)
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

/** constructor */
AnimX3d::AnimX3d(char *l)
{
  parser(l);

  //On se debarrasse des Pbs dus a l'absence de solide en en creant un factice
  char s[128];
  sprintf(s, "solid shape=\"bbox\" dim=\"%.2f %.2f %.2f\" />",dimx,dimy,dimz);
  parse()->parseSolid(s, SEP, this);

  x3dmodel = new X3d(names.url);
  X3dModel = x3dmodel;

  initializeMobileObject(0);
  setRenderPrior(RENDER_HIGH);
}

X3d * AnimX3d::current()
{
  return X3dModel;
}

void AnimX3d::render()
{
  glPushMatrix();
  glTranslatef(pos.x, pos.y, pos.z);
  x3dmodel->render();
  glPopMatrix();
}

bool AnimX3d::whenIntersect(WObject *pcur, WObject *pold)
{
  pold->copyPositionAndBB(pcur);
  return true;
}

void AnimX3d::start(AnimX3d *animx3d, void *d, time_t s, time_t u)
{
  animx3d->x3dmodel->animationOn = true;
}

void AnimX3d::pause(AnimX3d *animx3d, void *d, time_t s, time_t u)
{
  animx3d->x3dmodel->animationOn = false;
}

void AnimX3d::stop(AnimX3d *animx3d, void *d, time_t s, time_t u)
{
  animx3d->x3dmodel->animationOn = false;
  animx3d->x3dmodel->resetAnimations();
}

/**< Sets flashy the X3d object */
void AnimX3d::setFlashy(AnimX3d *animx3d, void *d, time_t s, time_t u)
{
  current()->setFlashy();
}

void AnimX3d::resetFlashy(AnimX3d *animx3d, void *d, time_t s, time_t u)
{
  current()->resetFlashy();
}

void AnimX3d::funcs()
{
  setActionFunc(ANIMX3D_TYPE, 0, WO_ACTION start, "Start");
  setActionFunc(ANIMX3D_TYPE, 1, WO_ACTION pause, "Pause");
  setActionFunc(ANIMX3D_TYPE, 2, WO_ACTION stop, "Stop");
  setActionFunc(ANIMX3D_TYPE, 3, WO_ACTION setFlashy, "Flash");
  setActionFunc(ANIMX3D_TYPE, 4, WO_ACTION resetFlashy, "Reset");
}
