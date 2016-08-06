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

const GLfloat Axis::DEF_LENGTH = .1;
const GLbyte  Axis::DEF_WIDTH = 2;


/* Constructor */
Axis::Axis()
{
  defaults();
  length = DEF_LENGTH;
}

Axis::Axis(GLfloat _length)
{
  defaults();
  length = _length;
}

void Axis::defaults()
{
  dlist = -1;
  visible = false;
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
   glColor3f(1, .5, .5); glVertex3f(length, 0, 0);

   glColor3f(1, 0, 0);	// X letter red
   glVertex3f(length+length/5, length/10, length/10);
   glVertex3f(length+length/5, -length/10, -length/10);
   glVertex3f(length+length/5, length/10, -length/10);
   glVertex3f(length+length/5, -length/10, length/10);

   glColor3f(1, 0, 0);   glVertex3f(0, 0, 0);		// X- dark red
   glColor3f(.5, 0, 0);  glVertex3f(-length, 0, 0);

   glColor3f(0, 1, 0);   glVertex3f(0, 0, 0);		// Y+ light green
   glColor3f(.5, 1, .5); glVertex3f(0, length, 0);

   glColor3f(0, 1, 0);	// Y letter green
   glVertex3f(length/10, length+length/5, length/10);
   glVertex3f(0, length+length/5, 0);
   glVertex3f(-length/10, length+length/5, length/10);
   glVertex3f(0, length+length/5, 0);
   glVertex3f(0, length+length/5, 0);
   glVertex3f(0, length+length/5, -length/10);

   glColor3f(0, 1, 0);   glVertex3f(0, 0, 0);		// Y- dark green
   glColor3f(0, .5, 0);  glVertex3f(0, -length, 0);

   glColor3f(0, 0, 1);   glVertex3f(0, 0, 0);		// Z+ light blue
   glColor3f(.5, .5, 1); glVertex3f(0, 0, length);

   glColor3f(0, 0, 1); 	// Z letter blue
   glVertex3f(-length/10, length/10, length+length/5);
   glVertex3f(length/10, length/10, length+length/5);
   glVertex3f(length/10, length/10, length+length/5);
   glVertex3f(-length/10, -length/10, length+length/5);
   glVertex3f(-length/10, -length/10, length+length/5);
   glVertex3f(length/10, -length/10, length+length/5);

   glColor3f(0, 0, 1);   glVertex3f(0, 0, 0);		// Z- dark blue
   glColor3f(0, 0, .5);  glVertex3f(0, 0, -length);
  glEnd();
  glEndList();
}

void Axis::render()
{
  if (! visible) return;

  glPushMatrix();
  glDisable(GL_LIGHTING);

  // get rid of translation movement:
  GLfloat matproj[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, matproj);
  matproj[12] = matproj[13] = matproj[14] = 0;
  glLoadMatrixf(matproj);

  glLineWidth(DEF_WIDTH);
  glCallList(dlist);

  glEnable(GL_LIGHTING);
  glPopMatrix();
}

void Axis::toggle()
{
  visible ^= 1;
}
