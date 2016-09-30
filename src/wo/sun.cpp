//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)       http://vreng.enst.fr/
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
#include "sun.hpp"
#include "draw.hpp"	// sphere
#include "flare.hpp"	// Flare
#include "user.hpp"	// localuser
#include "light.hpp"	// lightList
#include "solid.hpp"	// setFlary


const OClass Sun::oclass(SUN_TYPE, "Sun", Sun::creator);
const GLfloat Sun::DEF_SCALE = 1;
const GLfloat Sun::DEF_RADIUS = 5;
const GLfloat Sun::light_pos[4] = {1,0,0,0};
const GLfloat Sun::light_amb[4] = {0.05,0.05,0.05,1};


WObject * Sun::creator(char *l)
{
  return new Sun(l);
}

void Sun::defaults()
{
  scale = DEF_SCALE;
  radius = DEF_RADIUS;
  light_rot = 0;
  state = ACTIVE;
  flare = NULL;
}

void Sun::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if (! stringcmp(l, "scale"))  l = parse()->parseFloat(l, &scale, "scale");
    if (! stringcmp(l, "radius")) l = parse()->parseFloat(l, &radius, "radius");
  }
  end_while_parse(l);
}

void Sun::behavior()
{
  enableBehavior(SPECIFIC_RENDER);
  //setRenderPrior(RENDER_LOW);

  initializeMobileObject(1);
  enablePermanentMovement();
#if 0
  addToInvisible();	// show flare only
#endif
}

/** Solid geometry */
void Sun::makeSolid()
{
  char s[128];
#if 1
  sprintf(s, "solid shape=\"bbox\" r=\"%f\" />", radius);
#else // FIXME segfault
  sprintf(s, "solid shape=\"sphere\" r=\"%f\" dif=\"white\" amb-\"white\" />", radius);
#endif
  parse()->parseSolid(s, SEP, this);
}

void Sun::inits()
{
  ox = pos.x;
  oy = pos.y;
  oz = pos.z;

#if 0
  addToList(lightList);
#endif

  light_diff[0] = 1;
  light_diff[1] = 0.7; //MAX(sina, 0); //0.5 + 0.5*sina;
  light_diff[2] = 0;
  flare = new Flare(4, radius*2, light_diff);
  if (solid) solid->setFlary(true);
}

Sun::Sun(char *l)
{
  parser(l);
  behavior();
  makeSolid();
  inits();
  //draw();
}

/* system of equations handling permanent motion */
void Sun::changePermanent(float lasting)
{
  float ang = DEG2RAD(light_rot);
  float sina = sin(ang);
  float cosa = cos(ang);

  light_diff[0] = MAX(0.9*(1+sina), 0); //1
  light_diff[1] = 0.7 + 0.7*sina; //MAX(sina, 0); //0.5 + 0.5*sina;
  light_diff[2] = 0.2 + 0.2*sina;
  light_spec[0] = 1;
  light_spec[1] = MAX(sina, 0);
  light_spec[2] = MAX(sina, 0);
  light_rot += 0.005;	// in reallife: 360/86400

  pos.x = ox * cosa;
  pos.y = oy * cosa;
  pos.z = ox * sina;
  //trace(DBG_FORCE, "%.1f %.1f %.1f %.1f", pos.x,pos.y,pos.z,ang);
  if (flare) flare->setColor(light_diff);
}

void Sun::render()
{
   glPushMatrix();
    glScalef(scale, scale, scale);
    glTranslatef(pos.x, pos.y, pos.z);
    glRotatef(localuser->pos.ax, 1, 0, 0);
    glRotatef(localuser->pos.ay, 0, 1, 0);
    glRotatef(localuser->pos.az, 0, 0, 1);
    if (flare) flare->render(pos);
   glPopMatrix();
}

#if 0 //dax
void Sun::draw()
{
  dlist = glGenLists(1);
  glNewList(dlist, GL_COMPILE);
  Draw::sphere(radius, 16, 16, 0);
  glEndList();
}
#endif

/* called from general renderer if in lightList */
void Sun::lighting()
{
  static float tic = 0;

#if 0 //dax
  //DAX glPushAttrib(GL_ALL_ATTRIB_BITS);
  glPushMatrix();
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT3);
   glDisable(GL_LIGHT1);
   glDisable(GL_LIGHT0);
   glLightfv(GL_LIGHT3, GL_POSITION, light_pos);
   glLightfv(GL_LIGHT3, GL_DIFFUSE, light_diff);
   glLightfv(GL_LIGHT3, GL_SPECULAR, light_spec);
   glLightfv(GL_LIGHT3, GL_AMBIENT, light_amb);
   glMaterialfv(GL_FRONT, GL_EMISSION, light_diff);

   glRotatef(light_rot, -1, -1, -1);

   glEnable(GL_LIGHT0);
   glEnable(GL_LIGHT1);
   glDisable(GL_LIGHT3);
   glDisable(GL_LIGHTING);
  glPopMatrix();
  //DAX glPopAttrib();
#endif

  tic += 0.2;	// 360/86400
}

void Sun::quit()
{
} 

void Sun::funcs()
{ } 
