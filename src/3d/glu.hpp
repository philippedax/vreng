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
#ifndef GLU_HPP
#define GLU_HPP

#if !defined(HAVE_GLU)

GLint
gluProject(GLdouble objx, GLdouble objy, GLdouble objz,
           const GLdouble model[16], const GLdouble proj[16],
           const GLint viewport[4],
           GLdouble * winx, GLdouble * winy, GLdouble * winz);
GLint
gluUnProject(GLdouble winx, GLdouble winy, GLdouble winz,
             const GLdouble model[16], const GLdouble proj[16],
             const GLint viewport[4],
             GLdouble * objx, GLdouble * objy, GLdouble * objz);

void gluPerspective(GLfloat fovy, GLfloat ratio, GLfloat near, GLfloat far);

#endif

#endif
