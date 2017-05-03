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
#include "vreng.hpp"
#include "hud.hpp"
#include "pref.hpp"	// width3D


// local
static Hud _hud;	// hud singleton
static Hud *phud = NULL;// hud instance


/* Constructor */
Hud::Hud()
{
  visible = true;
  phud = this;
}

/* Accessor */
Hud * Hud::hud()
{
  return &_hud;
}

void Hud::toggle()
{
  visible ^= 1;
}

bool Hud::isVisible()
{
  return visible;
}

void Hud::bitmap_output(int x, int y, const char *s, void *font)
{
#if HAVE_LIBGLUT
  glRasterPos2f(x, y);
  int len = (int) strlen(s);
  for (int i=0; i < len; i++) {
    glutBitmapCharacter(font, s[i]);
  }
#endif //HAVE_LIBGLUT
}


/** text rendering */
void Hud::render(GLfloat r, GLfloat g, GLfloat b, GLfloat a, void *font, GLint x, GLint y, const char *s, ...)
{
  if (! visible) return;

  va_list ap;
  char buf[128];

  va_start(ap, s);
  vsprintf(buf, s, ap);
  va_end(ap);

  glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
   glLoadIdentity();
   glOrtho(0, (GLdouble) ::g.pref.width3D, 0, (GLdouble) ::g.pref.height3D, -1., 1.);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glColor4f(r, g, b, a);

   bitmap_output(x, y, buf, font);

   glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glPopAttrib();
}

void Hud::render(GLfloat r, GLfloat g, GLfloat b, GLfloat a, GLuint font, GLint x, GLint y, const char *s, ...)
{
  if (! visible) return;

  va_list ap;
  char buf[128];

  va_start(ap, s);
  vsprintf(buf, s, ap);
  va_end(ap);

  glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
   glLoadIdentity();
   glOrtho(0, (GLdouble) ::g.pref.width3D, 0, (GLdouble) ::g.pref.height3D, -1., 1.);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glColor4f(r, g, b, a);
   glRasterPos2i(x, y);

   glListBase(font);
   glCallLists(strlen(buf), GL_UNSIGNED_BYTE, buf);

   glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glPopAttrib();
}
