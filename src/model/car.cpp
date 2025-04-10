//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2008 Philippe Dax
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
// car.cpp
//
// Drawing a car
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "car.hpp"
#include "draw.hpp"	// box


/** Draws a car */
void Car::draw(GLfloat width, GLfloat depth, GLfloat height, const int tex[], GLfloat rtx[6][2], uint8_t slices, uint8_t style)
{
  //GLfloat cyan[] = { 0, 1, 1, .5 };

  glEnable(GL_NORMALIZE);
  glPushMatrix();

  // transformations to align car with dims
  glRotatef(90, 1, 0, 0);
  glRotatef(180, 0, 1, 0);
  glTranslatef(0, 0.7, 0);	// +z
  glScalef(width/11, depth/11, height/11); // /5

  glPushMatrix();
  //glMaterialfv(GL_FRONT, GL_AMBIENT, cyan);
  glScalef(10, 6, 6);
  Draw::box(1, 1, 1, tex, rtx, style);
  glPopMatrix();

  if (style == Draw::STYLE_FILL) glBegin(GL_QUADS);
  else glBegin(GL_LINE_LOOP);
  //front right
  glNormal3f(0, 0, 1);
  glVertex3f(5, 3, 3); glVertex3f(5, -3, 3); glVertex3f(12, -3, 3); glVertex3f(12, -1, 3);
  //front left
  glNormal3f(0, 0, -1);
  glVertex3f(5, 3, -3); glVertex3f(5, -3, -3); glVertex3f(12, -3, -3); glVertex3f(12, -1, -3);
  //front down
  glNormal3f(0, -1, 0);
  glVertex3f(5, -3, 3); glVertex3f(12, -3, 3); glVertex3f(12, -3, -3); glVertex3f(5, -3, -3);
  //front (motor)
  glNormal3f(1, 0, 0);
  glVertex3f(12, -3, 3); glVertex3f(12, -1, 3); glVertex3f(12, -1, -3); glVertex3f(12, -3, -3);
  //front up (pare brise)
  //glNormal3f(4, 7, 0);
  //glColor4fv(cyan);
  glVertex3f(12, -1, 3); glVertex3f(5, 3, 3); glVertex3f(5, 3, -3); glVertex3f(12, -1, -3);
  glEnd();

  wheelbackright(1.2, slices);
  wheelbackleft(1.2, slices);
  wheelfronttright(1.0, slices);
  wheelfronttleft(1.0, slices);

  glPopMatrix();
  glDisable(GL_NORMALIZE);
}

void Car::wheelbackright(GLfloat r, int p)
{
  glPushMatrix();
   glTranslatef(-2, -3, 3);
   cylindre(r, r, p);	//pneu
   cylindre(-6, .2, p);	//essieu
   wheel(r, p);
   glTranslatef(0, 0, r);
   wheel(r, p);
  glPopMatrix();
}

void Car::wheelbackleft(GLfloat r, int p)
{
  glPushMatrix();
   glTranslatef(-2, -3, -3);
   cylindre(r, r, p);	//pneu
   wheel(r, p);
   glTranslatef(0, 0, r);
   wheel(r, p);
  glPopMatrix();
}

void Car::wheelfronttright(GLfloat r, int p)
{
  glPushMatrix();
   glTranslatef(8, -3.5, 3);
   glPushMatrix();
    glRotatef(0, 0, 1, 0);
    cylindre(r, r, p);	//pneu
    wheel(r, p);
    glTranslatef(0, 0, r);
    wheel(r, p);
   glPopMatrix();
   cylindre(-6, .1, p); //essieu
  glPopMatrix();
}

void Car::wheelfronttleft(GLfloat r, int p)
{
  glPushMatrix();
   glTranslatef(8, -3.5, -3);
   glRotatef(0, 0, 1, 0);
   cylindre(-r, r, p);	//pneu
   wheel(r, p);
   glTranslatef(0, 0, -r);
   wheel(r, p);
  glPopMatrix();
}

void Car::wheel(GLfloat r, int p)
{
  GLfloat a1, a2;
  GLfloat grey30[] = {.3, .3, .3, 0};

  for (int i=0; i < p; i++) {
    a1 = M_2PI*i / p;
    a2 = M_2PI*(i+1) / p;

    glBegin(GL_TRIANGLES);
    glColor4fv(grey30);
    glNormal3f(0, 0, -1); glVertex3f(r*cos(a1), r*sin(a1), 0);
    glNormal3f(0, 0, -1); glVertex3f(r*cos(a2), r*sin(a2), 0);
    glNormal3f(0, 0, -1); glVertex3f(0, 0, 0);
    glEnd();
  }
}

void Car::cylindre(GLfloat height, GLfloat r, int p)
{
  GLfloat a1, a2;
  GLfloat grey20[] = {.2, .2, .2, 0};

  glBegin(GL_QUADS);
  for (int i=0 ; i < p ; i++) {
    a1 = M_2PI * i / p;
    a2 = M_2PI*(i+1) / p;

    glMaterialfv(GL_FRONT, GL_AMBIENT, grey20);
    //glColor4fv(grey20);
    glNormal3f(cos(a1), sin(a1), 0);
    glVertex3f(r*cos(a1), r*sin(a1), 0);
    glNormal3f(cos(a2), sin(a2), 0);
    glVertex3f(r*cos(a2), r*sin(a2), 0);
    glVertex3f(r*cos(a2), r*sin(a2), height);
    glNormal3f(cos(a1), sin(a1), 0);
    glVertex3f(r*cos(a1), r*sin(a1), height);
  }
  glEnd();
}
