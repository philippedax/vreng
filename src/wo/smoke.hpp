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
#define NA		8	// number of angles of polygon


/**
 * ParticleSmoke class
 */
class ParticleSmoke {

#if 0 //dax0
private:
  static const float S;
  static const float PI;
  static const float A[NA];
  static const float COS[NA];
  static const float SIN[NA];

public:
  Vector3 loc;
  Vector3 vel;
  Vector3 acc;
  float life;
  GLint dlist;

  ParticleSmoke(Vector3 l);

  void update();
  void draw();
#endif //dax0
};

/**
 * Smoke class
 */
class Smoke: public WObject {

public:
  Vector3 loc;
  Vector3 vel;
  Vector3 acc;
  float life;
  GLint dlist;

  vector<Smoke> particles;
  Vector3 emitter;
  uint16_t np;		// notused

  static const OClass oclass;	///< class variable.
  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist.

  Smoke(char *l);	///< Constructor.
  Smoke(Vector3 l);	///< Constructor.
  ~Smoke() {};		///< Destructor.

  static WObject * (creator)(char *l);
  ///< Creates from fileline.

  void changePermanent(float dt);

  void render();

  void update();
  void draw();

private:
  static const float S;
  static const float PI;
  static const float A[NA];
  static const float COS[NA];
  static const float SIN[NA];

  void defaults();

  /**< Sets defaults values. */

  void parser(char *l);
  /**< Parses. */

  void behavior();

  void geometry();

  void inits();	// notused
  /**< Initializations. */

  void addParticle();   

  void animParticle();
  void moveParticle();
  void drawParticle();
  void dlistParticle();

  Vector3 random();
};

#endif
