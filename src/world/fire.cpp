//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2011 Philippe Dax
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
// fire.cpp
//
// Fire effect
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "fire.hpp"
#include "matvec.hpp"   // V3 M4
#include "timer.hpp"	// delta
#include "sound.hpp"	// playSound


const OClass Fire::oclass(FIRE_TYPE, "Fire", Fire::creator);

// local
static struct sParticle particles[FIREMAX];
static Vector3 src(0,5,-1.5);
static Vector3 ds1(2,0,0);
static Vector3 ds2(0,2,0);
static struct sCyl cyl[3];


static float hrnd(float n) { return (n*rand())/RAND_MAX; }

// normal distribution randomizer
static float nrnd(float d)
{
  float r,v1,v2;

  do {
    v1 = 2*hrnd(1.)-1.;
    v2 = 2*hrnd(1.)-1.;
    r = v1*v1 + v2*v2;
  } while (r >= 1. || r == 0.);
  r = sqrt(-2. * log(r)/r);
  return v1*d*r;
}


/* creation from a file */
WO * Fire::creator(char *l)
{
  return new Fire(l);
}

Fire::Fire(char *l)
{
  parser(l);
  behaviors();
  geometry();
  inits();
}

void Fire::defaults()
{
  np = FIRENB;
  speed = 1; // .5
  anim = true;
  sound = false;
  signal(SIGUSR2, SIG_IGN);
}

void Fire::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if      (! stringcmp(l, "number")) l = parseUInt16(l, &np, "number");
    else if (! stringcmp(l, "speed"))  l = parseFloat(l, &speed, "speed");
    else if (! stringcmp(l, "sound"))  l = parseUInt8(l, &sound, "sound");
  }
  end_while_parse(l);
}

void Fire::geometry()
{
  char s[128];

  sprintf(s,"solid shape=\"bbox\" dim=\"%f %f %f\" />",.25,.25,.50);
  parseSolid(s);
}

void Fire::behaviors()
{
  enableBehavior(NO_ELEMENTARY_MOVE);
  enableBehavior(COLLIDE_NEVER);
  enableBehavior(PERMANENT_MOVEMENT);
  enableBehavior(SPECIFIC_RENDER);
}

void Fire::inits()
{
  initMobileObject(0);

  nowtime = 0;
  lasttime = 0;
  np = MIN(np, FIREMAX);

  V3 dim;
  getDim(dim);
  width = dim.v[0];
  depth = dim.v[1];
  height = dim.v[2];

  if (sound) {
    signal(SIGUSR2, sound_continue);
    Sound::playSound(FIRESND);
  }
  cyl[0].r = FIRECYL;
  cyl[0].d = ds1*2.;
  cyl[0].p = src-cyl[0].d*0.5;
  cyl[0].p.z -= FIRECYL;
  cyl[1].r = FIRECYL;
  cyl[1].d = (ds1*0.85+ds2*0.5)*2.;
  cyl[1].p = src-cyl[1].d*0.5;
  cyl[1].p.z -= FIRECYL;
  cyl[2].r = FIRECYL;
  cyl[2].d = (ds1*-0.5+ds2*0.85)*2.;
  cyl[2].p = src-cyl[2].d*0.5;
  cyl[2].p.z -= FIRECYL;
}

void Fire::draw()
{
  struct sParticle *p = particles;

  for (int n=0; n < np ; n++, p++) {
    draw(p->ex, p->ey, p->dx, p->dx, p->a);
  }
}

void Fire::draw(float ex, float ey, float dx, float dy, float a)
{
  float yellow[] = {1, 1, 0, 1};
  float red[]    = {1, 0, 0, 1};

  yellow[3] = a;
  red[3] = a;
  struct timeval tv;

  glBegin(GL_TRIANGLE_FAN);
  gettimeofday(&tv, NULL);
  srand((uint32_t) tv.tv_usec);
  if (drand48() > 0.5)
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, yellow);
  else
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, red);
  glVertex3f(ex, ex, ey);
  gettimeofday(&tv, NULL);
  srand((uint32_t) tv.tv_usec);
  if (drand48() > 0.5)
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, yellow);
  else
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, red);
  glVertex3f(ex-dx, ex-dx, ey);
  glVertex3f(ex-dx*1.8, ex-dx*1.8, ey + M_SQRT3_2*dy);
  glVertex3f(ex+dx*1.8, ex+dx*1.8, ey + M_SQRT3_2*dy);
  glVertex3f(ex+dx, ex+dx, ey);
  glVertex3f(ex+dx*1.8, ex+dx*1.8, ey - M_SQRT3_2*dy);
  glVertex3f(ex-dx*1.8, ex-dx*1.8, ey - M_SQRT3_2*dy);
  glVertex3f(ex-dx, ex-dx, ey);
  glEnd();
}

void Fire::changePermanent(float dt)
{
  struct sParticle *p;

  nowtime += dt;

  if (anim) {
    Vector3 a(0, 0, 1);
    float da = pow(0.2, dt);
    
    p = particles;
    for (int n=0; n < np; n++, p++) {
      if ((p->t -= dt) <= 0)
        *p = particles[--np];  // kill it
      else {  // animate it
        p->v += a * dt;
        p->p += p->v * dt;
        p->s.x = .67*FIRESIZE + .40*FIRESIZE*sin(M_PI*(p->t)/FIRELIFE);
        p->s.z = .05*FIRESIZE + .77*FIRESIZE*(1-p->t/FIRELIFE);
        p->a *= da;
      }
    }
    while (np < FIREMAX && (nowtime - lasttime >= FIREDELTA)) {
      Vector3 speed(0, 0, .25);
      float f;
      lasttime += FIREDELTA;
      p = particles + np++;
      do { f = nrnd(.2); } while (ABSF(f) > .45);
      p->p = src + cyl[rand()%5].d * f;
      p->v = speed;
      p->t = lasttime + FIRELIFE - nowtime;
      p->a = FIREALPHA;
      p->s.y = FIRESIZE;
      p->s.x = .67*FIRESIZE + .40*FIRESIZE*sin(M_PI*(p->t)/FIRELIFE);
      p->s.z = .05*FIRESIZE + .77*FIRESIZE*(1-p->t/FIRELIFE);
    }
  }
  else {
    lasttime += dt;
  }

  Vector3 v, o;
  Matrix3 m;
  m.apply(src, &o);
  o = src - o;
  p = particles;
  for (int n=0; n < np; n++, p++) {
    m.apply(p->p, &v);
    v += o;
    p->ex = width  * projx(v); //0.25
    p->ey = height * projy(v); //0.75
    projz(p->ez, v);
    p->dx = width  * p->s.x/v.y;
    p->dy = height * p->s.z/v.y;
  }

  for (int n=0; n<4; n++) {
    Quad fground;
    Vector3 vg = src;
    vg.y -= 2*FIRECYL;
    if ((n+1)&2) vg += ds1; else vg -= ds1;
    if ( n   &2) vg += ds2; else vg -= ds2;
    m.apply(vg, &v);
    v += o;
    fground.v[n].ex = projx(v);
    fground.v[n].ey = projy(v);
    projz(fground.v[n].ez, v);
  }
}

void Fire::render()
{
  static int nf = 0;

  glPushMatrix();
  glTranslatef(pos.x, pos.y, pos.z);
  float seed = (static_cast<float>(drand48()) * 2) - 1.;
  glRotatef(seed * 45 * (nf%4), 0, 0, 1);  // billboard effect
  glDisable(GL_CULL_FACE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glEnable(GL_BLEND);

  draw();

  glDisable(GL_BLEND);
  glEnable(GL_CULL_FACE);
  glPopMatrix();

  nf++;
}

void Fire::quit()
{
  signal(SIGUSR2, SIG_IGN);
}

void Fire::sound_continue(int sig)
{
  signal(SIGUSR2, sound_continue);
  Sound::playSound(FIRESND);
}

void Fire::sound_continue()
{
  if (sound) {
    sound = false;
    signal(SIGUSR2, SIG_IGN);  // stop
  }
  else {
    sound = true;
    signal(SIGUSR2, sound_continue);
    Sound::playSound(FIRESND);
  }
}

void Fire::sound_cb(Fire *fire, void *d, time_t s, time_t u)
{
  fire->sound_continue();
}

void Fire::stop_cb(Fire *fire, void *d, time_t s, time_t u)
{
  fire->anim = (fire->anim) ? false : true;
}

void Fire::funcs()
{
  setActionFunc(FIRE_TYPE, 0, _Action stop_cb, "On/Off");
  //setActionFunc(FIRE_TYPE, 1, _Action sound_cb, "Sound");
}
