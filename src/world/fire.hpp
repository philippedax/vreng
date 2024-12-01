//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2011 Philippe Dax
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
// fire.hpp
//---------------------------------------------------------------------------
#ifndef FIRE_HPP
#define FIRE_HPP

#define FIRE_TYPE 	75
#define FIRE_NAME 	"Fire"

#include "wobject.hpp"
#include "vector3.hpp"


#define FIREMAX		2000 // 1024
#define FIRENB		1000
#define FIRELIFE	1.5  // 1.5 sec
#define FIREDELTA	.005 // interval
#define FIRESIZE	.2   // 20cm
#define FIREALPHA	.65  // .35
#define FIRECYL		.1   // 10 cm

#define projx(p)	(2*(p).x/(p).y)
#define projy(p)	(2*(p).z/(p).y)
#define projz(ez,p)	(ez)=-((p).y*(float)(1./20.)-1.)


/**
 * structs
 */
struct sParticle {
  Vector3 p; // position
  Vector3 v; // dp/dt
  Vector3 s; // size
  float a;     // alpha
  float t;     // time to death
  float ex,ey,ez; // screen pos
  float dx,dy; // screen size
};

struct Vertex {
  float u,v;
  float ex,ey,ez;
};

struct Quad {
  Vertex v[4];
};

struct sCyl {
  Vector3 p;
  Vector3 d;
  float r;
}; // wood cylinders


/**
 * Fire class
 */
class Fire: public WO {
 public:
  static const OClass oclass;	///< class variable.
  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist.

  Fire(char *l);	///< Constructor.
  Fire() {}		///< Constructor needed by Smoke.

  static WO * (creator)(char *l);
  ///< Creates from fileline.

  void changePermanent(float lasting);
  /**< Animates Fire. */

  void render();
  /**< Renders the fire. */

  void quit();
  /**< Quits properly. */

 protected:
  uint16_t np;		///< number of particles
  float speed;
  float nowtime;	///< fire time
  float lasttime;
  float width;
  float depth;
  float height;
  bool anim;
  uint8_t sound;

  void geometry();
  /**< Makes a solid if no one. */

 private:
  void defaults();
  /**< Sets defaults values. */

  void parser(char *l);
  /**< Parses file line. */

  void behaviors();
  /**< Sets behaviors. */

  void inits();
  /**< Initializations. */

  void draw();
  /**< Draws fire particules. */

  void draw(float ex,float ey,float dx,float dy,float a);
  /**< Draws one particule. */

  static void sound_continue(int sig);
  void sound_continue();

  // Callbacks
  static void sound_cb(Fire *fire, void *d, time_t s, time_t u);
  static void stop_cb(Fire *fire, void *d, time_t s, time_t u);
};

#endif
