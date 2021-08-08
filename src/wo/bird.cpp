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


/* Creation from a file */
WObject * Bird::creator(char *l)
{
  return new Bird(l);
}

void Bird::defaults()
{
  flying = false;
  model = Wings::BUTTERFLY;
  wings = NULL;
}

/* Parser */
void Bird::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if (!stringcmp(l, "model=")) {
      l = parse()->parseString(l, modelname, "model");
      if (! stringcmp(modelname, "butterfly"))  model = Wings::BUTTERFLY;
    }
  }
  end_while_parse(l);
}

/* Behavior */
void Bird::behavior()
{
  enableBehavior(NO_ELEMENTARY_MOVE);
  enableBehavior(COLLIDE_NEVER);
  //enableBehavior(SPECIFIC_RENDER);

  initMobileObject(0);
  updatePosition();
}

/* Specific inits */
void Bird::inits()
{
  posinit = pos;
  wings = new Wings();
}

/* Constructor */
Bird::Bird(char *l)
{
  parser(l);
  behavior();
  inits();
}

/* Computes something at each loop */
void Bird::changePermanent(float lasting)
{
  int sign;
  srand((uint32_t) time(NULL));
  sign = rand()%2 - 1;
  pos.x -= sign * .001;
  srand((uint32_t) time(NULL));
  sign = rand()%2 - 1;
  pos.y -= sign * .001;
  pos.z += .01;
}

/* Renders at each loop */
void Bird::render()
{
  if (! flying) return;

  updatePosition();

  // push
  glPushMatrix();
  glEnable(GL_CULL_FACE);
  glTranslatef(pos.x, pos.y, pos.z);

  // render
  wings->render();

  // pop
  glDisable(GL_CULL_FACE);
  glDisable(GL_TEXTURE_2D);
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
  setActionFunc(BIRD_TYPE, 0, WO_ACTION fly_cb, "fly");
  setActionFunc(BIRD_TYPE, 1, WO_ACTION pause_cb, "pause");
  setActionFunc(BIRD_TYPE, 2, WO_ACTION reset_cb, "reset");
}
