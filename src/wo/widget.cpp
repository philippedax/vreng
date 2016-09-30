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
#include "widget.hpp"
#include "glu.hpp"	// glu alternative
#include "move.hpp"     // gotoFront


const OClass Widget::oclass(WIDGET_TYPE, "Widget", Widget::creator);


/* Creation from a file */
WObject * Widget::creator(char *l)
{
  return new Widget(l);
}

void Widget::defaults()
{
  focus = false;
  def_pixmap = NULL;
  tex_pixmap = NULL;

  // screen aspect
  tex_width = 4;
  tex_height = 3;
  u = v = 1.;
}

/* Parser */
void Widget::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
  }
  end_while_parse(l);
}

/* Behavior */
void Widget::behavior()
{
  enableBehavior(NO_ELEMENTARY_MOVE);
  enableBehavior(COLLIDE_NEVER);
  enableBehavior(SPECIFIC_RENDER);
  setRenderPrior(RENDER_NORMAL);

  initializeMobileObject(0);
  updatePosition();
}

/* Specific inits */
void Widget::inits()
{
  glGenTextures(1, &texture);	// texture number given by OpenGL
  defaultPixmap();
  setTexture(1);	// without mipmaps
  buildScreen();
}

/* Constructor */
Widget::Widget(char *l)
{
  parser(l);
  behavior();
  inits();
}

void Widget::buildScreen()
{
  if (_solids.empty()) return;	// no solid

  V3 dim;
  getDim(dim);

  float left  = MAX(dim.v[1], dim.v[0]);
  float front = MIN(dim.v[0], dim.v[1]) + 0.02;
  float top   = dim.v[2];
  float bot   = -top*0.9;
  float rsol = left / top;
  float rtex = (u * tex_width) / (v * tex_height);
  //float depth = 0.9 * front;
  float height;
  float width;

  if (rtex > rsol) {
    height = bot + (2*0.9) * left/rtex;
    width = 0.9 * left;
  }
  else {
    height = (2*0.9-1) * top;
    width = 0.9 * top * rtex;
  }

  vertices[ 0] = front;  vertices[ 1] = width;  vertices[ 2] = bot;	// bot right
  vertices[ 3] = front;  vertices[ 4] = width;  vertices[ 5] = height;	// top right
  vertices[ 6] = front;  vertices[ 7] = -width; vertices[ 8] = height;	// top left
  vertices[ 9] = front;  vertices[10] = -width; vertices[11] = bot;	// bot left
}

/* Fills the pixmap incrustated in the screen */
void Widget::defaultPixmap()
{
  def_pixmap = new GLubyte[3 * tex_width * tex_height];

  GLubyte *pix = tex_pixmap = def_pixmap;
  for (int i=0; i < tex_width; i++) {
    for (int j=0; j < tex_height; j++) {
      *pix++ = (GLubyte) 0;
      *pix++ = (GLubyte) 127;	// cyan
      *pix++ = (GLubyte) 127;
    }
  }
}

/* Set texture parameters */
void Widget::setTexture(bool mipmap)
{
  glEnable(GL_TEXTURE_2D);	// we need to use a texture
  glBindTexture(GL_TEXTURE_2D, texture);	// we use ours

  // put it into the video memory
  if (mipmap) { // with mipmap interpolation
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tex_width, tex_height,
                      GL_RGB, GL_UNSIGNED_BYTE, tex_pixmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }
  else {	// without mipmap interpolation
    glTexImage2D(GL_TEXTURE_2D, 0, 3, tex_width, tex_height,
                 0, GL_RGB, GL_UNSIGNED_BYTE, tex_pixmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }

  //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);	// bad fonts
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glDisable(GL_TEXTURE_2D);
}

/* Computes something at each loop */
void Widget::changePermanent(float lasting)
{
}

/* draw the screen */
void Widget::drawScreen()
{
  glBegin(GL_QUADS);
   glTexCoord2f(u, v); glVertex3fv(vertices+0);	// bot, right
   glTexCoord2f(u, 0); glVertex3fv(vertices+3);	// top, right
   glTexCoord2f(0, 0); glVertex3fv(vertices+6);	// top, left
   glTexCoord2f(0, v); glVertex3fv(vertices+9);	// bot, left
  glEnd();
}

/* Renders at each loop */
void Widget::render()
{
  updatePosition();

  // push
  //DAX glPushAttrib(GL_ALL_ATTRIB_BITS);
  glPushMatrix();
  glEnable(GL_LIGHTING);
  glEnable(GL_CULL_FACE);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture);

  // change system of coordonates
  M4 posmat;
  getPosition(posmat);
  GLfloat gl_mat[16];
  M4toV16(&posmat, gl_mat);
  glMultMatrixf(gl_mat);

  // render
  glRotatef(RAD2DEG(pos.az), 0, 0, 1);
  drawScreen();	// draw screen

  // update parameters for mouse handling
  GLint renderMode;
  glGetIntegerv(GL_RENDER_MODE, &renderMode);
  if (renderMode == GL_RENDER) {
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelmat);
    glGetDoublev(GL_PROJECTION_MATRIX, projmat);
  }

  // pop
  glDisable(GL_CULL_FACE);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);
  glPopMatrix();
  //DAX glPopAttrib();
}

void Widget::getCoords(int16_t &x, int16_t &y)
{
}

/* Redirects events */
bool Widget::mouseEvent(int16_t x, int16_t y, uint8_t button)
{
  return true;
}

bool Widget::keyEvent(const char *key, bool is_down)
{
  return true;
}

void Widget::quit()
{
  if (def_pixmap) delete[] def_pixmap;
}

/* Gives the focus to this object: all events will be redirected */
void Widget::takeFocus(Widget *widget, void *d, time_t s, time_t u)
{
  if (! widget->focus) {
    notice("Take Focus");
    //::g.gui.setToWidget(widget);
    widget->focus = true;
  }
}

/* Leave the focus from the object */
void Widget::leaveFocus(Widget *widget, void *d, time_t s, time_t u)
{
  if (widget->focus) {
    notice("Leave Focus");
    //::g.gui.setToWidget(NULL);
    widget->focus = false;
  }
}

void Widget::funcs()
{
  setActionFunc(WIDGET_TYPE, 0, WO_ACTION takeFocus,  "Focus");
  setActionFunc(WIDGET_TYPE, 1, WO_ACTION leaveFocus, "Navigate");
  setActionFunc(WIDGET_TYPE, 2, WO_ACTION gotoFront, "Approach");
}
