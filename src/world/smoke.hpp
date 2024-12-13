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

using namespace std;

#define SMOKENB		100	// number max of particles
#define NA		8	// number of angles of polygon


/**
 * Smoke class
 */
class Smoke: public WO {
 private:
  static const float SZ;
  static const float A[NA];

  uint16_t npmax;		///< number max of particles
  uint16_t np;			///< number of particles
  Vector3 loc;			///< location.
  Vector3 vel;			///< velocity.
  Vector3 acc;			///< acceleration.
  float life;			///< time to live.
  GLint dlist;			///< displaylist.
  vector<Smoke> psmokeList;	///< psmokeList.

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

  void animParticles();
  /**< Animates particles. */

  void update();
  void draw();

 public:
  static const OClass oclass;	///< class variable.
  const OClass* getOClass() {return &oclass;}

  static void funcs();		///< init funclist.

  Smoke(char *l);		///< Constructor.
  Smoke(Vector3 l);		///< Constructor.
  ~Smoke() {};			///< Destructor.

  static WO * (creator)(char *l);
  /**< Creates from fileline. */

  void changePermanent(float dt);

  void render();
};

#endif
