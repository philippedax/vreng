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
#include "bird.hpp"
#include "wings.hpp"


const OClass Bird::oclass(BIRD_TYPE, "Bird", Bird::creator);

const float Bird::BIRD_SCALE = 0.3;
const float Bird::BIRD_ZONE = 3;	// flying zone
const float Bird::BIRD_DELTA = .005;	// elem motion


/* Creation from a file */
WObject * Bird::creator(char *l)
{
  return new Bird(l);
}

void Bird::defaults()
{
  flying = false;
  model = Wings::BIRD;
  wings = NULL;
  radius = BIRD_ZONE;
  scale = BIRD_SCALE;
}

/* Parser */
void Bird::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if (! stringcmp(l, "model=")) {
      l = parse()->parseString(l, modelname, "model");
      if      (! stringcmp(modelname, "bird"))      model = Wings::BIRD;
      else if (! stringcmp(modelname, "butterfly")) model = Wings::BUTTERFLY;
    }
    else if (! stringcmp(l, "radius")) {
      l = parse()->parseFloat(l, &radius, "radius");
    }
    else if (! stringcmp(l, "scale")) {
      l = parse()->parseFloat(l, &scale, "scale");
    }
    else if (! stringcmp(l, "flying")) {
      l = parse()->parseBool(l, &flying, "flying");
    }
  }
  end_while_parse(l);
}

/* Behavior */
void Bird::behavior()
{
  enableBehavior(NO_ELEMENTARY_MOVE);
  enableBehavior(COLLIDE_NEVER);
  enableBehavior(SPECIFIC_RENDER);

  initMobileObject(0);
}

/* Specific inits */
void Bird::inits()
{
  posinit = pos;
  wings = new Wings(model, scale);
  pos.x += rand()%3 * BIRD_DELTA;
  pos.y += rand()%3 * BIRD_DELTA;
  pos.z += rand()%3 * BIRD_DELTA;
  //pos.ay += M_PI_2;
  updatePosition();

  if (flying) {
    fly();
  }
}

/* Constructor */
Bird::Bird(char *l)
{
  parser(l);
  behavior();
  inits();
}

/* Computes position at each loop */
void Bird::changePermanent(float lasting)
{
  static bool expansionx = true;
  static bool expansiony = true;
  static bool expansionz = true;
  static int signx = -1;
  static int signy = 1;
  static int signz = 1;

  // x
  if (expansionx) {
    signx = -1;
    if ( (pos.x < (posinit.x - radius)) || (pos.x > (posinit.x + radius)) ) {
      expansionx = false;
      signx = 1;
    }
  }
  else { // collapsex
    signx = 1;
    if ( (pos.x < (posinit.x - radius)) || (pos.x > (posinit.x + radius)) ) {
      expansionx = true;
      signx = -1;
    }
  }
  pos.x += (signx * rand()%3 * BIRD_DELTA);

  // y
  if (expansiony) {
    signy = 1;
    if ( (pos.y < (posinit.y - radius)) || (pos.y > (posinit.y + radius)) ) {
      expansiony = false;
      signy = -1;
    }
  }
  else { // collapsey
    signy = -1;
    if ( (pos.y < (posinit.y - radius)) || (pos.y > (posinit.y + radius)) ) {
      expansiony = true;
      signy = 1;
    }
  }
  pos.y += (signy * rand()%3 * BIRD_DELTA);

  // z
  if (expansionz) {
    signz = 1;
    if (pos.z > (posinit.z + radius)) {
      expansionz = false;
      signz = -1;
    }
  }
  else { // collapsez
    signz = -1;
    if (pos.z < posinit.z) {
      expansionz = true;
      signz = 1;
    }
  }
  pos.z += (signz * rand()%3 * BIRD_DELTA);

  updatePosition();
}

/* Renders at each loop */
void Bird::render()
{
  glPushMatrix();
  glEnable(GL_CULL_FACE);
  glTranslatef(pos.x, pos.y, pos.z);
  //glRotatef(-90, 1, 0, 0);
  //glRotatef(-90, 0, 1, 0);
  //glRotatef(-90, 0, 0, 1);	// stand up but...
  glScalef(scale, scale, scale);

  wings->render();	// render wings

  glDisable(GL_CULL_FACE);
  glPopMatrix();
}

void Bird::quit()
{
}

void Bird::fly()
{
  enableBehavior(SPECIFIC_RENDER);
  enablePermanentMovement();
  wings->start();
  flying = true;
}

void Bird::pause()
{
  flying = false;
  disablePermanentMovement();
  wings->stop();
}

void Bird::reset()
{
  pause();
  pos = posinit;
}

void Bird::fly_cb(Bird *bird, void *d, time_t s, time_t u)
{
  bird->fly();
}

void Bird::pause_cb(Bird *bird, void *d, time_t s, time_t u)
{
  bird->pause();
}

void Bird::reset_cb(Bird *bird, void *d, time_t s, time_t u)
{
  bird->reset();
}

void Bird::funcs()
{
  setActionFunc(BIRD_TYPE, 0, _Action fly_cb, "flying");
  setActionFunc(BIRD_TYPE, 1, _Action pause_cb, "pause");
  setActionFunc(BIRD_TYPE, 2, _Action reset_cb, "reset");
}
