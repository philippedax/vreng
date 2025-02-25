//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2012 Philippe Dax
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
// firework
//
// Firework effect
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "firework.hpp"
#include "matvec.hpp"   // V3 M4

/*
 * Idea taken from Andrey Mirtchovski (2012)
 */

const OClass Firework::oclass(FIREWORK_TYPE, "Firework", Firework::creator);
const float Firework::DEF_TTL = 20; // 20 sec


/* creation from a file */
Object * Firework::creator(char *l)
{
  return new Firework(l);
}

void Firework::defaults()
{
  system = FIREWORK;
  number = 400;
  speed = 0;
  flow = 0;
  points = true;
  pt_size = DEF_PTSIZE;
  ttl = DEF_TTL;
  for (int i=0; i<3; i++) color[i] = 1;	// white
}

void Firework::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    else if (! stringcmp(l, "number")) l = parseUInt16(l, &number, "number");
    else if (! stringcmp(l, "ttl"))    l = parseFloat(l, &ttl, "ttl");
    else if (! stringcmp(l, "speed"))  l = parseFloat(l, &speed, "speed");
    else if (! stringcmp(l, "color")) {
      l = parseVector3f(l, color, "color");
      onecolor = true;
    }
    else if (! stringcmp(l, "size"))   l = parseUInt8(l, &pt_size, "size");
    else {
      parse()->errorAtLine(l);
      l = parse()->nextToken();
      break;
    }
  }
  end_while_parse(l);
}

void Firework::geometry()
{
  char s[256];

  sprintf(s,"solid shape=\"bsphere\" radius=\"%f\" />",1.);
  parseSolid(s);

  V3 dim;
  getDim(dim);
  w = dim.v[0];
  d = dim.v[1];
  h = dim.v[2];
}

Firework::Firework(char *l)
{
  parser(l);
  behaviors();
  geometry();
  inits();
  imposedMovement(ttl);
  generate();
}

void Firework::generate()
{
  glColor3f(1, 1, 1);
  for (int i=0; i < number; i++) {
    particles[i].pos[0] = pos.x;
    particles[i].pos[1] = pos.y;
    particles[i].pos[2] = pos.z;
    particles[i].vel[0] = speed;
    particles[i].vel[1] = speed;
    particles[i].vel[2] = speed;
    particles[i].alive  = true;
  }
  state = ACTIVE;
}

void Firework::timing(time_t sec, time_t usec, float *dt)
{
  if (! lasting(sec, usec, dt)) state = INACTIVE;
}

void Firework::imposed(float dt)
{
  for (int i=0; i < number; i++) {
    particles[i].pos[0] += particles[i].vel[0] * dt;
    particles[i].pos[1] += particles[i].vel[1] * dt;
    particles[i].pos[2] += particles[i].vel[2] * dt;
    particles[i].vel[0] -= (rand()%2 ?1:-1) * dt;
    particles[i].vel[1] -= (rand()%2 ?1:-1) * dt;
    particles[i].vel[2] -= (rand()%2 ?1:-1) * dt;
    if (fabs(particles[i].vel[0]) < 10e-3 && 
        fabs(particles[i].vel[1]) < 10e-3 && 
        fabs(particles[i].vel[2]) < 10e-3)
      particles[i].alive = false;

    if (onecolor) {
      particles[i].rgb[0] = color[0];
      particles[i].rgb[1] = color[1];
      particles[i].rgb[2] = color[2];
    }
    else {
      particles[i].rgb[0] = 1;
      particles[i].rgb[1] = static_cast<float>(drand48());
      particles[i].rgb[2] = static_cast<float>(drand48());
    }
  }
}

void Firework::render()
{
  if (state == INACTIVE) return;

  float sqr_dist = w*w + d*d + h*h;

  for (int i=0; i < number; i++) {
    if (! particles[i].alive) continue; // dead
    // check distance
    if (SQR(particles[i].pos[0] - pos.x) +
        SQR(particles[i].pos[1] - pos.y) +
        SQR(particles[i].pos[2] - pos.z) < sqr_dist) {
      glPushMatrix();
       glEnable(GL_COLOR_MATERIAL);
       glPointSize(pt_size);
       glBegin(GL_POINTS);
        glColor3fv(particles[i].rgb);
        glVertex3fv(particles[i].pos);
       glEnd();
       glDisable(GL_COLOR_MATERIAL); // if not commented bubble text not visible
      glPopMatrix();
    }
    else {
      generate();
    }
  }
}

void Firework::funcs()
{}
