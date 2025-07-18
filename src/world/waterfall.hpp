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
// waterfall.hpp
//---------------------------------------------------------------------------
#ifndef WATERFALL_HPP
#define WATERFALL_HPP

#include "object.hpp"
#include "particle.hpp"

#define WATERFALL_TYPE	62
#define WATERFALL_NAME	"Waterfall"


/**
 * Waterfall class
 */
class Waterfall: public Particle {
 public:
  static const OClass oclass;   ///< class variable

  const OClass* getOClass() {return &oclass;}

  Waterfall(char *l);	///< Constructor
  ~Waterfall() {};	///< Destructor

  static void funcs();	///< init funclist

  static Object * (creator)(char *l);
  /**< Creates from file line */

 private:
  void parser(char *l);
  /**< Parses file line. */

  void defaults();
  /**< Sets default values. */

  void geometry();
  /**< Makes a pseudo solid. */

  static void pause(Waterfall *waterfall, void* d, time_t s, time_t u);
  /** Action on waterfall. */
};

#endif
