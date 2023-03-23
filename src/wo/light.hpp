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
#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "wobject.hpp"
#include <list>

#define LIGHT_TYPE	23
#define LIGHT_NAME	"Light"


/**
 * Light class
 */
class Light: public WObject {

private:
  GLfloat light_position[4];
  GLfloat light_ambient[4];
  bool islight;			///< flag light or not
  bool light_mode;		///< direction or position
  GLfloat fog;			///< fog density
  GLfloat color[3];		///< fog color

  enum {
    DIR,
    POS
  };

public:
  static const OClass oclass;	///< class variable

  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist

  Light(char *l);	///< Constructor
  Light();		///< Constructor

  static WObject * (creator)(char *l);

  void lighting();
  /**< Renders light. */

  static void lights();
  /**< Finds lights to render. */

  void quit();
  /**< Quits properly. */

private:
  void parser(char *l);
  /**< Parses file line. */

  void defaults();
  /**< Sets default values. */

};

#endif
