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
#ifndef FIREWORK_HPP
#define FIREWORK_HPP

#include "wobject.hpp"
#include "particle.hpp"

#define FIREWORK_TYPE	82
#define FIREWORK_NAME	"Firework"


/**
 * Firework class
 */
class Firework: public Particle {

public:
  static const OClass oclass;   ///< class variable

  const OClass* getOClass() {return &oclass;}

  Firework(char *l);	///< Constructor

  void updateTime(time_t s, time_t u, float *lasting);
  /**< Updates times. */

  void changePosition(float lasting);

  void render();

  static void funcs();	///< init funclist

  static WO * (creator)(char *l);
  /**< Creates from file line */

private:
  static const float DEF_TTL;

  float ttl;		///< time to live
  float w, d, h;	///< dim

  void parser(char *l);
  /**< Parses file line. */

  void defaults();
  /**< Sets default values. */

  void geometry();
  /**< Makes a default solid. */

  void generate();
  /**< Generates particles. */

};

#endif
