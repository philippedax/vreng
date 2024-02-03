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

const float Smoke::SZ = 0.003;	//3mm orig 0.005
float const Smoke::A[NA] = { M_PI*1/4,M_PI*2/4,M_PI*3/4,M_PI,M_PI*5/4,M_PI*6/4,M_PI*7/4,2*M_PI };


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
  //for (int i=0; i < NA; i++) {
  //  A[i] = i * M_PI/4;
  //}
}

/* creates one particle */
Smoke::Smoke(Vector3 l)
{
  loc = Vector3(l.x, l.y, l.z);
  vel = Vector3(0, 0.0005, 0);
  life = 255;
  dlist = -1;
}

void Smoke::changePermanent(float dt)
{
  if (np++ > npmax) np = 0;	// regenerate the flow

  //Vector3 emit(pos.x, pos.y, pos.z);	// good position, but not rendered, FIXME!!!
  Vector3 emit(0, 0, 0);		// wrong position, but rendered,    FIXME!!!

  Smoke p(emit);		// create particle p
  psmokeList.push_back(p);	// add p to psmokeList
  animParticles();
  //update();		// update particles
  //draw();
}

void Smoke::animParticles()
{     
  for (vector<Smoke>::iterator i = psmokeList.begin(); i < psmokeList.end(); ++i) {
    if ((*i).life > 0) {	// is alive
      (*i).update();
      (*i).draw();		// why draw now ???
    }
    else {
      psmokeList.erase(i);	// erase at end of life
    }
  }
} 

void Smoke::update()
{
  float x_acc = 0.000020 * (1+(-2*(static_cast<float>(rand())/(RAND_MAX))));	// 0.000034
  float y_acc = 0.000005 * (1+(-2*(static_cast<float>(rand())/(RAND_MAX))));	// 0.000010
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

  glPushMatrix();
  glTranslatef(pos.x, pos.y, pos.z);
  for (vector<Smoke>::iterator i = psmokeList.begin(); i < psmokeList.end(); ++i) {
    if ((*i).life > 0) {	// is alive
      //echo("rend: %.1f %.1f %.1f", (*i).loc.x, (*i).loc.y, (*i).loc.z);
      glPushMatrix();
      //glLoadIdentity();
      //glLoadMatrixf(m);
      //glTranslatef((*i).loc.x, (*i).loc.y, (*i).loc.z);	//coord Vreng  bad
      //glTranslatef(-(*i).loc.y, (*i).loc.z, -(*i).loc.x);	//coord opengl good
      glTranslatef(0,0,0);	//coord opengl good
      (*i).draw();
      glPopMatrix();
    }
  }
  glPopMatrix();
}

void Smoke::draw()
{
  float a = MIN(1.2 - life/255, 1);

  glColor4f(.9,.9,.9, a);
  glPushMatrix();
  //glTranslatef(pos.x,pos.y,pos.z);
  glBegin(GL_POLYGON);		// octogon
  for (int i=0; i<NA; i++) {
    glVertex3f(loc.x+SZ*cos(A[i]), loc.y+SZ*sin(A[i]), loc.z);
  }
  glEnd();
  glPopMatrix();
}

void Smoke::funcs() {}

#if 0 //notused
void Smoke::display()
{
  float a = MIN(1.2 - life/255, 1);

  dlist = glGenLists(1);
  glNewList(dlist, GL_COMPILE);
  glColor4f(.9,.9,.9, a);
  glBegin(GL_POLYGON);		// octogon
  for (int i=0; i<NA; i++) {
    glVertex3f(loc.x+SZ*cos(A[i]), loc.y+SZ*sin(A[i]), loc.z);
  }
  glEnd();
  glEndList();
}

Vector3 Smoke::random()
{ 
  float x = -0.01+(0.02*(static_cast<float>(rand())/(RAND_MAX))); // -0.02+(0.04 (more compact)
  float y = -0.01+(0.02*(static_cast<float>(rand())/(RAND_MAX))); // -0.02+(0.04 (more compact)
  return Vector3(x, y, 0);
}
#endif //notused
