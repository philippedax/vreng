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


void Car::draw(GLfloat width, GLfloat depth, GLfloat height, const int tex[], GLfloat rtx[6][2], uint8_t slices, uint8_t style)
{
  GLfloat a = 0;	//should be a parameter

  glEnable(GL_NORMALIZE);
  glPushMatrix();

  //transformations to align car with dims
  glRotatef(90, 1, 0, 0);
  glRotatef(180, 0, 1, 0);
  glTranslatef(0, 0.7, 0);	// +z
  glScalef(width/11, depth/11, height/11); // /5

  car(a, tex, rtx, slices, style);

  glPopMatrix();
  glDisable(GL_NORMALIZE);
}

void Car::car(GLfloat a, const int tex[], GLfloat rtx[6][2], uint8_t slices, uint8_t style)
{
  GLfloat cyan[] = { 0, 1, 1, .5 };

  glPushMatrix();
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
  //front
  glNormal3f(1, 0, 0);
  glVertex3f(12, -3, 3); glVertex3f(12, -1, 3); glVertex3f(12, -1, -3); glVertex3f(12, -3, -3);
  //front up
  glColor4fv(cyan);
  glNormal3f(4, 7, 0);
  glVertex3f(12, -1, 3); glVertex3f(5, 3, 3); glVertex3f(5, 3, -3); glVertex3f(12, -1, -3);
  glEnd();

  wheelbackright(1, slices);
  wheelbackleft(1, slices);
  wheelfronttright(.9, a, slices);
  wheelfronttleft(.9, a, slices);
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

void Car::wheelfronttright(GLfloat r, GLfloat a, int p)
{
  glPushMatrix();
   glTranslatef(8, -3.5, 3);
   glPushMatrix();
    glRotatef(a, 0, 1, 0);
    cylindre(r, r, p);	//pneu
    wheel(r, p);
    glTranslatef(0, 0, r);
    wheel(r, p);
   glPopMatrix();
   cylindre(-6, .1, p); //essieu
  glPopMatrix();
}

void Car::wheelfronttleft(GLfloat r, GLfloat a, int p)
{
  glPushMatrix();
   glTranslatef(8, -3.5, -3);
   glRotatef(a, 0, 1, 0);
   cylindre(-r, r, p);	//pneu
   wheel(r, p);
   glTranslatef(0, 0, -r);
   wheel(r, p);
  glPopMatrix();
}

void Car::wheel(GLfloat r, int p)
{
  GLfloat a1, a2;
  GLfloat grey[] = {.3, .3, .3, 0};

  for (int i=0; i < p; i++) {
    a1 = M_2PI*i / p;
    a2 = M_2PI*(i+1) / p;

    //glEnable(GL_COLOR_MATERIAL);
    //glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, grey);
    glBegin(GL_TRIANGLES);
    glColor4fv(grey);
    glNormal3f(0, 0, -1); glVertex3f(r*cos(a1), r*sin(a1), 0);
    glNormal3f(0, 0, -1); glVertex3f(r*cos(a2), r*sin(a2), 0);
    glNormal3f(0, 0, -1); glVertex3f(0, 0, 0);
    glEnd();
    //glDisable(GL_COLOR_MATERIAL);
  }
}

void Car::cylindre(GLfloat height, GLfloat r, int p)
{
  GLfloat a1, a2;
  GLfloat grey[] = {.2, .2, .2, 0};

  //glEnable(GL_COLOR_MATERIAL);
  glBegin(GL_QUADS);
  for (int i=0 ; i < p ; i++) {
    a1 = M_2PI * i / p;
    a2 = M_2PI*(i+1) / p;

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, grey);
    glColor4fv(grey);
    glNormal3f(cos(a1), sin(a1), 0);
    glVertex3f(r*cos(a1), r*sin(a1), 0);
    glNormal3f(cos(a2), sin(a2), 0);
    glVertex3f(r*cos(a2), r*sin(a2), 0);
    glVertex3f(r*cos(a2), r*sin(a2), height);
    glNormal3f(cos(a1), sin(a1), 0);
    glVertex3f(r*cos(a1), r*sin(a1), height);
  }
  glEnd();
  //glDisable(GL_COLOR_MATERIAL);
}

#if 0 //ev
void cylindreclose(GLfloat height, GLfloat r, int p)
{
  int sign;
  GLfloat a, ap, a1, a2;
  GLfloat cosa, sina, cosap, sinap;

  if (height < 0)
    sign = -1;
  else sign = 1;

  glBegin(GL_QUADS);
  for (int i=0 ; i < p ; i++) {
    a = M_2PI * i / p;
    ap = M_2PI * (i+1) / p;
    cosa = cos(a);
    sina = sin(a);
    cosap = cos(ap);
    sinap = sin(ap);
    glNormal3f(cosa, sina, 0);
    glVertex3f(r*cosa, r*sina, 0);
    glNormal3f(cosap, sinap, 0);
    glVertex3f(r*cosap, r*sinap, 0);
    glVertex3f(r*cosap, r*sinap, height);
    glNormal3f(cosa, sina, 0);
    glVertex3f(r*cosa, r*sina, height);
  }
  glEnd();

  for (int i=0; i < p; i++) {
    a1 = M_2PI * i / p;
    a2 = M_2PI * (i+1) / p;
    glBegin(GL_TRIANGLES);
    glNormal3f(0,0,-sign);
    glVertex3f(r*cos(a1), r*sin(a1), 0);
    glNormal3f(0,0,-sign);
    glVertex3f(r*cos(a2), r*sin(a2), 0);
    glNormal3f(0,0,-sign);
    glVertex3f(0,0,0);
    glEnd();
  }

  glTranslatef(0, 0, height);

  for (int i=0; i < p; i++) {
    a1 = M_2PI*i / p;
    a2 = M_2PI*(i+1) / p;
    glBegin(GL_TRIANGLES);
    glNormal3f(0,0,sign);
    glVertex3f(r*cos(a1), r*sin(a1), 0);
    glNormal3f(0,0,sign);
    glVertex3f(r*cos(a2), r*sin(a2), 0);
    glNormal3f(0,0,sign);
    glVertex3f(0,0,0);
    glEnd();
  }
}
#endif //ev
