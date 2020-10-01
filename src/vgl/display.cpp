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
/*
 * Authors: Fabrice Bellard, Philippe Dax
 */
#include "vreng.hpp"
#include "solid.hpp"	// Solid, object()
#include "render.hpp"	// sharedRender
#include "wobject.hpp"	// WObject
#include "user.hpp"	// localuser
#include "android.hpp"	// render
#include "body.hpp"	// render
#include "man.hpp"	// Man
#include "guy.hpp"	// Guy
#include "flare.hpp"	// render


void Solid::render3D(rendering_mode sel, uint8_t order)
{
  if (ray_dlist) displayRay();
  if (! object() || ! object()->isValid())  return;  // orphean solid
  if (sel != ZBUFSELECT) {
    if (! isVisible())  return;  // invisible
    if (isBlinking() && (! toggleBlinking()))  return;  // pass one turn
  }

  switch (order) {

    case Render::OPAQUE: // Display opaque solids
      if (isreflexive) {
        displayReflexive();
      }
      if (isflary) displayFlary();	// Display attached flares
      displayNormal();
      break;

    case Render::TRANSLUCID: // Display translucid solids 
      if (isreflexive) {
        trace2(DBG_VGL, " o%d %s", order, object()->typeName());
        displayReflexive();
      }
      else
        displayNormal();
      break;

    case Render::FLASHRAY: // Display flashy edges and ray
      if (isflashy && sel != ZBUFSELECT) displayFlashy();
      if (isflary) displayFlary();	// Display attached flares
      if (ray_dlist) displayRay();
      break;

    case Render::LOCALUSER: // Display local user last
      displayNormal();
      break;
  }
}

/* transpose vreng to opengl coordinates system */
void Solid::vr2gl()
{
  GLfloat gl_mat[16];
  M4toV16(&position, gl_mat);
  glMultMatrixf(gl_mat);
}

void Solid::displayRay()
{
  glPushAttrib(GL_LINE_BIT);
  glPushMatrix();
   glDisable(GL_LIGHTING);
   glEnable(GL_LINE_STIPPLE);
   glCallList(ray_dlist);
   glDisable(GL_LINE_STIPPLE);
   glEnable(GL_LIGHTING);
  glPopMatrix();
  glPopAttrib();
}

int Solid::displayNormal()
{
  return displayList(NORMAL);
}

int Solid::displayReflexive()
{
  return displayList(REFLEXIVE);
}

int Solid::displayVirtual()
{
  return displayList(VIRTUAL);
}

int Solid::displayFlashy()
{
  return displayList(FLASHY);
}

void Solid::displayFlary()
{
  if (object()->flare) {
    //error("flary: %s %.2f %.2f %.2f", object()->names.instance, object()->pos.x, object()->pos.y, object()->pos.z);
    displayNormal();  // object alone

    glPushMatrix();
    glRotatef(RAD2DEG(localuser->pos.az), 0, 0, -1);
    glTranslatef(object()->pos.x, object()->pos.y, object()->pos.z);
    vr2gl();
    object()->flare->render(object()->pos);
    glPopMatrix();
  }
  return;
}


/* Renders a solid calling its dlists. */
int Solid::displayList(int mode = NORMAL)
{
  if (! dlists)  return 0;

  glPushMatrix();
  {
   vr2gl();	// transpose vreng to opengl

   switch (mode) {

   case NORMAL:
      // marks the object in the zbuffer
      glPopName();
      glPushName((GLuint) (long) object()->num & 0xffffffff);

   case VIRTUAL:
     if (wobject && wobject->isValid() && wobject->type == USER_TYPE) {
       User *user = (User *) wobject;
       if (user->man) {
         glRotatef(RAD2DEG(-user->man->pos.ax), 1, 0, 0);
         glRotatef(RAD2DEG(-user->man->pos.ay), 0, 1, 0);
         glRotatef(RAD2DEG(-user->man->pos.az), 0, 0, 1);
       }
       else {
         glRotatef(RAD2DEG(-user->pos.ax), 1, 0, 0);
         glRotatef(RAD2DEG(-user->pos.ay), 0, 1, 0);
         glRotatef(RAD2DEG(-user->pos.az), 0, 0, 1);
       }
     }
     if (dlists[curframe] > 0) glCallList(dlists[curframe]);
     break;

   case FLASHY:
     glPushMatrix();
      glPolygonOffset(2., 1.);		// factor=2 unit=1
      glDisable(GL_POLYGON_OFFSET_FILL);// wired mode
      glColor3fv(flashcolor);
      glLineWidth(1);
      glScalef(1.03, 1.03, 1.03);	// 3%100 more
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      if (dlists[curframe] > 0) glCallList(dlists[curframe]);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
     glPopMatrix();
     break;

   case REFLEXIVE:
    if (! ::g.render.haveStencil()) {	// no stencil buffer
      static bool todo = true;
      if (todo) {
        todo = false;
        error("no stencils available");
      }
      glPushMatrix();
      if (dlists[curframe] > 0) glCallList(dlists[curframe]); // draw the mirror alone
      glPopMatrix();
      return dlists[curframe];
    } 

    glPushMatrix();
     glEnable(GL_STENCIL_TEST);		// enable stencil
     glClearStencil(0);			// set the clear value
     glClear(GL_STENCIL_BUFFER_BIT);	// clear the stencil buffer
     glStencilFunc(GL_ALWAYS, 1, 1);	// always pass the stencil test
     glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
     glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
     if (dlists[curframe] > 0) glCallList(dlists[curframe]); // draw the mirror inside the stencil
     glStencilFunc(GL_ALWAYS, 1, 1);	// always pass the stencil test
     glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
     glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); // make stencil buffer read only
     glEnable(GL_DEPTH_TEST);
     glStencilFunc(GL_EQUAL, 1, 1);	// draw only where the stencil == 1
     glPushMatrix();
      GLdouble plane[4] = {-1,0,0,0};	// do clipping plane to avoid bugs when the avatar is near
#if 0 //DAX
      glCullFace(GL_FRONT);
      glRotatef(RAD2DEG(object()->pos.az), 0,0,1);
      glTranslatef(-object()->pos.x, -object()->pos.y, -object()->pos.z);
#endif
      glClipPlane(GL_CLIP_PLANE0, plane);
      glEnable(GL_CLIP_PLANE0);

      displayMirroredScene();		// display the mirrored scene

      glDisable(GL_CLIP_PLANE0);
     glPopMatrix();
     glDisable(GL_STENCIL_TEST);	// disable the stencil
     glEnable(GL_BLEND);	// mirror shine effect
     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
     glDepthMask(GL_FALSE);
     glDepthFunc(GL_LEQUAL);

     if (dlists[curframe] > 0) glCallList(dlists[curframe]); // draw the physical mirror

     glDepthFunc(GL_LESS);
     glDepthMask(GL_TRUE);
     glDisable(GL_BLEND);
    glPopMatrix();
    break;
   }
  }
  glPopMatrix();
  return dlists[curframe];		// displaylist number
}

/* Display mirrored scene */
void Solid::displayMirroredScene()
{
  // 1) faire une translation pour amener le plan de reflexion à la position miroir
  glTranslatef(-object()->pos.x, 0, 0);
  // 2) le miroir est dans le plan YZ; faire une reflexion par -1 en X
  glScalef(-1, 1, 1);
  // 3) mettre un plan de clipping a la position du miroir afin d'eliminer
  //    les reflexions des objets qui sont à l'arriere du miroir
  // 4) faire la translation inverse
  glTranslatef(object()->pos.x, 0, 0);
  // D) displays scene (opaque solids only)
  for (list<Solid*>::iterator it=::g.render.solidList.begin(); it!=::g.render.solidList.end(); it++) {
    if ((*it)->object() && (*it)->isVisible() && (*it)->isOpaque()) {
      glPushMatrix();
       // rotation inverse lorsque que le miroir tourne parallelement a notre vision.
       glRotatef(RAD2DEG(object()->pos.az), 0,0,1);
       glRotatef(-RAD2DEG(object()->pos.ay), 0,1,0);
       glTranslatef(-object()->pos.x, object()->pos.y, -object()->pos.z);
       glScalef(1, -1, 1);
       (*it)->displayVirtual();
      glPopMatrix();
    }
  }
  glPushMatrix();
   glRotatef(RAD2DEG(object()->pos.az), 0,0,1);
   glRotatef(-RAD2DEG(object()->pos.ay), 0,1,0);
   glTranslatef(-object()->pos.x, object()->pos.y, -object()->pos.z);

   // Displays avatar
   if (localuser->android) {
#if 1 //dax
     localuser->android->getSolid()->displayVirtual();
#else
     Pos pos;
     pos.z = localuser->pos.z - localuser->height/2;
     pos.x = localuser->pos.x;
     pos.y = localuser->pos.y;
     localuser->android->body->render(pos);
#endif
   }
   else if (localuser->guy) localuser->guy->getSolid()->displayVirtual();
   else if (localuser->man) localuser->getSolid()->displayVirtual();
   else glCallList(localuser->getSolid()->displayVirtual());
  glPopMatrix();
}
