//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
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
#include "fountain.hpp"
#include "world.hpp"	// getGround


const OClass Fountain::oclass(FOUNTAIN_TYPE, "Fountain", Fountain::creator);


/* creation from a file */
WO * Fountain::creator(char *l)
{
  return new Fountain(l);
}

void Fountain::defaults()
{
  system = FOUNTAIN;
  number = DEF_NUM;
  flow = DEF_FLOW;
  speed = DEF_SPEED;
  pt_size = DEF_PTSIZE;
  points = false;	// lines
  ground = World::current()->getGround();
  for (int i=0; i<3; i++) color[i] = 1;	// white
}

void Fountain::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if      (! stringcmp(l, "number")) l = parseUInt16(l, &number, "number");
    else if (! stringcmp(l, "flow"))   l = parseFloat(l, &flow, "flow");
    else if (! stringcmp(l, "speed"))  l = parseFloat(l, &speed, "speed");
    else if (! stringcmp(l, "ground")) l = parseFloat(l, &ground, "ground");
    else if (! stringcmp(l, "points")) l = parseBool(l, &points, "points");
    else if (! stringcmp(l, "color")) {
      l = parseVector3f(l, color, "color");
      onecolor = true;
    }
    else if (! stringcmp(l, "size")) {
      l = parseUInt8(l, &pt_size, "size");
      if (pt_size <= 0) pt_size = 1;
    }
  }
  end_while_parse(l);
}

void Fountain::geometry()
{
  char s[128];
  sprintf(s, "solid shape=\"none\" />");
  parseSolid(s);
}

Fountain::Fountain(char *l)
{
  parser(l);
  behaviors();
  geometry();
  inits();
}

#if 0 //dax done by Particle::render()
void Fountain::render()
{
  if (state == INACTIVE) return;

  glPushMatrix();
  glEnable(GL_COLOR_MATERIAL);
  glDisable(GL_LIGHTING);

  if (! onecolor) {
    color[0] = (rand()%2) ? static_cast<float>(drand48()) : 1;
    color[1] = .5 + static_cast<float>(drand48())*.5;
    color[2] = 1;
  }
  glColor3fv(color);

  if (points) {
    glPointSize(pt_size);
    glBegin(GL_POINTS); // points
    for (int n=0; n < number; n++) {
      if (! particles[n].alive) continue;  // dead
      if (particles[n].vel[2] > 0) continue;
      if (particles[n].pos[2] > pos.z && particles[n].pos[2] < ground) continue;
      glVertex3fv(particles[n].pos);
    }
    glEnd();
  }
  glEnable(GL_LIGHTING);
  glDisable(GL_COLOR_MATERIAL);
  glPopMatrix();
}
#endif

void Fountain::pause(Fountain *fountain, void *d, time_t s, time_t u)
{
  if (fountain->state == ACTIVE) fountain->state = INACTIVE;
  else fountain->state = ACTIVE;
}

void Fountain::funcs()
{
  setActionFunc(FOUNTAIN_TYPE, 0, _Action pause, "Switch");
}
