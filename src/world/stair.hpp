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
// stair.hpp
//---------------------------------------------------------------------------
#ifndef STAIR_HPP
#define STAIR_HPP

#include "object.hpp"
#include "step.hpp"

#define STAIR_TYPE	58
#define STAIR_NAME	"Stair"


/**
 * Stair class
 */
class Stair: public Step {
 public:
  static const OClass oclass;	///< class variable

  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist

  Stair(char *l);	///< Constructor
  ~Stair();		///< Destructor

  static Object * (creator)(char *l);
  /**< Create from fileline */

 private:
  void parser(char *l);
  /**< Parses */

  void defaults();
  /**< Default values */

  void inits();
  /**< Sets initializations. */

  void build();
  /**< Builds stair */
};

#endif
