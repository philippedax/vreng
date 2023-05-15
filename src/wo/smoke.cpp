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
const float Smoke::SZ = 0.002;	//2mm orig 0.005
const float Smoke::A[NA] = { M_PI*1/4, M_PI*2/4, M_PI*3/4, M_PI, M_PI*5/4, M_PI*6/4, M_PI*7/4, 2*M_PI };
const float Smoke::COS[NA] = { SZ*cos(A[0]), SZ*cos(A[1]), SZ*cos(A[2]), SZ*cos(A[3]), SZ*cos(A[4]), SZ*cos(A[5]), SZ*cos(A[6]), SZ*cos(A[7]) };
const float Smoke::SIN[NA] = { SZ*sin(A[0]), SZ*sin(A[1]), SZ*sin(A[2]), SZ*sin(A[3]), SZ*sin(A[4]), SZ*sin(A[5]), SZ*sin(A[6]), SZ*sin(A[7]) };


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

/* creates one particle */
Smoke::Smoke(Vector3 l)
{
  //loc = l;
  vel = Vector3(0, 0.0005, 0);
  life = 255;
  dlist = -1;
  loc = Vector3(l.x, l.y, l.z);
  loc.x = pos.x;
  loc.y = pos.y;
  loc.z = pos.z;
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

  sprintf(s, "solid shape=\"bbox\" dim=\"%f %f %f\" a=\".7\" />", .05, .05, .05);
  parseSolid(s);
}

void Smoke::inits()
{
  initMobileObject(0);
  np = 0;
#if 0 //dax
  buildParticles();	// build particlesList
#endif
}

void Smoke::buildParticles()
{   
  Vector3 emit(pos.x, pos.y, pos.z);
  for (np=0; np < npmax; np++) {
    Smoke p(emit);
    particlesList.push_back(p);	// add p to particlesList
    np++;
  } 
}

void Smoke::changePermanent(float dt)
{
#if 1 //dax
  createParticle(pos.x, pos.y, pos.z);	// add one particle
#endif
  animParticles();	// update and draw particles
}

void Smoke::createParticle(float x, float y, float z)
{   
  np++;
  if (np > npmax) return;

  Vector3 emit(x, y, z);
  //Vector3 emit(pos.x, pos.y, pos.z);
  //emit.x = x;
  //emit.y = y;
  //emit.z = z;
  //Vector3 tmp = random();
  //tmp.add(emit);

  //echo("cre: %.1f %.1f %.1f", emit.x,emit.y,emit.z);
  Smoke p(emit);
  particlesList.push_back(p);	// add p to particlesList
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

#if 1 //notused
void Smoke::displayParticles()
{     
  for (vector<Smoke>::iterator it = particlesList.begin(); it < particlesList.end(); ++it) {
    if ((*it).dlist > 0) {	// is alive
      glCallList((*it).dlist);
    }
  }
} 
#endif //notused

void Smoke::update()
{
  //if (np > npmax) return;

  float x_acc = 0.000020 * (1+(-2*((float)rand())/(RAND_MAX)));	// 0.000034
  float y_acc = 0.000005 * (1+(-2*((float)rand())/(RAND_MAX)));	// 0.000010
  float z_acc = y_acc;

  acc = Vector3(x_acc, y_acc, z_acc);
  vel.add(acc);
  loc.add(vel);
  life -= 1.;
  //echo("upd: %.1f %.1f %.1f %.1f", loc.x, loc.y, loc.z, life);
}

void Smoke::render()
{
  if (np > npmax) return;

  //echo("rend: np=%d %3.1f %3.1f %3.1f (%.1f %.1f %.1f)",np,loc.x,loc.y,loc.z,pos.x,pos.y,pos.z);
  glPushMatrix();
  glTranslatef(loc.x, loc.y, loc.z);	// coord vreng
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  //glEnable(GL_BLEND);

  draw();
  //glCallList(dlist);

  //glDisable(GL_BLEND);
  glPopMatrix();
}

void Smoke::display()
{
  float a = 1.2 - life/255;
  a = MIN(a, 1);

  dlist = glGenLists(1);
  glNewList(dlist, GL_COMPILE);
  glColor4f(.9,.9,.9, a);
  glBegin(GL_POLYGON);		// octogon
  for (int i=0; i<NA; i++) {
    glVertex3f(loc.x+COS[i], loc.y+SIN[i], loc.z);
  }
  glEnd();
  glEndList();
}

void Smoke::draw()
{
  float a = 1.2 - life/255;
  a = MIN(a, 1);

  glColor4f(.9,.9,.9, a);
  glBegin(GL_POLYGON);		// octogon
  for (int i=0; i<NA; i++) {
    glVertex3f(loc.x+COS[i], loc.y+SIN[i], loc.z);
  }
  glEnd();
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
