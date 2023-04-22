//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)       http://www.vreng.enst.fr/
//
// Copyright (C) 1997-2021 Philippe Dax
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
#include "vreng.hpp"
#include "sun.hpp"
#include "flare.hpp"	// Flare
#include "user.hpp"	// localuser
#include "solid.hpp"	// setFlary
#include "draw.hpp"	// sphere
#include "light.hpp"	// lightList


const OClass Sun::oclass(SUN_TYPE, "Sun", Sun::creator);

const GLfloat Sun::DEF_RADIUS = 4;
const GLfloat Sun::light_pos[4] = {1,0,0,0};
const GLfloat Sun::light_amb[4] = {0.05,0.05,0.05,1};


WObject * Sun::creator(char *l)
{
  return new Sun(l);
}

void Sun::defaults()
{
  scale = 1;
  radius = DEF_RADIUS;
}

void Sun::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if (! stringcmp(l, "scale"))  l = parseFloat(l, &scale, "scale");
    if (! stringcmp(l, "radius")) l = parseFloat(l, &radius, "radius");
  }
  end_while_parse(l);
}

void Sun::behaviors()
{
  enableBehavior(COLLIDE_NEVER);
  enableBehavior(PERMANENT_MOVEMENT);
  enableBehavior(SPECIFIC_RENDER);

  initMobileObject(1);
}

/** Solid geometry */
void Sun::geometry()
{
  char s[128];

  sprintf(s, "solid shape=\"sphere\" r=\"%f\" emi=\"yellow\" />", radius);
  parseSolid(s);
}

void Sun::inits()
{
  ox = pos.x;
  oy = pos.y;
  oz = pos.z;

  light_dif[0] = 1;
  light_dif[1] = 0.7;
  light_dif[2] = 0;
  rot = 0;

  flares = new Flare(4, radius*2, light_dif);
  if (solid) solid->setFlary(true);
}

Sun::Sun(char *l)
{
  parser(l);
  behaviors();
  geometry();
  inits();
  draw();
}

void Sun::draw()
{
  dlist = glGenLists(1);
  glNewList(dlist, GL_COMPILE);
  Draw::sphere(radius, 16, 16, 0);
  glEndList();
}

/* system of equations handling permanent motion */
void Sun::changePermanent(float lasting)
{
  float ang = DEG2RAD(rot);
  float sina = sin(ang);
  float cosa = cos(ang);

  pos.x = ox * cosa;
  pos.y = oy * cosa;
  pos.z = ox * sina;

  light_dif[0] = .9; //MAX(.9*sina, 0);
  light_dif[1] = .7; //MAX(.7*sina, 0);
  light_dif[2] = .2; //MAX(.2*sina, 0);
  light_spe[0] = 1;
  light_spe[1] = MAX(sina, 0);
  light_spe[2] = MAX(sina, 0);

  rot += 0.01;	// in reallife: 360/86400

  if (flares) {
    flares->setColor(light_dif);
  }
}

void Sun::render()
{
   glPushMatrix();
    glEnable(GL_LIGHTING);
    glTranslatef(pos.x, pos.y, pos.z);
    glRotatef(RAD2DEG(localuser->pos.az), 0, 0, 1);
    glScalef(scale, scale, scale);
    //glMaterialfv(GL_FRONT, GL_EMISSION, light_dif);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, light_dif);

    glCallList(dlist);
    if (flares) {
      flares->render(localuser->pos);
    }

    glDisable(GL_LIGHTING);
   glPopMatrix();
}

/* called from general renderer if in lightList */
void Sun::lighting()
{
  static float tic = 0;

#if 0 //dax
  glPushMatrix();
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT3);
   glLightfv(GL_LIGHT3, GL_POSITION, light_pos);
   glLightfv(GL_LIGHT3, GL_DIFFUSE, light_dif);
   glLightfv(GL_LIGHT3, GL_SPECULAR, light_spe);
   glMaterialfv(GL_FRONT, GL_EMISSION, light_dif);

   glRotatef(rot, -1, -1, -1);

   glDisable(GL_LIGHT3);
   glDisable(GL_LIGHTING);
  glPopMatrix();
#endif

  tic += 0.2;	// 360/86400
}

void Sun::quit()
{} 

void Sun::funcs()
{} 
