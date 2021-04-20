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
const float ParticleSmoke::S = 0.0025;	//dax 0.005
const float ParticleSmoke::PI = 3.141592;
const float ParticleSmoke::A[10] = { PI*1/5, PI*2/5, PI*3/5, PI*4/5, PI*5/5, PI*6/5, PI*7/5, PI*8/5, PI*9/5, 2*PI };
const float ParticleSmoke::COS[10] = { S*cos(A[0]), S*cos(A[1]), S*cos(A[2]), S*cos(A[3]), S*cos(A[4]), S*cos(A[5]), S*cos(A[6]), S*cos(A[7]), S*cos(A[8]), S*cos(A[9]) };
const float ParticleSmoke::SIN[10] = { S*sin(A[0]), S*sin(A[1]), S*sin(A[2]), S*sin(A[3]), S*sin(A[4]), S*sin(A[5]), S*sin(A[6]), S*sin(A[7]), S*sin(A[8]), S*sin(A[9]) };


ParticleSmoke::ParticleSmoke(Vector3 l)
{
  loc = l;
  vel = Vector3(0, 0.0005, 0);
  life = 255;
  //error("new: loc=%.2f,%.2f,%.2f", loc.x,loc.y,loc.z);
}

void ParticleSmoke::update()
{
  float x_acc = 0.000034 * (1+(-2*((float)rand())/(RAND_MAX)));	//dax 0.000034
  float y_acc = 0.00001 *  (1+(-2*((float)rand())/(RAND_MAX)));
  float z_acc = 0;	//0.0001*randomf();

  acc = Vector3(x_acc, y_acc, z_acc);
  vel.add(acc);
  loc.add(vel);
  life -= 0.5;	//dax -> 511, 0.2 -> 1277
  //error("update: %.2f %.2f %.2f %.2f", loc.x,loc.y,loc.z,life);
}

void ParticleSmoke::draw()
{
  float a = 1.2 - life/255;
  float x = loc.x; // +2.25;
  float y = loc.y; // -2.25;
  float z = loc.z; // +0.25;

  glPushMatrix();
  glColor4f(.9,.9,.9, a);
  //dax glTranslatef(x, y, z);	// vreng
  //dax glTranslatef(-y, z, -x);	// opengl
  //error("draw: %.2f %.2f %.2f %.2f", x,y,z,a);
  glBegin( GL_POLYGON ); // OR GL_LINE_LOOP
  for (int i=0; i<10; ++i) {
    glVertex3f(loc.x+COS[i], loc.y+SIN[i], loc.z);
  }
  glEnd();
  glPopMatrix();
}


/* creation from a file */
WObject * Smoke::creator(char *l)
{
  return new Smoke(l);
}

/* constructor */
Smoke::Smoke(Vector3 l)
{
  emitter = l;
  //error("smoke: emitter=%.2f %.2f %.2f", emitter.x,emitter.y,emitter.z);
}

void Smoke::defaults()
{
  np = SMOKENB;
  speed = 0.5;
}

void Smoke::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if      (!stringcmp(l, "number")) l = parse()->parseUInt16(l, &np, "number");
    else if (!stringcmp(l, "speed"))  l = parse()->parseFloat(l, &speed, "speed");
  }
  end_while_parse(l);
}

void Smoke::behavior()
{
  enableBehavior(NO_ELEMENTARY_MOVE);
  enableBehavior(COLLIDE_NEVER);
  enableBehavior(SPECIFIC_RENDER);

  initMobileObject(0);
  enablePermanentMovement();
}

Smoke::Smoke(char *l)
{
  parser(l);
  behavior();
  makeSolid();
  inits();
}

void Smoke::makeSolid()
{
  char s[128];

  sprintf(s,"solid shape=\"bbox\" dim=\"%f %f %f\" a=\"0.7\" />", 0.25, 0.25, 0.25);
  parse()->parseSolid(s, SEP, this);
}

void Smoke::inits()
{
#if 0
  Vector3 emitter(pos.x, pos.y, pos.z);

  Smoke ps(emitter);

  for (int n=0; n < np; n++) {
    addParticle();
    animParticle();
  }
  //error("smoke: inits emitter=%.2f %.2f %.2f", emitter.x,emitter.y,emitter.z);
#endif //0
}

void Smoke::changePermanent(float dt)
{
  addParticle();
  updParticle();
  drawParticle();
}

void Smoke::addParticle()
{   
  Vector3 tmp = randomv();
  tmp.add(emitter);
  //error("add: tmp=%.2f %.2f %.2f", tmp.x,tmp.y,tmp.z);
  ParticleSmoke p(tmp);
#if 0 //dax1
  p.loc.x = pos.x;
  p.loc.y = pos.y;
  p.loc.z = pos.z;
#endif //dax1
  particles.push_back(p);
}

void Smoke::updParticle()
{     
  for (vector<ParticleSmoke>::iterator it = particles.begin(); it < particles.end(); ++it) {
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
  for (vector<ParticleSmoke>::iterator it = particles.begin(); it < particles.end(); ++it) {
    if ((*it).life > 0) {	// is alive
      (*it).draw();
    }
  }
  error("[%d]", particles.size());
} 

void Smoke::animParticle()
{     
  for (vector<ParticleSmoke>::iterator it = particles.begin(); it < particles.end(); ++it) {
    if ((*it).life > 0) {	// is alive
      (*it).update();
      (*it).draw();
    }
    else {
      particles.erase(it);
    }
  }
} 

Vector3 Smoke::randomv()
{ 
  float x = -0.01+(0.02*((float)rand())/(RAND_MAX));	//dax -0.02+(0.04
  float y = -0.01+(0.02*((float)rand())/(RAND_MAX));	//dax -0.02+(0.04
  return Vector3(x, y, 0);
}

void Smoke::render()
{
  static uint32_t nf = 0;

  //error("render: %.2f %.2f %.2f", pos.x,pos.y,pos.z);
  glPushMatrix();
  //dax glTranslatef(pos.x, pos.y, pos.z);
  glTranslatef(-pos.y, pos.z, -pos.x);	// coord opengl
  //float seed = ((float) drand48() * 2) - 1.;
  //glRotatef(seed * 45 * (nf%4), 0, 0, 1);  // billboard effect
  glDisable(GL_CULL_FACE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glEnable(GL_BLEND);

  drawParticle();

  glDisable(GL_BLEND);
  glPopMatrix();
  nf++;
}

void Smoke::funcs() {}
