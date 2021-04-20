//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2011 Philippe Dax
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
#ifndef SMOKE_HPP
#define SMOKE_HPP

#define SMOKE_TYPE 	76
#define SMOKE_NAME 	"Smoke"

#include "vector3.hpp"
#include "wobject.hpp"
#include <vector>

#define SMOKENB		100


/**
 * ParticleSmoke class
 */
class ParticleSmoke {

private:
  static const float S;
  static const float PI;
  static const float A[10];
  static const float COS[10];
  static const float SIN[10];

public:
  Vector3 loc;
  Vector3 vel;
  Vector3 acc;
  float life;

  ParticleSmoke(Vector3 l);

  void update();
  void draw();
};

/**
 * Smoke class
 */
class Smoke: public WObject {

public:
  static const OClass oclass;	///< class variable.
  virtual const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist.

  Smoke(char *l);	///< Constructor.
  ~Smoke() {};		///< Destructor.

  static WObject * (creator)(char *l);
  ///< Creates from fileline.

  virtual void changePermanent(float dt);

  virtual void render();

private:
  uint16_t np;
  float speed;
  vector<ParticleSmoke> particles;
  Vector3 emitter;

  Smoke(Vector3 l);	///< Constructor.

  virtual void defaults();
  /**< Sets defaults values. */

  virtual void parser(char *l);
  /**< Parses. */

  virtual void behavior();

  virtual void makeSolid();

  virtual void inits();
  /**< Initializations. */

  void addParticle();   

  void animParticle();
  void updParticle();
  void drawParticle();

  Vector3 randomv();
};

#endif
