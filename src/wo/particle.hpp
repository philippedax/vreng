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
#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include "wobject.hpp"

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
class Particle: public WObject {

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
  bool mycolor;		///< prefered color

public:
  static const OClass oclass;   ///< class variable

  virtual const OClass* getOClass() {return &oclass;}

  Particle(char *l);	///< Constructor
  Particle();		///< Constructor needed for sub classes

  static void funcs();	///< init funclist

  static WObject * (creator)(char *l);
  /**< Creates from file line */

  virtual void changePermanent(float lasting);
  /**< Equations to move permanently */

  virtual void render();
  /**< Specific rendering */

  virtual void quit();
  /**< Quits properly */

protected:
  virtual void behavior();
  /**< Sets behavior */

  virtual void inits();
  /**< Creates particles */

  void generate(tParticle *p, float dt);
  /**< Creates a particle */

  void regenerate(float dt);
  /**< Regenerates some particles */

private:

  virtual void timestep(tParticle *p, float dt);
  /**< Motion in delta t */

  virtual void bounce(tParticle *p, float dt);
  /**< When bouncing the ground */

  virtual float timedelta();
  /**< Diff time in sec */

  virtual bool fequal(float a, float b);
  /**< Compare floats */

  virtual void parser(char *l);
  /**< Parses */

  virtual void defaults();
  /**< Sets default values */

};

#endif
