//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2024 Philippe Dax
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
// smoke.cpp
//
// Smoke effect
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "smoke.hpp"


const OClass Smoke::oclass(SMOKE_TYPE, "Smoke", Smoke::creator);


/** Creation from a file */
WO * Smoke::creator(char *l)
{
  return new Smoke(l);
}

/** Constructors */
Smoke::Smoke(char *l)
{
  parser(l);
  behaviors();
  geometry();
  inits();
}

/** Creates one particle */
PSmoke::PSmoke(Vector3 l, float sz)
{
  for (int i = 0; i < SMOKE_NA; i++) {
    angles[i] = i * M_PI/4;
  }
  loc = Vector3(l.x, l.y, l.z);
  size = sz;
  vel = Vector3(0, 0.0005, 0);
  life = 255;
}

void Smoke::defaults()
{
  npmax = SMOKE_NB;
  size = SMOKE_SZ;
}

/** Parses a file line */
void Smoke::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if      (! stringcmp(l, "number")) l = parseUInt16(l, &npmax, "number");
    else if (! stringcmp(l, "size"))   { l = parseFloat(l, &size, "size");	// mm
                                         size /= 1000; 				// m
                                       }
  }
  end_while_parse(l);
}

/** Sets behaviors */
void Smoke::behaviors()
{
  enableBehavior(NO_ELEMENTARY_MOVE);
  enableBehavior(NO_BBABLE);
  enableBehavior(PERMANENT_MOVEMENT);
  enableBehavior(SPECIFIC_RENDER);
  enableBehavior(PARTICLE);
}

/** Sets geometry */
void Smoke::geometry()
{
  char s[128];

  sprintf(s, "solid shape=\"bbox\" dim=\"%f %f %f\" a=\".7\" />", .05, .05, .05);
  parseSolid(s);
}

/** Do specific inits */
void Smoke::inits()
{
  initMobileObject(0);
  np = 0;
  srand(time(NULL));
}

/** When moving */
void Smoke::changePermanent(float dt)
{
  if (np++ > npmax) {
    np = 0;	// regenerate the flow
  }
  //echo("%d", np);
  //if (rand()%4 == 0) return;

  // create particle
  //Vector3 emit(pos.x, pos.y, pos.z);	// good position, but not rendered, FIXME!!!
  Vector3 emit(0, 0, 0);	// wrong position, but rendered,    FIXME!!!
  PSmoke p(emit, size);		// create particle p
  smokeList.push_back(p);	// add to smokeList

  // update
  for (std::vector<PSmoke>::iterator i = smokeList.begin(); i < smokeList.end(); ++i) {
    if ((*i).life > 0) {	// if is alive
      (*i).update();
      (*i).draw();		// why draw now ???
    }
    else {
      //echo("d");
      smokeList.erase(i);	// erase at end of life
      //delete(*i);		// delete smoke particle
    }
  }
}

/** Renders smoke particles */
void Smoke::render()
{
  //GLfloat m[16];
  //m[0]=0;  m[4]=-1; m[8] =0; m[12]=0;           // Xogl = -Yvre
  //m[1]=0;  m[5]=0;  m[9] =1; m[13]=-1.85;       // Yogl = Zvre
  //m[2]=-1; m[6]=0;  m[10]=0; m[14]=0;           // Zogl = -Xvre
  //m[3]=0;  m[7]=0;  m[11]=0; m[15]=1;

  for (std::vector<PSmoke>::iterator i = smokeList.begin(); i < smokeList.end(); ++i) {
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

void PSmoke::update()
{
  float x_acc = 0.000020 * (1+(-2*(static_cast<float>(rand())/(RAND_MAX))));	// 0.000034
  //float y_acc = 0.000002 * (1+(-2*(static_cast<float>(rand())/(RAND_MAX))));	// 0.000010
  float y_acc = 0;
  float z_acc = y_acc;

  acc = Vector3(x_acc, y_acc, z_acc);
  vel.add(acc);			// vel = vel + acc
  loc.add(vel);			// loc = loc + vel
  --life;			// decrease time to live
}

void PSmoke::draw()
{
  //float a = MIN(1.1 - life/255, 1);
  float a = life/255;
  //float a = 1 - life/255;
  //echo("%.1f",a);

  glColor4f(.7,.7,.7, a);
  glBegin(GL_POLYGON);		// octogon
  for (int i=0; i < SMOKE_NA; i++) {
    glVertex3f(loc.x + size*cos(angles[i]), loc.y + size*sin(angles[i]), loc.z);
  }
  glEnd();
}

void Smoke::funcs() {}
