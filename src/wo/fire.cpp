//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2011 Philippe Dax
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
#include "fire.hpp"
#include "timer.hpp"	// delta
#include "sound.hpp"	// playSound


const OClass Fire::oclass(FIRE_TYPE, "Fire", Fire::creator);

// local
static struct sParticle particles[PARTMAX];
static Vector3 Src(0,5,-1.5);
static Vector3 DS1(2,0,0);
static Vector3 DS2(0,2,0);
static struct sCyl FCyl[3];


static float hrnd(float n) { return (n*rand())/RAND_MAX; }

static float nrnd(float d) // normal distribution randomizer
{
  float r,v1,v2;
  do {
    v1=2*hrnd(1.)-1.;
    v2=2*hrnd(1.)-1.;
    r=v1*v1+v2*v2;
  } while (r >= 1. || r == 0.);
  r = sqrt(-2.*log(r)/r);
  return v1*d*r;
}


/* creation from a file */
WObject * Fire::creator(char *l)
{
  return new Fire(l);
}

void Fire::defaults()
{
  np = NBPARTS;
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
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if      (! stringcmp(l, "number")) l = parse()->parseUInt16(l, &np, "number");
    else if (! stringcmp(l, "speed"))  l = parse()->parseFloat(l, &speed, "speed");
    else if (! stringcmp(l, "sound"))  l = parse()->parseUInt8(l, &sound, "sound");
  }
  end_while_parse(l);
}

void Fire::makeSolid()
{
  if (countOfSolids() == 0) {
    char s[256];
    sprintf(s,"solid shape=\"bbox\" dim=\"%f %f %f\" />",.25,.25,.75);
    parse()->parseSolid(s, SEP, this);
  }
}

void Fire::behavior()
{
  enableBehavior(NO_ELEMENTARY_MOVE);
  enableBehavior(COLLIDE_NEVER);
  enableBehavior(SPECIFIC_RENDER);
  setRenderPrior(RENDER_HIGH); // RENDER_LOW and RENDER_NORMAL does'nt work : FIXME

  initializeMobileObject(0);
  enablePermanentMovement();
}

Fire::Fire(char *l)
{
  parser(l);
  behavior();
  makeSolid();
  inits();
}

void Fire::inits()
{
  time = 0;
  lasttime = 0;
  np = MIN(np, PARTMAX);

  V3 dim;
  getDim(dim);
  width = dim.v[0];
  depth = dim.v[1];
  height = dim.v[2];

  if (sound) {
    signal(SIGUSR2, sound_continue);
    Sound::playSound(FIRESND);
  }
  FCyl[0].r = FILECYL;
  FCyl[0].d = DS1*2.;
  FCyl[0].p = Src-FCyl[0].d*0.5;
  FCyl[0].p.z -= FILECYL;
  FCyl[1].r = FILECYL;
  FCyl[1].d = (DS1*0.85+DS2*0.5)*2.;
  FCyl[1].p = Src-FCyl[1].d*0.5;
  FCyl[1].p.z -= FILECYL;
  FCyl[2].r = FILECYL;
  FCyl[2].d = (DS1*-0.5+DS2*0.85)*2.;
  FCyl[2].p = Src-FCyl[2].d*0.5;
  FCyl[2].p.z -= FILECYL;
}

void Fire::draw()
{
  struct sParticle *p = particles;

  for (int n=0; n<np ; n++, p++)
    draw(p->ex, p->ey, p->dx, p->dx, p->a);
}

void Fire::draw(float ex, float ey, float dx, float dy, float a)
{
  float yellow[] = {1, 1, 0, 1};
  float red[]    = {1, 0, 0, 1};

  yellow[3] = a;
  red[3] = a;

  glBegin(GL_TRIANGLE_FAN);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, yellow);
   glVertex3f(ex, ex, ey);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
   glVertex3f(ex-dx, ex-dx, ey);
   glVertex3f(ex-dx*.5, ex-dx*.5, ey + M_SQRT3_2*dy);
   glVertex3f(ex+dx*.5, ex+dx*.5, ey + M_SQRT3_2*dy);
   glVertex3f(ex+dx, ex+dx, ey);
   glVertex3f(ex+dx*.5, ex+dx*.5, ey - M_SQRT3_2*dy);
   glVertex3f(ex-dx*.5, ex-dx*.5, ey - M_SQRT3_2*dy);
   glVertex3f(ex-dx, ex -dx, ey);
  glEnd();
}

void Fire::render()
{
  static uint32_t nf = 0;

  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glPushMatrix();
  glTranslatef(pos.x, pos.y, pos.z);
  float seed = ((float) drand48() * 2) - 1.;
  glRotatef(seed * 45 * (nf%4), 0, 0, 1);  // billboard effect
  glDisable(GL_CULL_FACE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glEnable(GL_BLEND);

  draw();

  glDisable(GL_BLEND);
  glPopMatrix();
  glPopAttrib();
  nf++;
}

void Fire::changePermanent(float dt)
{
  struct sParticle *p;

  time += dt;

  if (anim) {
    Vector3 a(0,0,1);
    float da = pow(0.2, dt);
    
    p = particles;
    for (int n=0; n < np; n++, p++) {
      if ((p->t -= dt) <= 0)  // kill it
        *p = particles[--np];
      else {  // animate it
        p->v += a * dt;
        p->p += p->v * dt;
        p->s.x = .67*FIRESIZE + .40*FIRESIZE*sin(M_PI*(p->t)/FIRELIFE);
        p->s.z = .05*FIRESIZE + .77*FIRESIZE*(1-p->t/FIRELIFE);
        p->a *= da;
      }
    }
    while (np < PARTMAX && (time - lasttime >= FIREDELTA)) {
      Vector3 Dir(0,0,.25);
      float f;
      lasttime += FIREDELTA;
      p = particles + np++;
      do { f = nrnd(.2); } while (ABSF(f) > .45);
      p->p = Src + FCyl[rand()%5].d * f;
      p->v = Dir;
      p->t = lasttime + FIRELIFE - time;
      p->a = FIREALPHA;
      p->s.y = FIRESIZE;
      p->s.x = .67*FIRESIZE + .40*FIRESIZE*sin(M_PI*(p->t)/FIRELIFE);
      p->s.z = .05*FIRESIZE + .77*FIRESIZE*(1-p->t/FIRELIFE);
    }
  }
  else lasttime += dt;

  Vector3 v, o;
  Matrix3 m;
  m.apply(Src, &o);
  o = Src - o;
  p = particles;
  for (int n=0; n < np; n++, p++) {
    m.apply(p->p, &v);
    v += o;
    p->ex = width * ProjEX(v);  //0.25
    p->ey = height * ProjEY(v); //0.75
    ProjEZ(p->ez, v);
    p->dx = width * p->s.x/v.y;
    p->dy = height * p->s.z/v.y;
  }

#if 0 //dax
  for (int n=0; n<4; n++) {
    Quad FGround;
    Vector3 vg = Src;
    vg.y -= 2*FILECYL;
    if ((n+1)&2) vg += DS1; else vg -= DS1;
    if ( n   &2) vg += DS2; else vg -= DS2;
    m.apply(vg, &v);
    v += o;
    FGround.v[n].ex = ProjEX(v);
    FGround.v[n].ey = ProjEY(v);
    ProjEZ(FGround.v[n].ez, v);
  }
#endif
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
  setActionFunc(FIRE_TYPE, 0, WO_ACTION sound_cb, "Sound");
  setActionFunc(FIRE_TYPE, 1, WO_ACTION stop_cb, "On/Off");
}
