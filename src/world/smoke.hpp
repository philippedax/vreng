//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2024 Philippe Dax
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
// smoke.hpp
//---------------------------------------------------------------------------
#ifndef SMOKE_HPP
#define SMOKE_HPP

#define SMOKE_TYPE 	76
#define SMOKE_NAME 	"Smoke"

#include "vector3.hpp"
#include "wobject.hpp"
#include <vector>

#define SMOKE_NB	100	// number max of particles
#define SMOKE_SP	.0003	// speed of a smoke particle
#define SMOKE_SZ	.002	// side's size of a smoke particle
#define SMOKE_NA	8	// number of angles of polygon


/**
 * PSmoke class
 */
class PSmoke {
private:
  float ang[SMOKE_NA];	///< angles of octagon.
  float siz;		///< size of side.
  float speed;		///< speed of particle.

public:
  Vector3 loc;		///< location.
  Vector3 vel;		///< velocity.
  Vector3 acc;		///< acceleration.
  float life;		///< time to live.

  PSmoke(Vector3 l, float speed, float size);	///< Constructor.
  ~PSmoke() {};		///< Destructor.

  void update();	///< update a particle.
  void draw();		///< draw a particle.
};


/**
 * Smoke class
 */
class Smoke: public WO {
public:
  static const OClass oclass;		///< class variable.
  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist.

  Smoke(char *l);	///< Constructor.
  ~Smoke() {};		///< Destructor.

  static WO * (creator)(char *l);
  ///< Creates from fileline.

  void changePermanent(float dt);

  void render();

private:
  uint16_t npmax;	///< number max of particles.
  uint16_t np;		///< number of particles.
  float speed;		///< speed of particle.
  float size;		///< size of particle side.

  std::vector<PSmoke *> smokeList;	///< list of smoke particles.

  void defaults();
  /**< Sets defaults values. */

  void parser(char *l);
  /**< Parses. */

  void behaviors();
  /**< Sets behaviors. */

  void geometry();
  /**< Sets geometry. */

  void inits();
  /**< Initializations. */
};

#endif
