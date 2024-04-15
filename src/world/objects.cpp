//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2012 Philippe Dax
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
// objects.cpp
//
// Objects handling
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
#include "humanoid.hpp"		/* 40 */
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
#include "travolator.hpp"	/* 60 */
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
#include "face.hpp"		/* 81 */
#include "firework.hpp"		/* 82 */
#include "fractal.hpp"		/* 83 */
#include "human.hpp"		/* 84 */
#include "bird.hpp"		/* 85 */
#include "drone.hpp"		/* 86 */
#include "fog.hpp"		/* 87 */
#include "spot.hpp"		/* 88 */


const struct InitFunc initFuncs[] = {
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
/* 40 */	{Humanoid::funcs},
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
/* 60 */	{Travolator::funcs},
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
/* 81 */	{Face::funcs},
/* 82 */	{Firework::funcs},
/* 83 */	{Fractal::funcs},
/* 84 */	{Human::funcs},
/* 85 */	{Bird::funcs},
/* 86 */	{Drone::funcs},
/* 87 */	{Fog::funcs},
/* 88 */	{Spot::funcs},

/* END */	{NULL}
};


// local
static struct ProperFunc putPropArray[OBJECTSNUMBER][PROPSNUMBER];
static struct ProperFunc getPropArray[OBJECTSNUMBER][PROPSNUMBER];
static struct ActionFunc actionArray[OBJECTSNUMBER][ACTIONSNUMBER];

float maxlastings[OBJECTSNUMBER];
uint8_t propertiesnumber[OBJECTSNUMBER];


/** initialization array of functions pointers */
void World::initFunc()
{
  for (uint8_t i=0; i < OBJECTSNUMBER; i++) {

    // action functions
    for (uint8_t j=0; j < ACTIONSNUMBER ; j++) {
      actionArray[i][j].name[0] = '\0';
      actionArray[i][j].method = NULL;
    }

    // property functions
    for (uint8_t j=0; j < PROPSNUMBER ; j++) {
      getPropArray[i][j].method = NULL;
      putPropArray[i][j].method = NULL;
    }

    // property values
    propertiesnumber[i] = 0;

    // maxlasting values
    maxlastings[i] = MAXFLOAT;

    // init functions
    if (initFuncs[i].initfunc) {
      initFuncs[i].initfunc();
    }
  }
}

/** Initializes table of action functions */
void setActionFunc(uint8_t type, uint8_t action, void (*action_method)(WO *o, void *d, time_t s, time_t u), const char *action_name)
{
  strcpy(actionArray[type][action].name, action_name);
  actionArray[type][action].method = action_method;
}

/** Is an action ? */
bool isAction(uint8_t type, uint8_t action)
{
  return actionArray[type][action].method;
}

/** Do an action */
void doAction(uint8_t type, uint8_t action, WO *o, void *d, time_t s, time_t u)
{
  actionArray[type][action].method(o, d, s, u);
}

/** Is action named ? */
bool isActionName(uint8_t type, uint8_t action)
{
  return actionArray[type][action].name[0] != '\0';
}

/** Returns index of the action */
int indexAction(uint8_t type, const char *name)
{
  for (int i=0; isAction(type, i) ; i++) {
    if (! strcasecmp(actionArray[type][i].name, name)) {
      return i;
    }
  }
  return -1;
}

/** Copies action's name */
void copyActionName(uint8_t type, uint8_t action, char *dest)
{
  strcpy(dest, actionArray[type][action].name);
}

/** Gets action's name */
char * getActionName(uint8_t type, uint8_t action)
{
  return actionArray[type][action].name;
}

/** Gets action's method */
void * getActionMethod(uint8_t type, uint8_t action)
{
  return (void *) actionArray[type][action].method;
}

/** Sets maxlast */
void setMaxLastings(uint8_t type, float maxlast)
{
  maxlastings[type] = maxlast;
}

/** Gets maxlast */
float getMaxLastings(uint8_t type)
{
  return maxlastings[type];
}

/** Sets property number */
void setPropertiesnumber(uint8_t type, uint8_t nbprop)
{
  propertiesnumber[type] = nbprop;
}

/** Gets property number */
uint8_t getPropertiesnumber(uint8_t type)
{
  return propertiesnumber[type];
}

/** Initializes table of get property functions */
void getPropFunc(uint8_t type, uint8_t prop, void (*get_method)(WO *o, Payload *p))
{
  getPropArray[type][prop].method = get_method;
}

/** Initializes table of put property functions */
void putPropFunc(uint8_t type, uint8_t prop, void (*put_method)(WO *o, Payload *p))
{
  putPropArray[type][prop].method = put_method;
}

/** Is get property ? */
bool isGetPropFunc(uint8_t type, uint8_t prop)
{
  return getPropArray[type][prop].method;
}

/** Is put property ? */
bool isPutPropFunc(uint8_t type, uint8_t prop)
{
  return putPropArray[type][prop].method;
}

/** Exec get property */
void runGetPropFunc(uint8_t type, uint8_t prop, WO *o, Payload *pp)
{
  getPropArray[type][prop].method(o, pp);
}

/** Exec put property */
void runPutPropFunc(uint8_t type, uint8_t prop, WO *o, Payload *pp)
{
  putPropArray[type][prop].method(o, pp);
}
