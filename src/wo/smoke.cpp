//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2012 Philippe Dax
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
#include "smoke.hpp"


const OClass Smoke::oclass(SMOKE_TYPE, "Smoke", Smoke::creator);

// local
const float Smoke::S = 0.0025;	//orig 0.005
const float Smoke::PI = 3.141592;
const float Smoke::A[NA] = { PI*1/4, PI*2/4, PI*3/4, PI, PI*5/4, PI*6/4, PI*7/4, 2*PI };
const float Smoke::COS[NA] = { S*cos(A[0]), S*cos(A[1]), S*cos(A[2]), S*cos(A[3]), S*cos(A[4]), S*cos(A[5]), S*cos(A[6]), S*cos(A[7]) };
const float Smoke::SIN[NA] = { S*sin(A[0]), S*sin(A[1]), S*sin(A[2]), S*sin(A[3]), S*sin(A[4]), S*sin(A[5]), S*sin(A[6]), S*sin(A[7]) };


/* creation from a file */
WObject * Smoke::creator(char *l)
{
  return new Smoke(l);
}

/* constructors */
Smoke::Smoke(char *l)
{
  parser(l);
  behaviors();
  geometry();
  inits();
}

Smoke::Smoke(Vector3 l)
{
  loc = l;
  vel = Vector3(0, 0.0005, 0);
  life = 255;
  dlist = -1;
  loc = Vector3(pos.x, pos.y, pos.z);
  //echo("new: %.1f %.1f %.1f", loc.x,loc.y,loc.z);
}

void Smoke::defaults()
{
  npmax = SMOKENB;
}

void Smoke::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if (! stringcmp(l, "number")) l = parseUInt16(l, &npmax, "number");
  }
  end_while_parse(l);
}

void Smoke::behaviors()
{
  enableBehavior(NO_ELEMENTARY_MOVE);
  enableBehavior(NO_BBABLE);
  enableBehavior(PERMANENT_MOVEMENT);
  enableBehavior(SPECIFIC_RENDER);
}

void Smoke::geometry()
{
  char s[128];

  sprintf(s,"solid shape=\"bbox\" dim=\"%f %f %f\" a=\"0.7\" />", 0.05, 0.05, 0.05);
  parseSolid(s);
}

void Smoke::inits()
{
  initMobileObject(0);
  np = 0;
#if 0 //dax
  addParticles();
#endif
}

void Smoke::changePermanent(float dt)
{
#if 1 //dax
  addParticles();
#endif
  animParticles();
  //drawParticles();	// if commented no smake
}

void Smoke::addParticles()
{   
  Vector3 emit(pos.x, pos.y, pos.z);
//  Vector3 tmp = random();
//  tmp.add(emit);
#if 0 //dax
  for (np=0; np < npmax; np++) {
    Smoke p(emit);
    particlesList.push_back(p);	// add p to particlesList
    np++;
  } 
#else
  np++;
  if (np > npmax) return;
  Smoke p(emit);
  particlesList.push_back(p);	// add p to particlesList
#endif
}

void Smoke::updateParticles()
{     
  for (vector<Smoke>::iterator it = particlesList.begin(); it < particlesList.end(); ++it) {
    if ((*it).life > 0) {	// is alive
      (*it).update();
    }
    else {
      particlesList.erase(it);
    }
  }
} 

void Smoke::drawParticles()
{     
  for (vector<Smoke>::iterator it = particlesList.begin(); it < particlesList.end(); ++it) {
    if ((*it).life > 0) {	// is alive
      (*it).draw();
    }
  }
} 

void Smoke::dlistParticles()
{     
  for (vector<Smoke>::iterator it = particlesList.begin(); it < particlesList.end(); ++it) {
    if ((*it).dlist > 0) {	// is alive
      glCallList((*it).dlist);
    }
  }
} 

void Smoke::animParticles()
{     
  for (vector<Smoke>::iterator it = particlesList.begin(); it < particlesList.end(); ++it) {
    if ((*it).life > 0) {	// is alive
      (*it).update();
      (*it).draw();
    }
    else {
      particlesList.erase(it);
    }
  }
} 

void Smoke::render()
{
  if (np > npmax) return;

  echo("rend: np=%d %.1f %.1f %.1f", np, loc.x, loc.y, loc.z);
  glPushMatrix();
  glTranslatef(loc.x, loc.y, loc.z);	// coord vreng
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glEnable(GL_BLEND);

  draw();
  //drawParticles();
  //dax dlistParticles();

  glDisable(GL_BLEND);
  glPopMatrix();
}

void Smoke::update()
{
  float x_acc = 0.000034 * (1+(-2*((float)rand())/(RAND_MAX)));	// 0.000034
  float y_acc = 0.00001 *  (1+(-2*((float)rand())/(RAND_MAX)));
  float z_acc = y_acc;	//0

  acc = Vector3(x_acc, y_acc, z_acc);
  vel.add(acc);
  loc.add(vel);
  life -= 0.5;	// -> 511, 0.2 -> 1277
  //echo("upd: %.1f %.1f %.1f %.1f", loc.x, loc.y, loc.z, life);
}

void Smoke::draw()
{
  float a = 1.2 - life/255;	// 1.2

  //dax dlist = glGenLists(1);
  //dax glNewList(dlist, GL_COMPILE);
  glColor4f(.9,.9,.9, a);
  //glTranslatef(loc.x, loc.y, loc.z);
  glBegin(GL_POLYGON);
  for (int i=0; i < NA; i++) {
    glVertex3f(loc.x+COS[i], loc.y+SIN[i], loc.z);
  }
  glEnd();
  //dax glEndList();
}

#if 0 //notused
Vector3 Smoke::random()
{ 
  float x = -0.01+(0.02*((float)rand())/(RAND_MAX));	// -0.02+(0.04 (more compact)
  float y = -0.01+(0.02*((float)rand())/(RAND_MAX));	// -0.02+(0.04 (more compact)
  return Vector3(x, y, 0);
}
#endif //notused

void Smoke::funcs() {}
