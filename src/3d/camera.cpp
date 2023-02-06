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
#include "render.hpp"
#include "scene.hpp"	// getScene, setScene
#include "solid.hpp"	// Solid
#include "wobject.hpp"	// WObject
#include "world.hpp"	// current
#include "user.hpp"	// USER_TYPE
#include "glu.hpp"	// glu alternative
#include "img.hpp"	// saveJPG
#include "ogl.hpp"	// copyPixels
#include "pref.hpp"	// width3D
#include "timer.hpp"	// rate


// global
struct Render::sCamera cam_user;


void Render::camera()
{
  cameraProjection(cam_user.fovy, cam_user.near, cam_user.far);
}

/* Observer view, called by updateCamera: vr2gl.cpp. */
void Render::setCameraPosition(M4 *vrmat)
{
  camera_pos = *vrmat;
}

/*
 * Observer view, called by setCamera: user.cpp.
 * static
 */
void Render::cameraProjection(GLfloat fovy, GLfloat near, GLfloat far)
{
  GLint vp[4];

  glGetIntegerv(GL_VIEWPORT, vp);
  GLfloat ratio = (GLfloat) vp[2] / (GLfloat) vp[3];
  GLfloat top = near * tan(fovy * M_PI_180);
  GLfloat bot = -top;
  GLfloat right = top * ratio;
  GLfloat left = -right;
  cam_user.fovy = fovy;
  cam_user.near = near;
  cam_user.far = far;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(left, right, bot, top, near, far);
  glTranslatef(0, 0, -near);
  glMatrixMode(GL_MODELVIEW);
}

void Render::setViewMode(uint8_t _view)
{
  view = _view;
  if (view != VIEW_THIRD_PERSON) {
    third_xRot = third_yRot = third_Near = 0;
  }
}

uint8_t Render::getViewMode() const
{
  return view;
}

void Render::switchViewMap()
{
  viewMap = !viewMap;
}

void Render::switchViewSat()
{
  viewSat = !viewSat;
}

void Render::switchViewObj()
{
  viewObj = !viewObj;
}

void Render::setPitch(GLfloat angle)
{
  pitch = angle;
}

void Render::cameraPosition()
{
  cameraPosition(localuser);
}

/**
 *
 * coordinates opengl XYZ vs coordinates vreng xyz :
 *
 *    Y=z
 *     |
 *     |
 *     .____ X=-y
 *    /
 *   /
 * Z=-x
 */
void Render::cameraPosition(WObject *o)
{
  // coordinates are opengl coordinates
  M4 vrmat;	// vreng matrix

  glMatrixMode(GL_MODELVIEW);
  glDisable(GL_SCISSOR_TEST);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	//default

  switch (view) {

    case VIEW_FIRST_PERSON:		// 5cm front
      vrmat = mulM4(transM4(0, 0, 0.05), mulM4(rotM4(pitch, UX), camera_pos));
      break;

    case VIEW_THIRD_PERSON:		// 80cm back
      vrmat = mulM4(transM4(0, localuser->height/6, third_Near - 0.80),
                    mulM4(rotM4(M_PI_2/6 + third_xRot + pitch, UX),
                          mulM4(rotM4(third_yRot, UY), camera_pos)
                         )
                   );
      break;

    case VIEW_THIRD_PERSON_FAR: 	// 2m40 back
      vrmat = mulM4(transM4(0, localuser->height/6, third_Near - 2.40),
                    mulM4(rotM4(M_PI_2/6 + third_xRot + pitch, UX),
                          mulM4(rotM4(third_yRot, UY), camera_pos)
                         )
                   );
      break;

    case VIEW_THIRD_PERSON_FRONT:	// -3m front
      vrmat = mulM4(transM4(0, 0, -3), mulM4(rotM4(M_PI, UY), camera_pos));
      break;

    case VIEW_VERTICAL_FROM_OBJECT:
      {
      if (o == localuser) return;
      //echo("xyz: %.1f %.1f %.1f", o->pos.x, o->pos.y, o->pos.z);
      vrmat = mulM4(transM4(-o->pos.x, -o->pos.y, -o->pos.z),	// FIXME!
                    mulM4(rotM4(M_PI_2, UX), 
                          mulM4(rotM4(M_PI_2, UY), camera_pos)
                         )
                   );
      }
      break;

    case VIEW_VERTICAL:			// 50cm top
      vrmat = mulM4(rotM4(M_PI_2, UX), mulM4(transM4(0, -.5, 0), camera_pos));
      break;

    case VIEW_VERTICAL_FAR: 		// 5m top
      vrmat = mulM4(rotM4(M_PI_2, UX), mulM4(transM4(0, -5, 0), camera_pos));
      break;

    case VIEW_TURN_AROUND:		// -2m side
      turna += (M_PI/18) / MAX(::g.timer.rate() / 5, 1);
      vrmat = mulM4(transM4(0, localuser->height/4, -2),
                     mulM4(rotM4(M_PI_2/4, UX),
                           mulM4(rotM4(third_yRot + turna, UY), camera_pos)
                          )
                    );
      break;

    case VIEW_GROUND_LEVEL:		// 2cm top
      vrmat = mulM4(transM4(0, localuser->height - 0.02, 0), camera_pos);
      break;

    case VIEW_WIRED:
      glLineWidth(1);
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      vrmat = camera_pos;
      break;

    case VIEW_SCISSOR:
      vrmat = camera_pos;
      break;
  }

  // transpose Vreng to OpenGl coordinates
  GLfloat glmat[16];		// opengl matrix
  M4toV16(&vrmat, glmat);
  glLoadMatrixf(glmat);
}

/* Displays the map on the top-right corner of the canvas */
void Render::showMap()
{
  if (! viewMap) return;

  GLint x, y, w, h;

  ::g.gui.scene()->getScene(x, y, w, h);
  
  glScissor((w/3)*2, (h/3)*2, w/3, h/3);  // top-right corner
  glEnable(GL_SCISSOR_TEST);
 
  ::g.gui.scene()->setScene(w*2/3, h*2/3, w/3, h/3);
  glMatrixMode(GL_MODELVIEW);

#if 1 //dax
  // place the mini-map at a position depending on the world's dimensions
  World *world = World::current();
  float d = floor(MAX(world->bbsize.v[0], world->bbsize.v[1])/tan(DEG2RAD(User::FOVY))-5);
  //echo("map: %.1f", d);
  M4 vrmat = mulM4(rotM4(M_PI_2, UX), transM4(0, 0, -d)); // dm top
  //M4 vrmat = mulM4(rotM4(M_PI_2, UX), mulM4(transM4(0, -d, -0), camera_pos)); // dm top

  // transpose vreng to opengl
  GLfloat glmat[16];	// opengl matrix
  M4toV16(&vrmat, glmat);
  glLoadMatrixf(glmat);
#endif

  // redraw the scene inside the scissor
  minirender();

  // reset initial state
  glDisable(GL_SCISSOR_TEST);
  ::g.gui.scene()->setScene(x, y, w, h);
}

/* Displays the satellite on the bottom-left corner of the canvas */
void Render::showSat()
{
  if (view != VIEW_SCISSOR || viewObj) return;
  //if (! viewSat) return;

  GLint x, y, w, h;

  ::g.gui.scene()->getScene(x, y, w, h);

  glScissor(x, y, w/5, h/5);   // bottom-left corner
  glEnable(GL_SCISSOR_TEST);
  ::g.gui.scene()->setScene(0, 0, w/5, h/5);

  glMatrixMode(GL_MODELVIEW);

  // scene position
  glLoadIdentity();
  glRotatef(-90 ,1,0,0);	// X
  glRotatef(90 ,0,0,1);		// Z

  // camera position
  glRotatef(-RAD2DEG(satRot.v[2]), 0, 0, 1);
  glTranslatef(-satPos.v[0], -satPos.v[1], -satPos.v[2]);

  // draw the scene inside the scissor
  glClear(GL_COLOR_BUFFER_BIT);
  minirender();

  // reset initial state
  glDisable(GL_SCISSOR_TEST);
  ::g.gui.scene()->setScene(x, y, w, h);
}

//dax void Render::showView(float posx, float posy, float posz)
void Render::showView()
{
  if (! viewObj) return;

  GLint x, y, w, h;
  GLint X, Y, W, H;

  ::g.gui.scene()->getScene(x, y, w, h);
  X = w/4; Y = h/4; W = w/2; H = h/2;

  glEnable(GL_SCISSOR_TEST);
  glScissor(x+w/4, y+h/4, w/2, h/2);
  //GLint s[4];
  //glGetIntegerv(GL_SCISSOR_BOX, s);
  //echo("vp: %d %d %d %d, %d %d %d %d, %d %d %d %d",x,y,w,h,X,Y,W,H,s[0],s[1],s[2],s[3]);
  ::g.gui.scene()->setScene(X, Y, W, H);

  glMatrixMode(GL_MODELVIEW);

#if 1 //dax map
  M4 vrmat = mulM4(transM4(-satPos.v[0], -satPos.v[1], -satPos.v[2]), rotM4(M_PI_2, UZ));
  GLfloat glmat[16];		// opengl matrix
  M4toV16(&vrmat, glmat);	// transpose vreng to opengl
  glLoadMatrixf(glmat);
#else //dax sat
  glLoadIdentity();
  glRotatef(90, 0, 0, 1);
  glTranslatef(-satPos.v[0], -satPos.v[1], -satPos.v[2]);
#endif

  // draw the scene inside the scissor
  //minirender();	// segfault

  // reset initial state
  glDisable(GL_SCISSOR_TEST);
  ::g.gui.scene()->setScene(x, y, w, h);
}

void Render::resetCamera()
{
  view = VIEW_FIRST_PERSON;
  third_xRot = third_yRot = third_Near = 0;
}

void Render::setCameraScissor(GLfloat posx, GLfloat posy, GLfloat posz, GLfloat rotz)
{
  if (view != VIEW_SCISSOR) {
    view = VIEW_SCISSOR;
    viewSat = true;
    satPos.v[0] = posx;
    satPos.v[1] = posy;
    satPos.v[2] = posz;
    satRot.v[2] = rotz;
  }
  else {
    view = VIEW_FIRST_PERSON;
  }
}

V3 Render::getVisiblePosition(WObject *po)
{
  bool seen = false;
  GLint vp[4];
  GLdouble mview[16], mproj[16];

  ::g.gui.scene()->getScene(vp[0], vp[1], vp[2], vp[3]);
  GLint w = vp[2];
  GLint h = vp[3];

  GLfloat xa  = po->pos.x;
  GLfloat ya  = po->pos.y;
  GLfloat za  = po->pos.z;
  GLfloat dxa = po->pos.bbs.v[0];
  GLfloat dya = po->pos.bbs.v[1];
  GLfloat dza = po->pos.bbs.v[2];

  glGetDoublev(GL_MODELVIEW_MATRIX, mview);
  glGetDoublev(GL_PROJECTION_MATRIX, mproj);

  GLdouble x[5], y[5], z[5];
  gluProject(xa    , ya    , za    , mview, mproj, vp, &x[0],&y[0],&z[0]);
  gluProject(xa+dxa, ya+dya, za+dza, mview, mproj, vp, &x[1],&y[1],&z[1]);
  gluProject(xa-dxa, ya+dya, za+dza, mview, mproj, vp, &x[2],&y[2],&z[2]);
  gluProject(xa+dxa, ya-dya, za-dza, mview, mproj, vp, &x[3],&y[3],&z[3]);
  gluProject(xa-dxa, ya-dya, za-dza, mview, mproj, vp, &x[4],&y[4],&z[4]);

  for (int i=0; i<5; i++) {
    y[i] = vp[3] - y[i];
  }

  for (int i=0; i<5; i++) {
    if ((x[i] > w) || (x[i] < 0) || (y[i] > h) || (y[i] < 0)) continue;
    uint16_t num = bufferSelection((GLint) x[i], (GLint) y[i]);
    WObject *o = WObject::byNum(num);
    if (o && ! strcasecmp(po->getInstance(), o->getInstance())) {
      seen = true;
      break;
    }
  }
  return setV3(x[0], y[0], seen);
}

void Render::calculateFov(GLfloat posx, GLfloat posy, GLfloat posz, GLfloat rotz, char* filename)
{
  GLint vp[4];
  GLdouble mview[16], mproj[16];

  glGetIntegerv(GL_VIEWPORT, vp);
  glGetDoublev(GL_MODELVIEW_MATRIX, mview);
  glGetDoublev(GL_PROJECTION_MATRIX, mproj);

  GLint w = vp[2]; if (w & 1) ++w;	//even values
  GLint h = vp[3]; if (h & 1) ++h;

  // crash on some machines, use GL_BACK to render (test: va vers @dax@10,6,-5,1.0@ !)
  glDrawBuffer(GL_BACK);
  glPushMatrix();
   // transformations to have the good position
   glRotatef(-90, 1,0,0);
   glRotatef(90, 0,0,1);
   glRotatef(-RAD2DEG(rotz), 0,0,1);
   glTranslatef(-posx, -posy, -posz);

   // redraw the scene (objects only)
   clearBuffer();
   renderSolids(0);
  glPopMatrix();

  // we use the back buffer, then we read the back
  uint8_t *pixjpg = Ogl::copyPixels(::g.pref.width3D, ::g.pref.height3D, GL_BACK);

  // saving in a jpeg file
  int jpegquality = 1;
  Img::saveJPG(filename, ::g.pref.width3D, ::g.pref.height3D, jpegquality, pixjpg);
  delete[] pixjpg;

  // rendering reinitializes buffers then we draw in the front and the back
  minirender();
}

#if 0 // notused
void Render::computeCameraProjection()
{
  GLint vp[4];

  glGetIntegerv(GL_VIEWPORT, vp);
  GLint w = vp[2];
  GLint h = vvp[3];
  GLfloat fovy = cam_user.fovy;
  GLfloat near = cam_user.near;
  GLfloat far = cam_user.far;

  echo("User=(%.1f %.1f %.1f %.1f) fovy=%.2f near=%.1f far=%.1f w=%.1f h=%.1f", localuser->pos.x, localuser->pos.y, localuser->pos.z, localuser->pos.az, fovy, near, far, w, h);
}
#endif // notused
