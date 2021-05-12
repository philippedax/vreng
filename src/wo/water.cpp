//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://www.vreng.enst.fr/
//
// Copyright (C) 1997-2021 Philippe Dax
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
#include "vreng.hpp"
#include "water.hpp"
#include "user.hpp"	// USER_TYPE
#include "ball.hpp"	// BALL_TYPE
#include "cauldron.hpp"	// CAULDRON_TYPE
#include "sound.hpp"	// playSound
#include "timer.hpp"	// isRate
#include "solid.hpp"	// setTransparent


const OClass Water::oclass(WATER_TYPE, "Water", Water::creator);

const uint16_t Water::RATE = 10;
const float Water::MAX_OFF = 20.;
const uint8_t Water::MESH = 64;
const float Water::DEF_TRANSP = 0.8;
const float Water::DEF_HEIGHT = 0.15;
const float Water::DEF_AMPLITUDE = 0.05;	// original: 0.03
const float Water::DEF_FREQ = 1.;		// 1 wave
const float Water::DEF_PHASE = 0.0001;		// original: 0.00003
const float Water::INCR_AMPLITUDE = 0.01;
const float Water::INCR_FREQ = 1.;		// + 1 wave
const float Water::INCR_PHASE = 0.00005;
const float Water::INCR_TRANSP = 0.05;

// local
static float tcolor[] = {.4, .7, 1, Water::DEF_TRANSP}; // triangles light blue color
static bool first = true;


/* creation from a file */
WObject * Water::creator(char *l)
{
  return new Water(l);
}

void Water::defaults()
{
  amplitude = DEF_AMPLITUDE;
  freq = DEF_FREQ;
  phase = DEF_PHASE;
  height = DEF_HEIGHT;
  transparency = DEF_TRANSP;
  first = true;
  play = true;
}

void Water::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if (! stringcmp(l, "amplitude")) {
      l = parse()->parseFloat(l, &amplitude, "amplitude");
      amplitude *= DEF_AMPLITUDE;	// coef
    }
    else if (! stringcmp(l, "freq")) {
      l = parse()->parseFloat(l, &freq, "freq");
    }
    else if (! stringcmp(l, "phase")) {
      l = parse()->parseFloat(l, &phase, "phase");
      phase *= DEF_PHASE;	// coef
    }
    else if (! stringcmp(l, "height")) {
      l = parse()->parseFloat(l, &height, "height");
    }
  }
  end_while_parse(l);
}

void Water::behavior()
{
  enableBehavior(NO_ELEMENTARY_MOVE);
  enableBehavior(LIQUID);
  enableBehavior(SPECIFIC_RENDER);
}

void Water::inits()
{
  initFluidObject(0);
  enablePermanentMovement();

  rotx = 0;
  roty = 90;
  rotz = 90;
  trace(DBG_WO, "Water: bbsize=%.2f,%.2f,%.2f bbcent=%.2f,%.2f,%.2f",
        pos.bbsize.v[0], pos.bbsize.v[1], pos.bbsize.v[2],
        pos.bbcent.v[0], pos.bbcent.v[1], pos.bbcent.v[2]);
  depth = pos.bbsize.v[0];
  width = pos.bbsize.v[1];

  this->getSolid()->setTransparent(tcolor[3]);
}

Water::Water(char *l)
{
  parser(l);
  behavior();
  inits();
}

void Water::changePermanent(float lasting)
{
  play = ::g.timer.isRate(RATE);
}

void Water::draw()
{
  float d = 1./MESH;
  for (int i=0; i < MESH; i++) {
    glBegin(GL_TRIANGLE_STRIP);
    for (int j=0; j < MESH; j++) {
      float u, v, x, y, z;
      u = j * d;
      v = i * d;
      x = -1. + 2. * u;
      z = -1. + 2. * v;
      y = amplitude * sin(freq * M_2PI * v + off);
      glTexCoord2f(u, v);
      glVertex3f(x, y, z);
      u += d;
      v += d;
      x = -1. + 2. * u;
      z = -1. + 2. * v;
      y = amplitude * sin(freq * M_2PI * v + off);
      glTexCoord2f(u, v);
      glVertex3f(x, y, z);
      if (play) {
        off += phase;
        if (off > MAX_OFF) off = 0.;
      }
    }
    glEnd();
  }
}

void Water::render()
{
  glPushMatrix();
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, tcolor);

   glTranslatef(pos.x, pos.y, pos.z + height);
   glRotatef(rotx, 1, 0, 0);
   glRotatef(roty, 0, 1, 0);
   glRotatef(rotz, 0, 0, 1);
   glScalef(width, 1, depth);

   draw();

   glDisable(GL_BLEND);
  glPopMatrix();
}

bool Water::whenIntersect(WObject *pcur, WObject *pold)
{
  switch (pcur->type) {

  case BALL_TYPE:
    pcur->pos.z += 3 * Ball::DELTAZ;
    disableBehavior(COLLIDE_ONCE);
    break;

  case USER_TYPE:
    enableBehavior(COLLIDE_ONCE);
    sound();
    break;

  case CAULDRON_TYPE:
    if (first) {
      trace(DBG_WO, "WaterC: bbsize=%.2f,%.2f,%.2f bbcent=%.2f,%.2f,%.2f",
            pos.bbsize.v[0], pos.bbsize.v[1], pos.bbsize.v[2],
            pos.bbcent.v[0], pos.bbcent.v[1], pos.bbcent.v[2]);
      first = false;
    }
    pcur->pos.z += 2 * Ball::DELTAZ;
    pcur->pos.ay = DEG2RAD((float) floor(3 * amplitude * freq * (off-MAX_OFF/2)));
    break;
  }
  pcur->updatePositionAndGrid(pcur->pos);
  return true;
}

bool Water::whenIntersectOut(WObject *pcur, WObject *pold)
{
  if (pcur->type == USER_TYPE) signal(SIGUSR2, SIG_IGN);
  return true;
}

void Water::sound_continue(int sig)
{
  signal(SIGUSR2, sound_continue);
  Sound::playSound(BUBBLESSND);
}

void Water::sound()
{
  signal(SIGUSR2, sound_continue);
  Sound::playSound(BUBBLESSND);
}

void Water::moreAmpl(Water *water, void *d, time_t s, time_t u)
{
  water->amplitude += Water::INCR_AMPLITUDE;
}

void Water::lessAmpl(Water *water, void *d, time_t s, time_t u)
{
  water->amplitude -= Water::INCR_AMPLITUDE;
  if (water->amplitude < 0.)
    water->amplitude = Water::INCR_AMPLITUDE;
}

void Water::moreFreq(Water *water, void *d, time_t s, time_t u)
{
  water->freq += Water::INCR_FREQ;
}

void Water::lessFreq(Water *water, void *d, time_t s, time_t u)
{
  water->freq -= Water::INCR_FREQ;
  if (water->freq < 0.)
    water->freq = Water::INCR_FREQ;
}

void Water::morePhase(Water *water, void *d, time_t s, time_t u)
{
  water->phase += Water::INCR_PHASE;
}

void Water::lessPhase(Water *water, void *d, time_t s, time_t u)
{
  water->phase -= Water::INCR_PHASE;
  if (water->phase < 0.)
    water->phase = Water::INCR_PHASE;
}

void Water::moreTransp(Water *water, void *d, time_t s, time_t u)
{
  water->transparency -= Water::INCR_TRANSP;
  water->transparency = MAX(water->transparency, 0);
  tcolor[3] = water->transparency;
  water->getSolid()->setTransparent(tcolor[3]);
}

void Water::lessTransp(Water *water, void *d, time_t s, time_t u)
{
  water->transparency += Water::INCR_TRANSP;
  water->transparency = MIN(water->transparency, 1);
  tcolor[3] = water->transparency;
  water->getSolid()->setTransparent(tcolor[3]);
}

void Water::reset(Water *water, void *d, time_t s, time_t u)
{
  water->amplitude = Water::DEF_AMPLITUDE;
  water->freq = Water::DEF_FREQ;
  water->phase = Water::DEF_PHASE;
  water->transparency = Water::DEF_TRANSP;
  tcolor[3] = water->transparency;
  water->getSolid()->setTransparent(tcolor[3]);
}

void Water::funcs()
{
  setActionFunc(WATER_TYPE, 0, WO_ACTION moreAmpl, "Ampl+");
  setActionFunc(WATER_TYPE, 1, WO_ACTION lessAmpl, "Ampl-");
  setActionFunc(WATER_TYPE, 2, WO_ACTION moreFreq, "Freq+");
  setActionFunc(WATER_TYPE, 3, WO_ACTION lessFreq, "Freq-");
  setActionFunc(WATER_TYPE, 4, WO_ACTION morePhase, "Phase+");
  setActionFunc(WATER_TYPE, 5, WO_ACTION lessPhase, "Phase-");
  setActionFunc(WATER_TYPE, 6, WO_ACTION moreTransp, "Transp+");
  setActionFunc(WATER_TYPE, 7, WO_ACTION lessTransp, "Transp-");
  setActionFunc(WATER_TYPE, 8, WO_ACTION reset, "Defaults");
}
