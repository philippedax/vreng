//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2009 Philippe Dax
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
/*
    particle.c
    Nate Robins, 1997
    An example of a simple particle system.
 */

#include "vreng.hpp"
#include "particle.hpp"
#include "timer.hpp"	// getRate diffDates
#include "world.hpp"	// getGround


const OClass Particle::oclass(PARTICLE_TYPE, "Particle", Particle::creator);

const uint16_t Particle::DEF_NUM = 5000; //5000;
const float Particle::DEF_FLOW = 500;
const float Particle::DEF_SPEED = 1.;
const uint8_t Particle::DEF_PTSIZE = 1;
const float Particle::DEF_GRAVITY = -9.8;

// local
static struct timeval begintime;


/* creation from a file */
WObject * Particle::creator(char *l)
{
  return new Particle(l);
}

void Particle::defaults()
{
  number = DEF_NUM;
  flow = DEF_FLOW;
  system = WATERFALL;
  speed = DEF_SPEED;
  pt_size = DEF_PTSIZE;
  mycolor = false;
  living = 0;
  ground = World::current()->getGround();
  for (int i=0; i<3; i++) color[i] = 1;	// default white
}

void Particle::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if (!stringcmp(l, "system")) {
      char systemstr[16];
      l = parse()->parseString(l, systemstr, "system");
      if      (! stringcmp(systemstr, "fountain")) system = FOUNTAIN;
      else if (! stringcmp(systemstr, "firework")) system = FIREWORK;
      else if (! stringcmp(systemstr, "rain"))     system = RAIN;
      else if (! stringcmp(systemstr, "snow"))     system = SNOW;
      else                                          system = WATERFALL;
    }
    else if (!stringcmp(l, "number")) l = parse()->parseUInt16(l, &number, "number");
    else if (!stringcmp(l, "flow"))   l = parse()->parseFloat(l, &flow, "flow");
    else if (!stringcmp(l, "speed"))  l = parse()->parseFloat(l, &speed, "speed");
    else if (!stringcmp(l, "ground")) l = parse()->parseFloat(l, &ground, "ground");
    else if (!stringcmp(l, "size"))   l = parse()->parseUInt8(l, &pt_size, "size");
    else if (!stringcmp(l, "color")) {l = parse()->parseVector3f(l, color, "color"); mycolor = 1; }
  }
  end_while_parse(l);
}

void Particle::behavior()
{
  enableBehavior(PARTICLE);
  enableBehavior(NO_ELEMENTARY_MOVE);
  enableBehavior(NO_BBABLE);
  enableBehavior(UNSELECTABLE);
  enableBehavior(SPECIFIC_RENDER);
  setRenderPrior(RENDER_LOW);

  initMobileObject(0);
  switch (system) {
  case FIREWORK:
    break;
  default:     
    enablePermanentMovement();
    break;
  }
}

/* constructor for sub-classes */
Particle::Particle()
{
  living = 0;
  state = ACTIVE;
}

Particle::Particle(char *l)
{
  parser(l);
  behavior();
  inits();
}

void Particle::inits()
{
  particles = new tParticle[number];
  for (int n=0; n < number; n++) {
    particles[n].pos[0]  = 0;
    particles[n].pos[1]  = 0;
    particles[n].pos[2]  = 0;
    particles[n].prev[0] = 0;
    particles[n].prev[1] = 0;
    particles[n].prev[2] = 0;
    particles[n].vel[0]  = 0;
    particles[n].vel[1]  = 0;
    particles[n].vel[2]  = 0;
    particles[n].damp    = 0;
    particles[n].alive   = false;
  }

  gettimeofday(&begintime, NULL);
}

/* timedelta: returns the number of seconds that have elapsed since
   the previous call to the function. */
float Particle::timedelta()
{
  struct timeval endtime;
  gettimeofday(&endtime, NULL);
  srand((time_t) endtime.tv_usec);

  double diff = Times::diffDates(begintime, endtime);
  begintime = endtime;
  return (float)diff;
}

void Particle::generate(tParticle *p, float dt)
{
  p->pos[0] = pos.x;
  p->pos[1] = pos.y;
  p->pos[2] = pos.z;
  p->prev[0] = p->pos[0];
  p->prev[1] = p->pos[1];
  p->prev[2] = p->pos[2];

  switch (system) {
  case WATERFALL:
    points = true;
    p->vel[0] = 2*((float) drand48()-.5);
    p->vel[1] = 2*((float) drand48()-.5);
    p->vel[2] = 0;
    p->damp = .45*(float) drand48();
    break;
  case FOUNTAIN:
    points = true;
    p->vel[0] = 2*((float) drand48()-.5);
    p->vel[1] = 2*((float) drand48()-.5);
    p->vel[2] = .75*speed;
    p->damp = .35*(float) drand48();
    break;
  case FIREWORK:
    points = true;
    p->vel[0] = 5*((float) drand48()-.5);
    p->vel[1] = 5*((float) drand48()-.5);
    p->vel[2] = 5*((float) drand48()-.5);
    p->damp = .35*(float) drand48();
    break;
  case SNOW:
    points = true;
    p->vel[0] = 3*((float) drand48()-.5);
    p->vel[1] = 3*((float) drand48()-.5);
    p->vel[2] = .2*speed;
    p->damp = .25*(float) drand48();
    break;
  case RAIN:
    points = false;
    p->vel[0] = 4*((float) drand48()-.5);
    p->vel[1] = 4*((float) drand48()-.5);
    p->vel[2] = 2*speed;
    p->damp = .15*(float) drand48();
    break;
  }
  p->alive = true;
  timestep(p, dt * (float) drand48());
}

/**
   bounce: the particle has gone past (or exactly hit) the ground
   plane, so calculate the time at which the particle actually
   intersected the ground plane (s).  essentially, this just rolls
   back time to when the particle hit the ground plane, then starts
   time again from then.
   -  -   o A  (previous position)
   |  |    \
   |  s     \   o  (position it _should_ be at) -
   t  |      \ /                                | t - s
   |  - ------X--------                         -
   |           \
   -            o B  (new position)
   A + V*s = 0 or s = -A/V
   to calculate where the particle should be:
   A + V*t + V*(t-s)*d
   where d is a damping factor which accounts for the loss of energy due to the bounce.
**/
void Particle::bounce(tParticle *p, float dt)
{
  if (! p->alive) return;  // dead

  // since we know it is the ground plane, we only need to calculate s for a single dimension.
  float s = -p->prev[2] / p->vel[2];

  p->pos[0] = (p->prev[0] + p->vel[0] * s + p->vel[0] * (dt-s) * p->damp);
  p->pos[1] = (p->prev[1] + p->vel[1] * s + p->vel[1] * (dt-s) * p->damp);
  p->pos[2] = (p->prev[2] + p->vel[2] * s + p->vel[2] * (dt-s) * p->damp);

  // damp the reflected velocity (since the particle hit something, it lost some energy)
  p->vel[0] *=  p->damp;
  p->vel[1] *=  p->damp;
  p->vel[2] *= -p->damp;  // reflect
}

bool Particle::fequal(float a, float b)
{
  return (fabs(a - b) < 10e-3) ? true : false;
}

void Particle::timestep(tParticle *p, float dt)
{
  if (! p->alive) return;  // dead

  p->vel[0] += 0;
  p->vel[1] += 0;
  p->vel[2] -= 2*dt;  // slow down
  p->prev[0] = p->pos[0];
  p->prev[1] = p->pos[1];
  p->prev[2] = p->pos[2];
  if ((p->pos[2] + p->vel[2] * dt) > (ground + 0.005)) {
    p->pos[0] += p->vel[0] * dt * speed/2;
    p->pos[1] += p->vel[1] * dt * speed/2;
    p->pos[2] += p->vel[2] * dt * speed/2;
  }
}

void Particle::regenerate(float dt)
{
  // slow the simulation if we can't keep the frame rate up around 10 fps
  if (dt > 0.1) speed *= 0.75;
  else          speed *= 1;
  dt *= speed;

  // resurrect a few particles
  for (int i=0; i < (flow * dt); i++) {
    generate(&particles[living], dt);
    if (++living >= number) living = 0;
  }

  // expire some particles
  for (int n=0; n < number; n++) {
    timestep(&particles[n], dt);

    // collision with ground ?
    if (particles[n].pos[2] <= ground)
      //bounce(&particles[n], dt);

    // dead particle ?
    if (particles[n].pos[2] < (ground + 0.005)) {
      //particles[n].alive = false;  // death
    }
    if (fequal(particles[n].vel[2], 0)) {
      particles[n].alive = false;  // death
      //generate(&particles[n], dt);
    }
  }
}

void Particle::changePermanent(float dt)
{
  if (state == INACTIVE) return;
  regenerate(dt);
}

void Particle::render()
{
  if (state == INACTIVE) return;

  glPushMatrix();
#if 0 //FOG
  float black[] = { 0, 0, 0, 1 };
  glFogfv(GL_FOG_COLOR, black);
  glFogf(GL_FOG_START, 2.5);
  glFogf(GL_FOG_END, 4);
  glFogi(GL_FOG_MODE, GL_LINEAR);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_FOG);
#endif
  glEnable(GL_COLOR_MATERIAL);
  glDisable(GL_LIGHTING);

  if (mycolor) {
    glColor3fv(color);
  }
  else {
    color[0] = (rand()%2) ? (float) drand48() : 1;
    color[1] = .5 + (float) drand48()*.5;
    color[2] = 1;
    glColor3fv(color);
  }
  if (points) {
    glPointSize(pt_size);
    glBegin(GL_POINTS);	// points
    for (int n=0; n < number; n++) {
      if (! particles[n].alive) continue;  // dead
      //dax if (particles[n].vel[2] > 0) continue;
      if (particles[n].pos[2] > pos.z && particles[n].pos[2] < ground) continue;
      glVertex3fv(particles[n].pos);
    }
    glEnd();
  }
  else { // lines
    glLineWidth(pt_size);
    //glLineStipple(1, 0xF0F0);
    glBegin(GL_LINES);	// segments
    //glEnable(GL_LINE_STIPPLE);
    for (int n=0; n < number; n++) {
      if (! particles[n].alive) continue;  // dead
      //dax if (particles[n].vel[2] > 0) continue;
      if (particles[n].pos[2] > pos.z && particles[n].pos[2] < ground) continue;
      glVertex3fv(particles[n].prev);
      glVertex3fv(particles[n].pos);
    }
    //glDisable(GL_LINE_STIPPLE);
    glEnd();
  }
  glEnable(GL_LIGHTING);
  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_FOG);
  glPopMatrix();
}

void Particle::quit()
{
  if (particles) delete[] particles;
}

void Particle::funcs() {}
