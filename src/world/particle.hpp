//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2009 Philippe Dax
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
// particle.hpp
//---------------------------------------------------------------------------
#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include "object.hpp"

#define PARTICLE_TYPE	31
#define PARTICLE_NAME	"Particle"


typedef struct {
  float pos[3];	///< current position
  float prev[3];///< previous position
  float vel[3];	///< velocity (magnitude & direction)
  float rgb[3];	///< individual color
  float damp;	///< % of energy lost on collision
  bool  alive;	///< is this particle alive?
} tParticle;


/**
 * Particle class
 */
class Particle: public Object {
 protected:
  static const uint16_t DEF_NUM;
  static const float DEF_FLOW;
  static const float DEF_SPEED;
  static const uint8_t DEF_PTSIZE;
  static const float DEF_GRAVITY;

  /* systems */
  enum {
    WATERFALL,
    FOUNTAIN,
    FIREWORK,
    RAIN,
    SNOW,
    NONE
  };

  tParticle *particles;	///< particles ptr
  uint16_t number;	///< number of particles
  uint16_t living;	///< paticles in live
  uint8_t system;	///< system mode
  uint8_t pt_size;	///< size of point
  float color[3];	///< color of particle
  float flow;		///< flow
  float speed;		///< speed
  float ground;		///< ground position
  bool points;		///< flag points or not
  bool onecolor;	///< mono color

 public:
  static const OClass oclass;   ///< class variable

  const OClass* getOClass() {return &oclass;}

  Particle(char *l);	///< Constructor
  Particle();		///< Constructor needed for sub classes

  static void funcs();	///< init funclist

  static Object * (creator)(char *l);
  /**< Creates from file line */

  void permanent(float lasting);
  /**< Equations to move permanently */

  void render();
  /**< Specific rendering */

  void quit();
  /**< Quits properly */

 protected:
  void behaviors();
  /**< Sets behaviors */

  void inits();
  /**< Creates particles */

  void generate(tParticle *p, float dt);
  /**< Creates a particle */

  void regenerate(float dt);
  /**< Regenerates some particles */

 private:
  void timestep(tParticle *p, float dt);
  /**< Motion in delta t */

  void bounce(tParticle *p, float dt);
  /**< When bouncing the ground */

  float timedelta();
  /**< Diff time in sec */

  bool fequal(float a, float b);
  /**< Compare floats */

  void parser(char *l);
  /**< Parses */

  void defaults();
  /**< Sets default values */

  void geometry();
  /**< Sets solid geometry */
};

#endif
