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
#include "drone.hpp"
#include "wings.hpp"
#include "user.hpp"	// setView
#include "render.hpp"	// cameraPosition


const OClass Drone::oclass(DRONE_TYPE, "Drone", Drone::creator);

const float Drone::DRONE_SCALE = .4;
const float Drone::DRONE_ZONE = 20;	// flying zone 20x20
const float Drone::DRONE_DELTA = .05;	// elementary movement 5cm


/* Creation from a file */
WObject * Drone::creator(char *l)
{
  return new Drone(l);
}

void Drone::defaults()
{
  model = Wings::HELICOPTER;
  wings = NULL;
  zone = DRONE_ZONE;
  scale = DRONE_SCALE;
  flying = false;
  filming = false;
  driven = false;
}

/* Parser */
void Drone::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if      (! stringcmp(l, "zone"))   l = parseFloat(l, &zone, "zone");
    else if (! stringcmp(l, "scale"))  l = parseFloat(l, &scale, "scale");
    else if (! stringcmp(l, "flying")) l = parseBool(l, &flying, "flying");
  }
  end_while_parse(l);
}

/* Behavior */
void Drone::behavior()
{
  enableBehavior(NO_ELEMENTARY_MOVE);
  enableBehavior(COLLIDE_NEVER);
  enableBehavior(SPECIFIC_RENDER);
}

/* Specific inits */
void Drone::inits()
{
  vieworig = ::g.render.getViewMode();
  posorig = pos;
  wings = new Wings(model, scale);

  initMobileObject(0);

  if (flying)
    fly();
}

/* Constructor */
Drone::Drone(char *l)
{
  parser(l);
  behavior();
  inits();
}

/* Computes postion at each loop */
void Drone::changePermanent(float lasting)
{
  struct timeval tv;

  gettimeofday(&tv, NULL);
  srand((time_t) tv.tv_sec);

  static bool expandx = true;
  static bool expandy = true;
  static int signx = rand()%2 -1; //orig -1;
  static int signy = rand()%2 -1; //orig 1;
  static int signz = 1;

  if (! driven) {
    // x
    signx = signx ? signx : -1;
    if (expandx) {
      if ( (pos.x < -zone) || (pos.x > zone) ) {
        expandx = false;
        signx = (signx<0) ? 1 : -1;
      }
    }
    else { // collapsex
      if ( (pos.x < -zone) || (pos.x > zone) ) {
        expandx = true;
        signx = (signx<0) ? 1 : -1;
      }
    }
    pos.x += (signx * rand()%3 * DRONE_DELTA);
  
    // y
    signy = signy ? signy : -1;
    if (expandy) {
      if ( (pos.y < -zone) || (pos.y > zone) ) {
        expandy = false;
        signy = (signy<0) ? 1 : -1;
      }
    }
    else { // collapsey
      if ( (pos.y < -zone) || (pos.y > zone) ) {
        expandy = true;
        signy = (signy<0) ? 1 : -1;
      }
    }
    pos.y += (signy * rand()%3 * DRONE_DELTA);
    //echo("pos: %.1f %.1f", pos.x, pos.y);

    // z
    if (pos.z > (zone / 2)) {
      signz = 0;	// asymptote
    }
    pos.z += (signz * rand()%3 * DRONE_DELTA);

    updatePosition();

    if (filming) {
      // user follows the drone
      localuser->pos.x = pos.x;
      localuser->pos.y = pos.y;
      localuser->pos.z = pos.z;
      localuser->disableGravity();
    }
  }
  else {	// drone is drived by user
    pos.x = localuser->pos.x;
    pos.y = localuser->pos.y;
    pos.z = localuser->pos.z;
  }
}

/* Renders at each loop */
void Drone::render()
{
  if (filming) {
    //echo("drone: %.1f %.1f %.1f", pos.x,pos.y,pos.z);
#if 1 //dax
    glPushMatrix();
    ::g.render.setCameraScissor(pos.x, pos.y, pos.z, 90);
    ::g.render.showView();
    glPopMatrix();
#else
    ::g.render.cameraPosition(this);
#endif
    return;
  }
  glPushMatrix();
  glEnable(GL_CULL_FACE);
  glTranslatef(pos.x, pos.y, pos.z);
  glRotatef(-90, 1, 0, 0);
  glRotatef(-90, 0, 0, 1);
  glScalef(scale, scale, scale);

  wings->render();	// render wings

  glDisable(GL_CULL_FACE);
  glPopMatrix();
}

void Drone::quit()
{
}

void Drone::fly()
{
  flying = true;
  enableBehavior(SPECIFIC_RENDER);

  enablePermanentMovement();
  wings->start();
}

void Drone::pause()
{
  flying = false;
  disablePermanentMovement();
  wings->stop();
}

/** toggle follow */
void Drone::follow()
{
  if (flying) {
    if (filming) {
      filming = false;
      ::g.render.switchViewObj();
      localuser->setView(vieworig);
    }
    else {
      filming = true;
      ::g.render.switchViewObj();
      localuser->setView(Render::VIEW_VERTICAL_FROM_OBJECT);
    }
  }
}

/** toggle drive */
void Drone::drive()
{
  if (driven) {
    driven = false;	// drone takes control
    localuser->enableGravity();
  }
  else {
    driven = true;	// user takes control of drone
  }
}

void Drone::reset()
{
  pause();
  pos = posorig;
  if (filming) {
    filming = false;
    driven = false;
    localuser->setView(vieworig);
    localuser->enableGravity();
  }
}

void Drone::fly_cb(Drone *drone, void *d, time_t s, time_t u)
{
  drone->fly();
}

void Drone::follow_cb(Drone *drone, void *d, time_t s, time_t u)
{
  drone->follow();
}

void Drone::drive_cb(Drone *drone, void *d, time_t s, time_t u)
{
  drone->drive();
}

void Drone::pause_cb(Drone *drone, void *d, time_t s, time_t u)
{
  drone->pause();
}

void Drone::reset_cb(Drone *drone, void *d, time_t s, time_t u)
{
  drone->reset();
}

void Drone::funcs()
{
  setActionFunc(DRONE_TYPE, 0, _Action fly_cb, "flying");
  setActionFunc(DRONE_TYPE, 1, _Action follow_cb, "follow");
  setActionFunc(DRONE_TYPE, 2, _Action drive_cb, "drive");
  setActionFunc(DRONE_TYPE, 3, _Action pause_cb, "pause");
  setActionFunc(DRONE_TYPE, 4, _Action reset_cb, "reset");
}
