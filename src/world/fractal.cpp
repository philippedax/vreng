//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)       https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2017 Philippe Dax
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
// fractal.cpp
//
// Fractal effect
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "fractal.hpp"
#include "matvec.hpp"   // V3 M4

const OClass Fractal::oclass(FRACTAL_TYPE, "Fractal", Fractal::creator);

static const float left_alpha  =  2.0;
static const float right_alpha =  2.2;
static const float left_angle  = 20.0;
static const float right_angle = 26.0;


/** creation from a file */
Object * Fractal::creator(char *l)
{
  return new Fractal(l);
}

void Fractal::defaults()
{
  level = level_in = 14;
  for (int i=0; i<3; i++) color[i] = 1; // white
  state = INACTIVE;
  mycolor = false;
}

Fractal::Fractal(char *l)
{
  parser(l);
  behaviors();
  geometry();
  inits();
}

void Fractal::behaviors()
{
  enableBehavior(NO_ELEMENTARY_MOVE);
  enableBehavior(NO_BBABLE);
  enableBehavior(UNSELECTABLE);
  enableBehavior(SPECIFIC_RENDER);
}

void Fractal::geometry()
{
  char s[256];

  sprintf(s,"solid shape=\"bbox\" dim=\"%f %f %f\" />",1.,.25,1.);
  parseSolid(s);
}

void Fractal::inits()
{
  V3 dim;
  getDim(dim);
  width = dim.v[0];
  //depth = dim.v[1];
  height = dim.v[2];

  left_w_factor  = pow(2.0, -1.0/left_alpha);
  right_w_factor = pow(2.0, -1.0/right_alpha);
  left_h_factor  = pow(2.0, -2.0/(3*left_alpha));
  right_h_factor = pow(2.0, -2.0/(3*right_alpha));

  stillObject();
}

void Fractal::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    else if (! stringcmp(l, "level")) l = parseUInt16(l, &level_in, "level");
    else if (! stringcmp(l, "color")) { l = parseVector3f(l, color, "color"); mycolor = 1; }
  }
  end_while_parse(l);
}

void Fractal::changePosition(float dt)
{
}

void Fractal::drawbranch(float w, float x0, float z0, float x1, float z1)
{
  if (w < 1.5) {
    glLineWidth(2.);
    glBegin(GL_LINES);
     glVertex3f(x0, 0, z0);
     glVertex3f(x1, 0, z1);
    glEnd();
  }
  glLineWidth(w);
  glBegin(GL_LINES);
   glVertex3f(x0, 0, z0);
   glVertex3f(x1, 0, z1);
  glEnd();
}
  
/** Recursively called */
void Fractal::generate(float x, float z, float w, float h, float a, uint16_t l)
{
  turtle_x = x;
  turtle_z = z;
  turtle_r = h;
  step();
  float x2 = turtle_x;
  float z2 = turtle_z;
  l--;
  if (l < 3) {
    glColor3fv(color);
    drawbranch(w, x, z, x2, z2);
  }
  else {
    glColor3f(0.0, 0.0, 0.0);
    drawbranch(w, x, z, x2, z2);
  }
  if (l > 0) {
    turtle_theta = point(x, z, x2, z2);
    turn(left_angle);
    generate(turtle_x, turtle_z, left_w_factor*w, left_h_factor*h, left_angle, l);
    turtle_theta = point(x, z, x2, z2);
    turn(-right_angle);
    generate(x2, z2, left_w_factor*w, left_h_factor*h, right_angle, l);
  }
}

void Fractal::turn(float a)
{
  turtle_theta += a;
}

float Fractal::point(float x1, float z1, float x2, float z2)
{
  float theta;
  
  if ((x2 - x1) == 0.0) {
    if (z2 > z1) theta = 90.0;
    else         theta = 270.0;
  }
  else {
    theta = atan((z2-z1) / (x2-x1)) * 57.295779;
  }
  if (x1 > x2) theta += 180.0;
  return theta;
}

void Fractal::step()
{
  turtle_x += turtle_r * cos(turtle_theta*M_PI_180);
  turtle_z += turtle_r * sin(turtle_theta*M_PI_180);
}

void Fractal::render()
{
  //if (state == INACTIVE) return;
  glPushMatrix();
  glEnable(GL_LIGHTING);

  x = width/2.0;
  z = height/10.0;
  x2 = x;
  z2 = z + height;

  //echo("w=%.2f x=%.2f z=%.2f x2=%.2f z2=%.2f",width,x,z,x2,z2);
  glScalef(.4, .4, .4);
  glTranslatef(pos.x, pos.y, pos.z);
  glRotatef(RAD2DEG(pos.ax), 1, 0, 0);
  glRotatef(RAD2DEG(pos.ay), 0, 1, 0);
  glRotatef(RAD2DEG(pos.az), 0, 0, 1);

  drawbranch(width, x, z, x2, z2);

  turtle_r = height;
  turtle_theta = point(x, z, x2, z2);
  turtle_x = x;
  turtle_z = z;
  turn(left_angle);
  generate(x2, z2, left_w_factor*width, left_h_factor*height, left_angle, level);
  turtle_theta = point(x, z, x2, z2);
  turn(-right_angle);
  generate(x2, z2, left_w_factor*width, left_h_factor*height, right_angle, level);
  glDisable(GL_LIGHTING);
  glPopMatrix();
}

void Fractal::funcs()
{}
