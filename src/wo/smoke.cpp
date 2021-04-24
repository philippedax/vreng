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
#include "wobject.hpp"


const OClass Smoke::oclass(SMOKE_TYPE, "Smoke", Smoke::creator);

// local
const float Smoke::S = 0.0025;	//dax 0.005
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
  behavior();
  makeSolid();
  inits();
}

void Smoke::defaults()
{
  np = SMOKENB;
}

void Smoke::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if (!stringcmp(l, "number")) l = parse()->parseUInt16(l, &np, "number");
  }
  end_while_parse(l);
}

void Smoke::behavior()
{
  enableBehavior(NO_ELEMENTARY_MOVE);
  enableBehavior(NO_BBABLE);
  enableBehavior(SPECIFIC_RENDER);

  initMobileObject(0);
  enablePermanentMovement();
}

void Smoke::makeSolid()
{
  char s[128];

  sprintf(s,"solid shape=\"bbox\" dim=\"%f %f %f\" a=\"0.7\" />", 0.25, 0.25, 0.25);
  parse()->parseSolid(s, SEP, this);
}

void Smoke::inits()
{
  //dax Vector3 emitter(pos.x, pos.y, pos.z);
  //error("smoke: inits emitter=%.2f %.2f %.2f", emitter.x,emitter.y,emitter.z);
}

void Smoke::changePermanent(float dt)
{
  addParticle();
  moveParticle();
  drawParticle();	// if commented no smake
}

void Smoke::addParticle()
{   
  //dax Vector3 emitter(pos.x, pos.y, pos.z);
  //error("add: emitter=%.1f %.1f %.1f", emitter.x,emitter.y,emitter.z);
  Vector3 tmp = random();
  tmp.add(emitter);
  error("add: tmp=%.1f %.1f %.1f", tmp.x,tmp.y,tmp.z);
  Smoke p(tmp);
#if 0 //dax1 bad no smoke
  p.loc.x = pos.x;
  p.loc.y = pos.y;
  p.loc.z = pos.z;
#endif //dax1

  particles.push_back(p);	// add p to particles
}

void Smoke::moveParticle()
{     
  for (vector<Smoke>::iterator it = particles.begin(); it < particles.end(); ++it) {
    if ((*it).life > 0) {	// is alive
      (*it).update();
    }
    else {
      particles.erase(it);
    }
  }
} 

void Smoke::drawParticle()
{     
  for (vector<Smoke>::iterator it = particles.begin(); it < particles.end(); ++it) {
    if ((*it).life > 0) {	// is alive
      (*it).draw();
    }
  }
  error("(%d)", particles.size());
} 

void Smoke::dlistParticle()
{     
  for (vector<Smoke>::iterator it = particles.begin(); it < particles.end(); ++it) {
    if ((*it).dlist > 0) {	// is alive
      glCallList((*it).dlist);
    }
  }
  error("[%d]", particles.size());
} 

void Smoke::animParticle()
{     
  for (vector<Smoke>::iterator it = particles.begin(); it < particles.end(); ++it) {
    if ((*it).life > 0) {	// is alive
      (*it).update();
      (*it).draw();
    }
    else {
      particles.erase(it);
    }
  }
} 

Vector3 Smoke::random()
{ 
  float x = -0.01+(0.02*((float)rand())/(RAND_MAX));	//dax -0.02+(0.04 (more compact)
  float y = -0.01+(0.02*((float)rand())/(RAND_MAX));	//dax -0.02+(0.04 (more compact)
  return Vector3(x, y, 0);
}

void Smoke::render()
{
  //error("render: %.1f %.1f %.1f", pos.x,pos.y,pos.z);
  glPushMatrix();
  glTranslatef(pos.x, pos.y, pos.z);	// coord vreng
  //dax glTranslatef(-pos.y, pos.z, -pos.x);	// coord opengl
  //dax glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  //dax glEnable(GL_BLEND);

  drawParticle();
  //dax dlistParticle();

  //dax glDisable(GL_BLEND);
  glPopMatrix();
}

Smoke::Smoke(Vector3 l)
{
  loc = l;
  vel = Vector3(0, 0.0005, 0);
  life = 255;
  dlist = -1;
  //error("new: loc=%.2f,%.2f,%.2f", loc.x,loc.y,loc.z);
}

void Smoke::update()
{
  float x_acc = 0.000034 * (1+(-2*((float)rand())/(RAND_MAX)));	//dax 0.000034
  float y_acc = 0.00001 *  (1+(-2*((float)rand())/(RAND_MAX)));
  float z_acc = y_acc;	//0

  acc = Vector3(x_acc, y_acc, z_acc);
  vel.add(acc);
  loc.add(vel);
  life -= 0.5;	//dax -> 511, 0.2 -> 1277
  //error("update: %.2f %.2f %.2f %.2f", loc.x,loc.y,loc.z,life);
}

void Smoke::draw()
{
  float a = 1.2 - life/255;	//dax 1.2

  glPushMatrix();
  //dax dlist = glGenLists(1);
  //dax glNewList(dlist, GL_COMPILE);
  glColor4f(.9,.9,.9, a);
  //glTranslatef(pos.x, pos.y, pos.z);
  glBegin(GL_POLYGON);
  for (int i=0; i < NA; ++i) {
    glVertex3f(loc.x+COS[i], loc.y+SIN[i], loc.z);
  }
  glEnd();
  //dax glEndList();
  glPopMatrix();
}

void Smoke::funcs() {}
