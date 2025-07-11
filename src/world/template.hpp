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
// template.hpp
//---------------------------------------------------------------------------
#ifndef TEMPLATE_HPP
#define TEMPLATE_HPP

#define TEMPLATE_TYPE	0
#define TEMPLATE_NAME	"Template"

#include "object.hpp"


/**
 * Template class
 */
class Template : public Object {
 private:

 public:
  static const OClass oclass;   ///< class variable

  const OClass* getOClass() {return &oclass;}

  static void funcs();		///< init funclist

  Template(char *l);	///< Constructor
  ~Template() {};	///< Destructor

  static Object * (creator)(char *l);
  /**< Creates from fileline */

  void permanent(float lasting);
  /**< Called each frame - check if ths texture has to be updated */

  void render();
  /**< Render the object */

 private:
  void parser(char *l);
  /**< Parses */

  void defaults();
  /**< Sets defaults values */

  void behaviors();
  /**< Sets behaviors */

  void inits();
  /**< Do specific inits */

  // GUI callbacks
  static void action1(Template *o, void *d, time_t s, time_t u);
  static void action2(Template *o, void *d, time_t s, time_t u);
};

#endif
