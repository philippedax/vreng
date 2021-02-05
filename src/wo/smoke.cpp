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
#include "timer.hpp"	// delta


const OClass Smoke::oclass(SMOKE_TYPE, "Smoke", Smoke::creator);

// local
static float norms[3][3][3] = {
{ {TILE*D0,TILE*D1,TILE*D2}    ,{TILE*D1,TILE*D1_1,TILE*D2_1}      ,{TILE*D2,TILE*D2_1,TILE*D2_2} },
{ {TILE*D1,TILE*D1_1,TILE*D2_1},{TILE*D1_1,TILE*D1_1_1,TILE*D2_1_1},{TILE*D2_1,TILE*D2_1_1,TILE*D2_2_1} },
{ {TILE*D2,TILE*D2_1,TILE*D2_2},{TILE*D2_1,TILE*D2_1_1,TILE*D2_2_1},{TILE*D2_2,TILE*D2_2_1,TILE*D2_2_2} }
};

struct SMOKE_PARTICLE {
  Vector3 p, v;
} offset[NZ][NY][NX];

static struct sParticle particles[FIREMAX];
static float distvar[3] = {-TILE,0,TILE};
static float deplfact[1<<8];
static Vector3 src(0,5,-1.5);
static Vector3 fpart(0.,0.,4.);
static Matrix3 M;
static Vector3 O;

/* creation from a file */
WObject * Smoke::creator(char *l)
{
  return new Smoke(l);
}

void Smoke::defaults()
{
  np = FIRENB;
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
  setRenderPrior(PRIOR_MEDIUM);

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

void Smoke::inits()
{
  lasttime = 0;
  np = MIN(np, FIREMAX);
  anim = true;

  for (int i=0 ; i < (1<<8) ; i++)
    deplfact[i] = i/((float)(1<<8));

  //initialise gelly
  float t0, t1, t2, t3, tt0, tt1, tt2, tt3, ttt0, ttt1, ttt2, ttt3, xf, yf;
  float f0, f1, f2, f3, ff0, ff1, ff2, ff3, fff0, fff1, fff2, fff3;
  ttt0 = rnd2(M_2PI); f0=rnd2(ANGRND)+ANG0; ff0=rnd2(ANGRND)+ANG0; fff0=rnd2(ANGRND)+ANG0;
  ttt1 = rnd2(M_2PI); f1=rnd2(ANGRND)+ANG0; ff1=rnd2(ANGRND)+ANG0; fff1=rnd2(ANGRND)+ANG0;
  ttt2 = rnd2(M_2PI); f2=rnd2(ANGRND)+ANG0; ff2=rnd2(ANGRND)+ANG0; fff2=rnd2(ANGRND)+ANG0;
  ttt3 = rnd2(M_2PI); f3=rnd2(ANGRND)+ANG0; ff3=rnd2(ANGRND)+ANG0; fff3=rnd2(ANGRND)+ANG0;

  SMOKE_PARTICLE *p = &offset[0][0][0];
  for (int z=0; z < NZ; z++) {
    //notused zf = float(z);
    tt0 = ttt0; tt1 = ttt1; tt2 = ttt3; tt3 = ttt3;
    ttt0 += fff0; ttt1 += fff1; ttt2 += fff2; ttt3 += fff3;
    for (int y=0; y < NY; y++) {
      yf = float(y);
      t0 = tt0; t1 = tt1; t2 = tt3; t3 = tt3;
      tt0 += ff0; tt1 += ff1; tt2 += ff2; tt3 += ff3;
      for (int x=0; x < NX; x++, p++) {
        xf = float(x);
        t0 += f0; t1 += f1; t2 += f2; t3 += f3;
        p->p.x = WARPF*(sin(t2-xf)+cos(t2+yf)+sin(t0-yf)-cos(t0+xf));
        p->p.y = WARPF*(sin(t1-xf)+cos(t3+yf)+sin(t3-yf)-cos(t1+xf));
        p->p.z = WARPF*(sin(t0-xf)+cos(t2+yf)+sin(t0-yf)-cos(t2+xf));
        p->v.reset();
      }
    }
  }
}

void Smoke::changePermanent(float dt)
{
  struct sParticle *p = particles;
  float da = pow(0.2, dt);

  time += dt;
  //error("np=%d", np);
  motionAnimate(dt);
  for (int n=0; n < np; n++, p++) {
    if ((p->t -= dt) <= 0)
      *p = particles[--np]; // kill it
    else { // animate it
      p->v += fpart * dt;
      p->p += p->v * dt;
      motionWarp(p->p, dt);
      p->a *= da;
    }
  }
  lasttime += dt;
  while (np < FIREMAX && (time - lasttime >= SMOKEDELTA)) { 
    Vector3 dir(0,0,.25);
    lasttime += SMOKEDELTA;
    p = particles + (np++);
    p->p = src; // + DS1*rnd2(0.25) + DS2*rnd2(0.25);
    p->v = dir;
    motionWarp(p->p, (time - lasttime));
    p->t = SMOKELIFE + lasttime - time;
    float size = SMOKESIZE * (0.5+rnd2(0.5));
    float alpha = SMOKEALPHA * (float)pow(size/SMOKESIZE, 0.5);
    p->s.x = size;
    p->s.y = size;
    p->s.z = size;
    p->a = alpha*pow(da, (time - lasttime));
    p->s *= 0.5 + rnd2(0.5);
  }
  O.reset();
  M.apply(src, &O);
  O = src - O;
  p = particles;
  for (int n=0; n < np; n++, p++) {
    float foc = 2.;
    Vector3 v;
    M.apply(p->p, &v);
    v += O;
    if (v.y > 1) {
      p->ex = projx(v);
      p->ey = projy(v);
      projz(p->ez, v);
      p->dx = foc*p->s.x/v.y;
      p->dy = foc*p->s.z/v.y;
    }
    else p->dx = 0;
  }
}

// Handles the wobbling gel, which is used to give the WarpMap it's shape
void Smoke::motionAnimate(float dt)
{
  SMOKE_PARTICLE *p = &offset[0][0][0];
  for (int z=0; z < NZ; z++) {
    int zh = z+1; int zl = z-1;
    for (int y=0; y < NY; y++) {
      int yh = y+1; int yl = y-1;
      for (int x=0; x < NX; x++,p++) {
        int xh = x+1; int xl = x-1;
        for (int zi=zl; zi<=zh; zi++) {
          for (int yi=yl; yi<=yh; yi++) {
            for (int xi=xl; xi<=xh; xi++) {
              if ((xi != x) || (yi != y) || (zi!=z)) {
                float norm = norms[abs(zi-z)][abs(yi-y)][abs(xi-x)];
                SMOKE_PARTICLE *p2 = &offset[zi&(NZ-1)][yi&(NY-1)][xi&(NX-1)];
                Vector3 spring = p2->p;
                spring -= p->p;
                spring.x += distvar[xi-x+1];
                spring.y += distvar[yi-y+1];
                spring.z += distvar[zi-z+1];
                spring *= (norm-spring.length()) * (float)SPRINGF * dt;
                p2->v += spring;
              }
            }
          }
        }
      }
    }
  }
  p = &offset[0][0][0];
  float ft = (float) pow(DYNF, dt);
  for (int z=0; z < NZ; z++) {
    for (int y=0; y < NY; y++) {
      for (int x=0; x < NX; x++, p++) {
        p->p += p->v * dt;
        if (p->p.length2() > (MAXWARPF*MAXWARPF))
          p->v.reset();
        else
          p->v *= ft;
      }
    }
  }
}

void Smoke::motionWarp(Vector3 &p, float dt)
{
  static Vector3 v0,v1,v2,v3,v4,v5,v;

  int sx = (f2int((p.x)*(TILE*(1<<8)))+((NX/2)<<8)) & ((NX<<8)-1);
  int sy = (f2int((p.y)*(TILE*(1<<8)))+((NY/2)<<8)) & ((NY<<8)-1);
  int sz = (f2int((p.z)*(TILE*(1<<8)))) & ((NZ<<8)-1);
  float fx = deplfact[sx&((1<<8)-1)];
  float fy = deplfact[sy&((1<<8)-1)];
  float fz = deplfact[sz&((1<<8)-1)];

  sx >>= 8; //notused int x2 = (sx+1)&(NX-1);
  sy >>= 8; //notused int y2 = (sy+1)&(NY-1);
  sz >>= 8; int z2 = (sz+1)&(NZ-1);

  v0.lerp(offset[sz][sy][sx].p, offset[sz][sy][sx+1].p, fx);
  v1.lerp(offset[z2][sy][sx].p, offset[z2][sy][sx+1].p, fy);
  v.lerp(v0, v1, fz);
  v *= dt;
  p += v;
}

void Smoke::draw(float ex, float ey, float dx, float dy, float a)
{
  float white[] = {.7, .7, .7, .7};
  float grey[] = {.1, .1, .1, .1};

  white[3] = a;
  grey[3] = a;
  struct timeval tv;
  //error("draw %.2f %.2f %.2f %.2f %.2f", ex,ey,dx,dy,a);

  glBegin(GL_TRIANGLE_FAN);
    gettimeofday(&tv, NULL);
    srand((time_t) tv.tv_usec);
    if (drand48() > 0.5)
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white);
    else
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, grey);
    glVertex3f(ex, ex, ey);
    //dax gettimeofday(&tv, NULL);
    //dax srand((time_t) tv.tv_usec);
    if (drand48() > 0.5)
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white);
    else
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, grey);
    glVertex3f(ex-dx, ex-dx, ey);
    glVertex3f(ex-dx, ex-dx, ey + M_SQRT3_2*dy);
    glVertex3f(ex+dx, ex+dx, ey + M_SQRT3_2*dy);
    glVertex3f(ex+dx, ex+dx, ey);
    glVertex3f(ex+dx, ex+dx, ey - M_SQRT3_2*dy);
    glVertex3f(ex-dx, ex-dx, ey - M_SQRT3_2*dy);
    glVertex3f(ex-dx, ex-dx, ey);
  glEnd();
}

void Smoke::draw()
{
  struct sParticle *p = particles;

  for (int n=0; n<np ; n++, p++) {
    draw(p->ex, p->ey, p->dx, p->dy, p->a);
  }
}

void Smoke::render()
{
  static uint32_t ns = 0;

  //error("render: %.2f,%.2f,%.2f", pos.x,pos.y,pos.z);
  glPushMatrix();
  glTranslatef(pos.x, pos.y, pos.z);
  float seed = ((float) drand48() * 2) - 1.;
  glRotatef(seed * 45 * (ns%4), 0, 0, 1);  // billboard effect
  glDisable(GL_CULL_FACE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  //glEnable(GL_BLEND);

  draw();

  //glDisable(GL_BLEND);
  glPopMatrix();
  ns++;
}

void Smoke::funcs() {}
