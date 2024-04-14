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


void Car::draw(GLfloat width, GLfloat depth, GLfloat height, const int textures[], GLfloat rtx[6][2], uint8_t slices, uint8_t style)
{
  GLfloat adir = 0;	//should be a parameter

  glEnable(GL_NORMALIZE);
  glPushMatrix();

  //transformations to align car with dims
  glRotatef(90, 1, 0, 0);
  glRotatef(180, 0, 1, 0);
  glTranslatef(0, 0.7, 0);	// +z
  glScalef(width/10, depth/10, height/10); // /5

  car(adir, textures, rtx, slices, style);

  glPopMatrix();
  glDisable(GL_NORMALIZE);
}

void Car::car(GLfloat adir, const int textures[], GLfloat rtx[6][2], uint8_t slices, uint8_t style)
{
  glPushMatrix();
  glScalef(10, 6, 6);
  Draw::box(1, 1, 1, textures, rtx, style);
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
  glNormal3f(4, 7, 0);
  glVertex3f(12, -1, 3); glVertex3f(5, 3, 3); glVertex3f(5, 3, -3); glVertex3f(12, -1, -3);
  glEnd();

  rouederdte(2, slices);
  rouedergche(2, slices);
  roueavtdte(1.5, adir, slices);
  roueavtgche(1.5, adir, slices);
}

void Car::rouederdte(GLfloat r, int p)
{
  glPushMatrix();
   glTranslatef(-2, -3, 3);
   cylindre(r, r, p);	//pneu
   cylindre(-6, 1, p);	//essieu
   roue(r, p);
   glTranslatef(0, 0, r);
   roue(r, p);
  glPopMatrix();
}

void Car::rouedergche(GLfloat r, int p)
{
  glPushMatrix();
   glTranslatef(-2, -3, -3);
   cylindre(r, r, p);	//pneu
   roue(r, p);
   glTranslatef(0, 0, r);
   roue(r, p);
  glPopMatrix();
}

void Car::roueavtdte(GLfloat r, GLfloat adir, int p)
{
  glPushMatrix();
   glTranslatef(8, -3.5, 3);
   glPushMatrix();
    glRotatef(adir, 0, 1, 0);
    cylindre(r, r, p);	//pneu
    roue(r, p);
    glTranslatef(0, 0, r);
    roue(r, p);
   glPopMatrix();
   cylindre(-6, 0.7, p); //essieu
  glPopMatrix();
}

void Car::roueavtgche(GLfloat r, GLfloat adir, int p)
{
  glPushMatrix();
   glTranslatef(8, -3.5, -3);
   glRotatef(adir, 0, 1, 0);
   cylindre(-r, r, p);	//pneu
   roue(r, p);
   glTranslatef(0, 0, -r);
   roue(r, p);
  glPopMatrix();
}

void Car::roue(GLfloat r, int p)
{
  for (int i=0; i < p; i++) {
    GLfloat a1 = M_2PI*i / p;
    GLfloat a2 = M_2PI*(i+1) / p;
    GLfloat grey[] = {.2, .2, .2, 0};

    //glEnable(GL_COLOR_MATERIAL);
    //glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, grey);
    glBegin(GL_TRIANGLES);
    glColor4fv(grey);
    glNormal3f(0, 0, -1); glVertex3f(r*cos(a1), r*sin(a1), 0);
    glNormal3f(0, 0, -1); glVertex3f(r*cos(a2), r*sin(a2), 0);
    glNormal3f(0, 0, -1); glVertex3f(0, 0, 0);
    glEnd();
    //glDisable(GL_COLOR_MATERIAL);
  }
}

void Car::cylindre(GLfloat height, GLfloat radius, int precision)
{
  //glEnable(GL_COLOR_MATERIAL);
  glBegin(GL_QUADS);
  for (int i=0 ; i < precision ; i++) {
    GLfloat a = M_2PI * i / precision;
    GLfloat ap = M_2PI * (i+1) / precision;
    GLfloat cosa = cos(a);
    GLfloat sina = sin(a);
    GLfloat cosap = cos(ap);
    GLfloat sinap = sin(ap);
    GLfloat grey[] = {.2, .2, .2, 0};

    //glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, grey);
    glColor4fv(grey);
    glNormal3f(cosa, sina, 0);
    glVertex3f(radius*cosa, radius*sina, 0);
    glNormal3f(cosap, sinap, 0);
    glVertex3f(radius*cosap, radius*sinap, 0);
    glVertex3f(radius*cosap, radius*sinap, height);
    glNormal3f(cosa, sina, 0);
    glVertex3f(radius*cosa, radius*sina, height);
  }
  glEnd();
  //glDisable(GL_COLOR_MATERIAL);
}

#if 0 //ev
void cylindreclose(GLfloat height, GLfloat radius, int precision)
{
  int sign;
  GLfloat a, ap, a1, a2;
  GLfloat cosa, sina, cosap, sinap;

  if (height < 0)
    sign = -1;
  else sign = 1;

  glBegin(GL_QUADS);
  for (int i=0 ; i < precision ; i++) {
    a = M_2PI * i / precision;
    ap = M_2PI * (i+1) / precision;
    cosa = cos(a);
    sina = sin(a);
    cosap = cos(ap);
    sinap = sin(ap);
    glNormal3f(cosa, sina, 0);
    glVertex3f(radius*cosa, radius*sina, 0);
    glNormal3f(cosap, sinap, 0);
    glVertex3f(radius*cosap, radius*sinap, 0);
    glVertex3f(radius*cosap, radius*sinap, height);
    glNormal3f(cosa, sina, 0);
    glVertex3f(radius*cosa, radius*sina, height);
  }
  glEnd();

  for (int i=0; i < precision; i++) {
    a1 = M_2PI * i / precision;
    a2 = M_2PI * (i+1) / precision;
    glBegin(GL_TRIANGLES);
    glNormal3f(0,0,-sign);
    glVertex3f(radius*cos(a1), radius*sin(a1), 0);
    glNormal3f(0,0,-sign);
    glVertex3f(radius*cos(a2), radius*sin(a2), 0);
    glNormal3f(0,0,-sign);
    glVertex3f(0,0,0);
    glEnd();
  }

  glTranslatef(0, 0, height);

  for (int i=0; i < precision; i++) {
    a1 = M_2PI*i / precision;
    a2 = M_2PI*(i+1) / precision;
    glBegin(GL_TRIANGLES);
    glNormal3f(0,0,sign);
    glVertex3f(radius*cos(a1), radius*sin(a1), 0);
    glNormal3f(0,0,sign);
    glVertex3f(radius*cos(a2), radius*sin(a2), 0);
    glNormal3f(0,0,sign);
    glVertex3f(0,0,0);
    glEnd();
  }
}
#endif //ev
