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
#include "wobject.hpp"
#include "world.hpp"

/***    object includes    ***/
#include "user.hpp"		/* 01 */
#include "plane.hpp"		/* 02 */
#include "door.hpp"		/* 03 */
#include "thing.hpp"		/* 04 */
#include "ball.hpp"		/* 05 */
#include "step.hpp"		/* 06 */
#include "mirage.hpp"		/* 07 */
#include "gate.hpp"		/* 08 */
#include "wall.hpp"		/* 09 */
#include "ground.hpp"		/* 10 */
#include "web.hpp"		/* 11 */
#include "button.hpp"		/* 12 */
#include "cauldron.hpp"		/* 13 */
#include "projectile.hpp"	/* 14 */
#include "bullet.hpp"		/* 15 */
#include "dart.hpp"		/* 16 */
#include "host.hpp"		/* 17 */
#include "board.hpp"		/* 18 */
#include "doc.hpp"		/* 19 */
#include "walls.hpp"		/* 20 */
#include "entry.hpp"		/* 21 */
#include "bgcolor.hpp"		/* 22 */
#include "light.hpp"		/* 23 */
#include "music.hpp"  		/* 24 */
#include "movie.hpp"		/* 25 */
#include "clip.hpp"		/* 26 */
#include "clock.hpp"		/* 27 */
#include "download.hpp"		/* 28 */
#include "mirror.hpp"		/* 29 */
#include "stars.hpp"		/* 30 */
#include "particle.hpp"		/* 31 */
#include "water.hpp"		/* 32 */
#include "mech.hpp"		/* 33 */
#include "guy.hpp"		/* 34 */
#include "vnc.hpp"		/* 35 */
#include "vrelet.hpp"		/* 36 */
#include "vjc.hpp"		/* 37 */
#include "animator.hpp"		/* 38 */
#include "text.hpp"		/* 39 */
#include "android.hpp"		/* 40 */
#include "carrier.hpp" 		/* 41 */
#include "cart.hpp" 		/* 42 */
#include "book.hpp" 		/* 43 */
#include "sheet.hpp" 		/* 44 */
#include "icon.hpp" 		/* 45 */
#include "model.hpp" 		/* 46 */
#include "flag.hpp" 		/* 47 */
#include "guide.hpp" 		/* 48 */
#include "area.hpp" 		/* 49 */
#include "lift.hpp" 		/* 50 */
#include "transform.hpp"	/* 51 */
#include "aoi.hpp"		/* 52 */
#include "carousel.hpp"		/* 53 */
#include "wind.hpp"		/* 54 */
#include "widget.hpp"		/* 55 */
#include "thunder.hpp"		/* 56 */
#include "cloud.hpp"		/* 57 */
#include "stair.hpp"		/* 58 */
#include "escalator.hpp"	/* 59 */
#include "travelator.hpp"	/* 60 */
#include "spiral.hpp"		/* 61 */
#include "waterfall.hpp"	/* 62 */
#include "fountain.hpp"		/* 63 */
#include "rain.hpp"		/* 64 */
#include "snow.hpp"		/* 65 */
#include "bubble.hpp"		/* 66 */
#include "terrain.hpp"		/* 67 */
#include "river.hpp"		/* 68 */
#include "sun.hpp"		/* 69 */
#include "cloth.hpp"		/* 70 */
#include "halo.hpp"		/* 71 */
#include "hat.hpp"		/* 72 */
#include "dress.hpp"		/* 73 */
#include "wings.hpp"		/* 74 */
#include "fire.hpp"		/* 75 */
#include "smoke.hpp"		/* 76 */
#include "hairs.hpp"		/* 77 */
#include "slider.hpp"		/* 78 */
#include "bumper.hpp"		/* 79 */
#include "accessory.hpp"	/* 80 */
#include "head.hpp"		/* 81 */
#include "firework.hpp"		/* 82 */
#include "fractal.hpp"		/* 83 */
#include "human.hpp"		/* 84 */
#include "bird.hpp"		/* 85 */
#include "drone.hpp"		/* 86 */


const struct GeneralInitList generalInitList[] = {
/* 00 */	{NULL},
/* 01 */	{User::funcs},
/* 02 */	{Plane::funcs},
/* 03 */	{Door::funcs},
/* 04 */	{Thing::funcs},
/* 05 */	{Ball::funcs},
/* 06 */	{Step::funcs},
/* 07 */	{Mirage::funcs},
/* 08 */	{Gate::funcs},
/* 09 */	{Wall::funcs},
/* 10 */	{Ground::funcs},
/* 11 */	{Web::funcs},
/* 12 */	{Button::funcs},
/* 13 */	{Cauldron::funcs},
/* 14 */	{Projectile::funcs},
/* 15 */	{Bullet::funcs},
/* 16 */	{Dart::funcs},
/* 17 */	{Host::funcs},
/* 18 */	{Board::funcs},
/* 19 */	{Doc::funcs},
/* 20 */	{Walls::funcs},
/* 21 */	{Entry::funcs},
/* 22 */	{Bgcolor::funcs},
/* 23 */	{Light::funcs},
/* 24 */	{Music::funcs},
/* 25 */	{Movie::funcs},
/* 26 */	{Clip::funcs},
/* 27 */	{Clock::funcs},
/* 28 */	{Download::funcs},
/* 29 */	{Mirror::funcs},
/* 30 */	{Stars::funcs},
/* 31 */	{Particle::funcs},
/* 32 */	{Water::funcs},
/* 33 */	{Mech::funcs},
/* 34 */	{Guy::funcs},
/* 35 */	{Vnc::funcs},
/* 36 */	{Vrelet::funcs},
/* 37 */	{Vjc::funcs},
/* 38 */	{Animator::funcs},
/* 39 */	{Text::funcs},
/* 40 */	{Android::funcs},
/* 41 */	{Carrier::funcs},
/* 42 */	{Cart::funcs},
/* 43 */	{Book::funcs},
/* 44 */	{Sheet::funcs},
/* 45 */	{Icon::funcs},
/* 46 */	{Model::funcs},
/* 47 */	{Flag::funcs},
/* 48 */	{Guide::funcs},
/* 49 */	{Area::funcs},
/* 50 */	{Lift::funcs},
/* 51 */	{Transform::funcs},
/* 52 */	{Aoi::funcs},
/* 53 */	{Carousel::funcs},
/* 54 */	{Wind::funcs},
/* 55 */	{Widget::funcs},
/* 56 */	{Thunder::funcs},
/* 57 */	{Cloud::funcs},
/* 58 */	{Stair::funcs},
/* 59 */	{Escalator::funcs},
/* 60 */	{Travelator::funcs},
/* 61 */	{Spiral::funcs},
/* 62 */	{Waterfall::funcs},
/* 63 */	{Fountain::funcs},
/* 64 */	{Rain::funcs},
/* 65 */	{Snow::funcs},
/* 66 */	{Bubble::funcs},
/* 67 */	{Terrain::funcs},
/* 68 */	{River::funcs},
/* 69 */	{Sun::funcs},
/* 70 */	{Cloth::funcs},
/* 71 */	{Halo::funcs},
/* 72 */	{Hat::funcs},
/* 73 */	{Dress::funcs},
/* 74 */	{Wings::funcs},
/* 75 */	{Fire::funcs},
/* 76 */	{Smoke::funcs},
/* 77 */	{Hairs::funcs},
/* 78 */	{Slider::funcs},
/* 79 */	{Bumper::funcs},
/* 80 */	{Accessory::funcs},
/* 81 */	{Head::funcs},
/* 82 */	{Firework::funcs},
/* 83 */	{Fractal::funcs},
/* 84 */	{Human::funcs},
/* 85 */	{Bird::funcs},
/* 86 */	{Drone::funcs},

/* END */	{NULL}
};


// local
static struct PropertyFuncList putPropertyFuncList[OBJECTSNUMBER+1][PROPSNUMBER];
static struct PropertyFuncList getPropertyFuncList[OBJECTSNUMBER+1][PROPSNUMBER];
static struct GeneralActionList generalActionList[OBJECTSNUMBER+1][ACTIONSNUMBER];

float maxlastings[OBJECTSNUMBER+1];
uint8_t propertiesnumber[OBJECTSNUMBER+1];


/** initialization array of functions pointers */
void World::initGeneralFuncList()
{
  for (int i=0; i <= OBJECTSNUMBER; i++) {

    // action functions
    for (int j=0; j < ACTIONSNUMBER ; j++) {
      generalActionList[i][j].name[0] = '\0';
      generalActionList[i][j].method = NULL;
    }

    // property functions
    for (int j=0; j < PROPSNUMBER ; j++) {
      getPropertyFuncList[i][j].method = NULL;
      putPropertyFuncList[i][j].method = NULL;
    }

    // property values
    propertiesnumber[i] = 0;

    // maxlasting values
    maxlastings[i] = MAXFLOAT;

    // init functions
    if (generalInitList[i].initfunc) {
      trace(DBG_WO, "initfunc: i=%d f=%p", i, generalInitList[i].initfunc);
      generalInitList[i].initfunc();
    }
  }
}

/** Initializes table of action functions */
void setActionFunc(int type, uint8_t action, void (*action_method)(WObject *o, void *d, time_t s, time_t u), const char *action_name)
{
  strcpy(generalActionList[type][action].name, action_name);
  generalActionList[type][action].method = action_method;
}

bool isAction(int type, uint8_t action)
{
  return generalActionList[type][action].method;
}

void doAction(int type, uint8_t action, WObject *o, void *d, time_t s, time_t u)
{
  generalActionList[type][action].method(o, d, s, u);
}

bool isActionName(int type, uint8_t action)
{
  return generalActionList[type][action].name[0] != '\0';
}

int indexAction(int type, const char *name)
{
  for (int i=0; isAction(type, i) ; i++) {
    if (! strcasecmp(generalActionList[type][i].name, name))
      return i;
  }
  return -1;
}

void copyActionName(int type, uint8_t action, char *dest)
{
  strcpy(dest, generalActionList[type][action].name);
}

char * getActionName(int type, uint8_t action)
{
  return generalActionList[type][action].name;
}

void * getActionMethod(int type, uint8_t action)
{
  return (void *) generalActionList[type][action].method;
}

void setMaxLastings(int type, float maxlast)
{
  maxlastings[type] = maxlast;
}

float getMaxLastings(int type)
{
  return maxlastings[type];
}

void setPropertiesnumber(int type, uint8_t nbprop)
{
  propertiesnumber[type] = nbprop;
}

uint8_t getPropertiesnumber(int type)
{
  return propertiesnumber[type];
}

/** Initializes table of property functions */
void getPropertyFunc(int type, uint8_t prop, void (*getprop_method)(WObject *o, Payload *p))
{
  getPropertyFuncList[type][prop].method = getprop_method;
}

void putPropertyFunc(int type, uint8_t prop, void (*putprop_method)(WObject *o, Payload *p))
{
  putPropertyFuncList[type][prop].method = putprop_method;
}

bool isGetPropertyFunc(int type, uint8_t prop)
{
  return getPropertyFuncList[type][prop].method;
}

bool isPutPropertyFunc(int type, uint8_t prop)
{
  return putPropertyFuncList[type][prop].method;
}

void runGetPropertyFunc(int type, uint8_t prop, WObject *o, Payload *pp)
{
  getPropertyFuncList[type][prop].method(o, pp);
}

void runPutPropertyFunc(int type, uint8_t prop, WObject *o, Payload *pp)
{
  putPropertyFuncList[type][prop].method(o, pp);
}
