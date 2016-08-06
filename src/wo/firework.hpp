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

  virtual const OClass* getOClass() {return &oclass;}

  Firework(char *l);	///< Constructor

  virtual void updateTime(time_t s, time_t u, float *lasting);
  /**< Updates times. */

  virtual void changePosition(float lasting);

  virtual void render();

  static void funcs();	///< init funclist

  static WObject * (creator)(char *l);
  /**< Creates from file line */

private:
  static const float DEF_TTL;

  float ttl;		///< time to live
  float w, d, h;	///< dim

  virtual void parser(char *l);
  /**< Parses file line. */

  virtual void defaults();
  /**< Sets default values. */

  virtual void makeSolid();
  /**< Makes a default solid. */

  void generate();
  /**< Generates particles. */

};

#endif
