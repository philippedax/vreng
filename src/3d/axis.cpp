//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2008 Philippe Dax
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
//
// Author: Mario Trentini (2002)
//
#include "vreng.hpp"
#include "axis.hpp"


// local
static Axis _axis;	// axis instance

const GLfloat Axis::DEF_GLYPH = 0.04;	// 4 cm
const GLfloat Axis::DEF_WIDTH = 2;	// 2 pixels


/* Constructor */
Axis::Axis()
{
  defaults();
  glyph = DEF_GLYPH;
}

Axis::Axis(GLfloat _glyph)
{
  defaults();
  glyph = _glyph;
}

void Axis::defaults()
{
  dlist = -1;
  visible = false;
}

void Axis::reset()
{
  defaults();
}

/* Accessor */
Axis * Axis::axis()
{
  return &_axis;
}

void Axis::init()
{
  draw();
}

void Axis::draw()
{
  if (dlist != -1) glDeleteLists(dlist, 1);
  dlist = glGenLists(1);
  glNewList(dlist, GL_COMPILE);
  glBegin(GL_LINES);
   glColor3f(1, 0, 0);   glVertex3f(0, 0, 0);		// X+ light red
   glColor3f(1, .5, .5); glVertex3f(glyph, 0, 0);
   glColor3f(1, 0, 0);					// X letter red
   glVertex3f(glyph+glyph/6, glyph/12, glyph/12);
   glVertex3f(glyph+glyph/6, -glyph/12, -glyph/12);
   glVertex3f(glyph+glyph/6, glyph/12, -glyph/12);
   glVertex3f(glyph+glyph/6, -glyph/12, glyph/12);
   glColor3f(1, 0, 0);   glVertex3f(0, 0, 0);		// X- dark red
   glColor3f(.5, 0, 0);  glVertex3f(-glyph, 0, 0);

   glColor3f(0, 1, 0);   glVertex3f(0, 0, 0);		// Y+ light green
   glColor3f(.5, 1, .5); glVertex3f(0, glyph, 0);
   glColor3f(0, 1, 0);					// Y letter green
   glVertex3f(glyph/12, glyph+glyph/6, glyph/12);
   glVertex3f(0, glyph+glyph/6, 0);
   glVertex3f(-glyph/12, glyph+glyph/6, glyph/12);
   glVertex3f(0, glyph+glyph/6, 0);
   glVertex3f(0, glyph+glyph/6, 0);
   glVertex3f(0, glyph+glyph/6, -glyph/12);
   glColor3f(0, 1, 0);   glVertex3f(0, 0, 0);		// Y- dark green
   glColor3f(0, .5, 0);  glVertex3f(0, -glyph, 0);

   glColor3f(0, 0, 1);   glVertex3f(0, 0, 0);		// Z+ light blue
   glColor3f(.5, .5, 1); glVertex3f(0, 0, glyph);
   glColor3f(0, 0, 1); 					// Z letter blue
   glVertex3f(-glyph/12, glyph/12, glyph+glyph/6);
   glVertex3f(glyph/12, glyph/12, glyph+glyph/6);
   glVertex3f(glyph/12, glyph/12, glyph+glyph/6);
   glVertex3f(-glyph/12, -glyph/12, glyph+glyph/6);
   glVertex3f(-glyph/12, -glyph/12, glyph+glyph/6);
   glVertex3f(glyph/12, -glyph/12, glyph+glyph/6);
   glColor3f(0, 0, 1);   glVertex3f(0, 0, 0);		// Z- dark blue
   glColor3f(0, 0, .5);  glVertex3f(0, 0, -glyph);
  glEnd();
  glEndList();
}

void Axis::render()
{
  if (! visible) return;

  glPushMatrix();
  glDisable(GL_LIGHTING);

  // get rid of translation movement:
  GLfloat gl_mat[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, gl_mat);
  gl_mat[12] = gl_mat[13] = gl_mat[14] = 0;
  glLoadMatrixf(gl_mat);

  glLineWidth(DEF_WIDTH);
  glCallList(dlist);

  glEnable(GL_LIGHTING);
  glPopMatrix();
}

void Axis::toggle()
{
  visible ^= 1;
}
