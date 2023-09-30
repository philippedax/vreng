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
const float PSmoke::SZ = 0.003;	//3mm orig 0.005
const float PSmoke::A[NA] = { M_PI*1/4, M_PI*2/4, M_PI*3/4, M_PI, M_PI*5/4, M_PI*6/4, M_PI*7/4, 2*M_PI };
const float PSmoke::COS[NA] = { SZ*cos(A[0]), SZ*cos(A[1]), SZ*cos(A[2]), SZ*cos(A[3]), SZ*cos(A[4]), SZ*cos(A[5]), SZ*cos(A[6]), SZ*cos(A[7]) };
const float PSmoke::SIN[NA] = { SZ*sin(A[0]), SZ*sin(A[1]), SZ*sin(A[2]), SZ*sin(A[3]), SZ*sin(A[4]), SZ*sin(A[5]), SZ*sin(A[6]), SZ*sin(A[7]) };


/* creation from a file */
WO * Smoke::creator(char *l)
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
PSmoke::PSmoke(Vector3 l)
{
  loc = Vector3(l.x, l.y, l.z);
  vel = Vector3(0, 0.0005, 0);
  life = 255;
  dlist = -1;
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
  enableBehavior(PARTICLE);
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
}

void Smoke::changePermanent(float dt)
{
  createParticle(pos.x, pos.y, pos.z);	// add one particle
  animParticles();	// update particles
}

void Smoke::createParticle(float x, float y, float z)
{   
  if (np++ > npmax) np = 0;	// regenerate the flow

  //Vector3 emit(x, y, z);	// good position, but not rendered, FIXME!!!
  Vector3 emit(0, 0, 0);	// wrong position, but rendered,    FIXME!!!

  PSmoke p(emit);		// create particle p
  particlesList.push_back(p);	// add p to particlesList
}

void Smoke::animParticles()
{     
  for (vector<PSmoke>::iterator i = particlesList.begin(); i < particlesList.end(); ++i) {
    if ((*i).life > 0) {	// is alive
      (*i).update();
      (*i).draw();		// why draw now ???
    }
    else {
      particlesList.erase(i);	// erase at end of life
    }
  }
} 

void PSmoke::update()
{
  float x_acc = 0.000020 * (1+(-2*((float)rand())/(RAND_MAX)));	// 0.000034
  float y_acc = 0.000005 * (1+(-2*((float)rand())/(RAND_MAX)));	// 0.000010
  float z_acc = y_acc;

  acc = Vector3(x_acc, y_acc, z_acc);
  vel.add(acc);			// vel = vel + acc
  loc.add(vel);			// loc = loc + vel
  life -= 1.;			// decrease time to live
}

void Smoke::render()
{
  //if (np > npmax) return;

  GLfloat m[16];
  m[0]=0;  m[4]=-1; m[8] =0; m[12]=0;           // Xogl = -Yvre
  m[1]=0;  m[5]=0;  m[9] =1; m[13]=-1.85;       // Yogl = Zvre
  m[2]=-1; m[6]=0;  m[10]=0; m[14]=0;           // Zogl = -Xvre
  m[3]=0;  m[7]=0;  m[11]=0; m[15]=1;

  for (vector<PSmoke>::iterator i = particlesList.begin(); i < particlesList.end(); ++i) {
    if ((*i).life > 0) {	// is alive
      //echo("rend: %.1f %.1f %.1f", (*i).loc.x, (*i).loc.y, (*i).loc.z);
      glPushMatrix();
      //glLoadIdentity();
      //glLoadMatrixf(m);
      //glTranslatef((*i).loc.x, (*i).loc.y, (*i).loc.z);	//coord Vreng  bad
      glTranslatef(-(*i).loc.y, (*i).loc.z, -(*i).loc.x);	//coord opengl good
      (*i).draw();
      glPopMatrix();
    }
  }
}

void PSmoke::draw()
{
  float a = MIN(1.2 - life/255, 1);

  glColor4f(.9,.9,.9, a);
  glBegin(GL_POLYGON);		// octogon
  for (int i=0; i<NA; i++) {
    glVertex3f(loc.x+COS[i], loc.y+SIN[i], loc.z);
  }
  glEnd();
}

void PSmoke::display()
{
  float a = MIN(1.2 - life/255, 1);

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

Vector3 Smoke::random()
{ 
  float x = -0.01+(0.02*((float)rand())/(RAND_MAX));	// -0.02+(0.04 (more compact)
  float y = -0.01+(0.02*((float)rand())/(RAND_MAX));	// -0.02+(0.04 (more compact)
  return Vector3(x, y, 0);
}

void Smoke::funcs() {}
