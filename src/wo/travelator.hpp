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
#ifndef TRAVELATOR_HPP
#define TRAVELATOR_HPP

#include "wobject.hpp"
#include "step.hpp"

#define TRAVELATOR_TYPE	60
#define TRAVELATOR_NAME	"Travelator"

/**
 * Travelator class
 */
class Travelator: public Step {

private:
  bool on;	///< running or pause

public:
  static const OClass oclass;	///< class variable

  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist

  Travelator(char *l);	///< Constructor

  static WObject * (creator)(char *l);
  /**< Create from fileline */

  void quit();
  /**< Quits */

private:
  void parser(char *l);
  /**< Parses */

  void defaults();
  /**< Default values */

  void behaviors();
  /**< Sets behaviors */

  void build();
  /**< Builds travelator */
};

#endif
