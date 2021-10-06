//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2011 Philippe Dax
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
#include "mirror.hpp"
#include "user.hpp"	// USER_TYPE
#include "solid.hpp"	// getSolid
#include "android.hpp"	//
#include "guy.hpp"	//
#include "man.hpp"	//
#include "render.hpp"	// minirender
#include "icon.hpp"	// ICON_TYPE


const OClass Mirror::oclass(MIRROR_TYPE, "Mirror", Mirror::creator);

// local
static uint16_t oid = 0;


WObject * Mirror::creator(char *l)
{
  return new Mirror(l);
}

void Mirror::defaults()
{
  state = false;
}

void Mirror::behavior()
{
  enableBehavior(COLLIDE_ONCE);

  initMobileObject(0);
  createPermanentNetObject(PROPS, ++oid);

  setReflexive(state);
}

void Mirror::parser(char *l)
{
  defaults();
  l = tokenize(l);
  l = parse()->parseAttributes(l, this);
}

Mirror::Mirror(char *l)
{
  parser(l);
  behavior();
}

bool Mirror::updateToNetwork(const Pos &oldpos)
{
  return updatePosToNetwork(oldpos, PROPXY, PROPZ, PROPAZ, PROPAX, PROPAY);
}

bool Mirror::whenIntersect(WObject *pcur, WObject *pold)
{
  switch (pcur->type) {
  case USER_TYPE:
    projectPosition(pcur, pold);
    break;
  case ICON_TYPE:
    // stick the icon on the wall
    doAction(ICON_TYPE, Icon::STICK, this, pcur, 0, 0);
    pold->copyPosAndBB(pcur->pos);
    break;
  default:
    pold->copyPositionAndBB(pcur);
  }
  return true;
}

void Mirror::render()
{
#if 0 //done by solid.cpp
  glEnable(GL_STENCIL_TEST);         // enable stencil
  glClearStencil(0);                 // set the clear value
  glClear(GL_STENCIL_BUFFER_BIT);    // clear the stencil buffer
  glStencilFunc(GL_ALWAYS, 1, 1);    // always pass the stencil test
  glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  if (dlists[curframe] > 0)
    glCallList(dlists[curframe]);    // draw the mirror inside the stencil
  glStencilFunc(GL_ALWAYS, 1, 1);    // always pass the stencil test
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); // make stencil buffer read only
  glEnable(GL_DEPTH_TEST);
  glPushMatrix();
   GLdouble eqn[4] = {-1,0,0,0};	// do clipping plane to avoid bugs when the avatar is near
   glClipPlane(GL_CLIP_PLANE0, eqn);
   glEnable(GL_CLIP_PLANE0);
   glStencilFunc(GL_EQUAL, 1, 1);	// draw only where the stencil == 1
#endif

  if (state == true) {			// if is reflexive
    mirroredScene();			// display the mirrored scene
  }
  else {
    glPushMatrix();
     glRotatef(RAD2DEG(pos.az), 0,0,1);
     glTranslatef(pos.x, pos.y, pos.z);
     glEnable(GL_BLEND);                // mirror shine effect
     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
     glDepthMask(GL_FALSE);
     glDepthFunc(GL_LEQUAL);

     GLint dlist = getSolid()->getDlist();
     if (dlist > 0)
       glCallList(dlist);    		// draw the physical mirror without reflexions

     glDepthFunc(GL_LESS);
     glDepthMask(GL_TRUE);
     glDisable(GL_BLEND);
    glPopMatrix();
  }

#if 0 //done by solid.cpp
   glDisable(GL_CLIP_PLANE0);
  glPopMatrix();
  glDisable(GL_STENCIL_TEST);        // disable the stencil
  glEnable(GL_BLEND);                // mirror shine effect
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDepthMask(GL_FALSE);
  glDepthFunc(GL_LEQUAL);

  if (dlists[curframe] > 0)
    glCallList(dlists[curframe]);    // draw the physical mirror

  glDepthFunc(GL_LESS);
  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);
#endif
}

void Mirror::mirroredScene()
{
  if (state == false) return;

  // 1) faire une translation pour amener le plan de reflexion a la position miroir
  //dax8 glTranslatef(-pos.x, -pos.y, -pos.z);
  glTranslatef(-pos.x, 0, 0);
  // 2) le miroir est dans le plan YZ; faire une reflexion par -1 en X
  glScalef(-1, 1, 1);
  // 3) mettre un plan de clipping a la position du miroir afin d'eliminer
  //    les reflexions des objets qui sont a l'arriere du miroir
  // 4) faire la translation inverse
  //dax8 glTranslatef(pos.x, pos.y, pos.z);
  glTranslatef(pos.x, 0, 0);
  // D) displays scene (opaque objects only)
  int i = 0;
  list<Solid*> solidlist = ::g.render.getSolidList();
  for (list<Solid*>::iterator s = solidlist.begin(); s != solidlist.end(); s++, i++) {
    if ((*s) && (*s)->isVisible() && (*s)->isOpaque()) {
      //trace2(DBG_FORCE, " %d %s", i, (*s)->object()->getInstance());
      glPushMatrix();
       // rotation inverse lorsque que le miroir tourne parallelement a notre vision.
       glRotatef(RAD2DEG((*s)->object()->pos.az), 0,0,1);
       glRotatef(-RAD2DEG((*s)->object()->pos.ay), 0,1,0);
       //dax7 glRotatef(-RAD2DEG((*s)->object()->pos.ax), 1,0,0);
       glTranslatef(-(*s)->object()->pos.x, -(*s)->object()->pos.y, -(*s)->object()->pos.z);
       glScalef(1, -1, 1);
       glCallList((*s)->getDlist());
      glPopMatrix();
    }
  }
}

void Mirror::quit()
{
  oid = 0;
  savePersistency();
}

void Mirror::mirrorOn(Mirror *po, void *d, time_t s, time_t u)
{
  po->state = true;
  po->setReflexive(true);
  po->enableBehavior(SPECIFIC_RENDER);
}

void Mirror::mirrorOff(Mirror *po, void *d, time_t s, time_t u)
{
  po->state = false;
  po->setReflexive(false);
  po->disableBehavior(SPECIFIC_RENDER);
}

void Mirror::funcs()
{
  setActionFunc(MIRROR_TYPE, 0, WO_ACTION mirrorOn, "Switch-On");
  setActionFunc(MIRROR_TYPE, 1, WO_ACTION mirrorOff, "Switch-Off");
  setActionFunc(MIRROR_TYPE, 2, WO_ACTION gotoFront, "Approach");
}
