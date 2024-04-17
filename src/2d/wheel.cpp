//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)       https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2011 Philippe Dax
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
// wheel.cpp
//
// Wheel asynchronous progress indicator
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "wheel.hpp"
#include "pref.hpp"	// ::g.pref


// local
static Wheel *pwheel;


void Wheel::defaults()
{
  needle_angle = 0;
}

/** constructor with parameters */
Wheel::Wheel(uint8_t teeth, GLfloat r, const GLfloat *color)
{
  defaults();
  pwheel = this;
  displaylist(teeth, r, color);
  needle_displaylist(teeth, r);
}

Wheel::Wheel()
{
  GLfloat color[4] = {.8,.8,.8,1};      // light grey

  defaults();
  pwheel = this;
  displaylist(12, 1, color);
  needle_displaylist(12, 1);
}

/** destructor */
Wheel::~Wheel()
{
  pwheel = NULL;
  if (wheel_dlist > 0) glDeleteLists(wheel_dlist, 1);
  if (needle_dlist > 0) glDeleteLists(needle_dlist, 1);
}

/** accessor */
Wheel * Wheel::current()
{
  return pwheel;
}

/** draws one spoke */
void Wheel::spoke(GLfloat r)
{
  GLfloat w = r / 2.5;

  glShadeModel(GL_FLAT);
  glNormal3f(0.0, 0.0, 1.0);

  glBegin(GL_QUADS);
   glVertex3f(r, - w/2, 0);
   glVertex3f(r*2, - w/2, 0);
   glVertex3f(r*2, + w/2, 0);
   glVertex3f(r, + w/2, 0);
  glEnd();
}

/** draws every spoke */
void Wheel::dial(GLfloat a, GLfloat r, const GLfloat *color)
{
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
  glPushMatrix();
   glRotatef(a, 0, 0, 1);
   spoke(r);
  glPopMatrix();
}

/** draws needles */
void Wheel::needle(uint8_t teeth, GLfloat r)
{
  GLfloat c[4] = {0,0,0,1};	// black

  for (int i=0; i<teeth ; i++) {
    switch (i) {
    case 0: case 2:
      c[3] = .7;	// basic grey
      break;
    case 1:
      c[3] = .95;	// dark grey
      break;
    default:
      c[3] = 0.2;	// light grey
    }
    float a = i * 360 / teeth;
    dial(a, r, c);
  }
}

/** needle display-list */
void Wheel::needle_displaylist(uint8_t teeth, GLfloat r)
{
  needle_dlist = glGenLists(1);
  glNewList(needle_dlist, GL_COMPILE);
  needle(teeth, r);
  glEndList();
}

/** draws a wheel */
void Wheel::draw(uint8_t teeth, GLfloat r, const GLfloat *color)
{
  GLfloat c[4];

  for (int i=0; i<4 ; i++) {
    c[i] = color[i];
  }
  for (int i=0; i<teeth ; i++) {
    float a = i * 360 / teeth;
    dial(a, r, c);
  }
  glRotatef(-90, 0, 1, 0);
}

/** wheel display-list */
void Wheel::displaylist(uint8_t teeth, GLfloat r, const GLfloat *color)
{
  wheel_dlist = glGenLists(1);
  glNewList(wheel_dlist, GL_COMPILE);
  draw(teeth, r, color);
  glEndList();
}

/** Renders */
void Wheel::render()
{
  glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT);
  glPushMatrix();
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0, (GLdouble) ::g.pref.width3D, 0, (GLdouble) ::g.pref.height3D, -1., 1.);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glEnable(GL_LIGHTING);
   glEnable(GL_BLEND);
   glDisable(GL_DEPTH_TEST);
   glRasterPos2i(300, 200);
   glRotatef(90, 1, 0, 0);
   glRotatef(-90, 0, 1, 0);

   glCallList(wheel_dlist);

   needle_angle += 5;
   glTranslatef(-0.01, 0, 0);
   glRotatef(needle_angle, 0, 0, 1);

   glCallList(needle_dlist);

   glEnable(GL_DEPTH_TEST);
   glDisable(GL_BLEND);
   glDisable(GL_LIGHTING);
   glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glPopAttrib();
}
