//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
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
// bird.cpp
//
// Bird handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "bird.hpp"
#include "wings.hpp"


const OClass Bird::oclass(BIRD_TYPE, "Bird", Bird::creator);

const float Bird::BIRD_SCALE = 0.3;
const float Bird::BIRD_ZONE = 3;	// flying zone
const float Bird::BIRD_DELTA = .01;	// elem motion


/** Creation from a file */
Object * Bird::creator(char *l)
{
  return new Bird(l);
}

void Bird::defaults()
{
  model = Wings::BIRD;
  wings = NULL;
  zone = BIRD_ZONE;
  scale = BIRD_SCALE;
}

/** Parser */
void Bird::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if (! stringcmp(l, "model=")) {
      l = parseString(l, modelname, "model");
      if      (! stringcmp(modelname, "bird"))      model = Wings::BIRD;
      else if (! stringcmp(modelname, "butterfly")) model = Wings::BUTTERFLY;
    }
    else if (! stringcmp(l, "zone"))   l = parseFloat(l, &zone, "zone");
    else if (! stringcmp(l, "scale"))  l = parseFloat(l, &scale, "scale");
    else if (! stringcmp(l, "flying")) l = parseBool(l, &flying, "flying");
    else {
      parse()->errorAtLine(l);
      l = parse()->nextToken();
      break;
    }
  }
  end_while_parse(l);
}

/** Behavior */
void Bird::behaviors()
{
  enableBehavior(COLLIDE_NEVER);
  enableBehavior(SPECIFIC_RENDER);
}

/** Specific inits */
void Bird::inits()
{
  wings = new Wings(model, scale, 1);
  posorig = pos;

  mobileObject(0);

  if (flying) {
    fly();
  }
}

/** Constructor */
Bird::Bird(char *l)
{
  flying = false;
  parser(l);
  behaviors();
  inits();
}

/** Computes position at each loop */
void Bird::permanent(float lasting)
{
  srand(time(NULL));

  static bool expandx = true;
  static bool expandy = true;
  static bool expandz = true;
  static int signx = rand()%2 -1; //orig -1;
  static int signy = rand()%2 -1; //orig 1;
  static int signz = 1;

  // x
  if (expandx) {
    signx = signx ? signx : -1;
    if ( (pos.x < (posorig.x - zone)) || (pos.x > (posorig.x + zone)) ) {
      expandx = false;
      signx = (signx<0) ? 1 : -1;
    }
  }
  else { // collapsex
    signx = 1;
    if ( (pos.x < (posorig.x - zone)) || (pos.x > (posorig.x + zone)) ) {
      expandx = true;
      signx = (signx<0) ? 1 : -1;
    }
  }
  pos.x += (signx /* * rand()%3 */ * BIRD_DELTA);

  // y
  if (expandy) {
    signy = signy ? signy : -1;
    if ( (pos.y < (posorig.y - zone)) || (pos.y > (posorig.y + zone)) ) {
      expandy = false;
      signy = (signy<0) ? 1 : -1;
    }
  }
  else { // collapsey
    signy = -1;
    if ( (pos.y < (posorig.y - zone)) || (pos.y > (posorig.y + zone)) ) {
      expandy = true;
      signy = (signy<0) ? 1 : -1;
    }
  }
  pos.y += (signy /* * rand()%3 */ * BIRD_DELTA);

  // z
  if (expandz) {
    signz = signz ? signz : -1;
    if (pos.z > (posorig.z + zone)) {
      expandz = false;
      signz = (signz<0) ? 1 : -1;
    }
  }
  else { // collapsez
    signz = -1;
    if (pos.z < posorig.z) {
      expandz = true;
      signz = (signz<0) ? 1 : -1;
    }
  }
  pos.z += (signz /* * rand()%3 */ * BIRD_DELTA);

  updatePos();
}

/** Renders at each loop */
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
  permanentMovement();
  wings->start();
  flying = true;
}

void Bird::pause()
{
  flying = false;
  stopPermanent();
  wings->stop();
}

void Bird::reset()
{
  pause();
  pos = posorig;
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
