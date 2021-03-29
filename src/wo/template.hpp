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
#ifndef TEMPLATE_HPP
#define TEMPLATE_HPP

#define TEMPLATE_TYPE	55
#define TEMPLATE_NAME	"Template"

#include "wobject.hpp"

/**
 * Template class
 */
class Template : public WObject {

 private:

 public:
  static const OClass oclass;   ///< class variable

  virtual const OClass* getOClass() {return &oclass;}

  static void funcs();		///< init funclist

  Template(char *l);
  /**< Constructor */

  static WObject * (creator)(char *l);
  /**< Creates from fileline */

  virtual void changePermanent(float lasting);
  /**< Called each frame - check if ths texture has to be updated */

  virtual void render();
  /**< Render the object */

  virtual void quit();
  /**< Called when the object is destroy */

 private:
  virtual void parser(char *l);
  /**< Parses */

  virtual void defaults();
  /**< Sets defaults values */

  virtual void behavior();
  /**< Sets behavior */

  virtual void inits();
  /**< Do specific inits */

  // GUI callbacks
  static void action1(Template *o, void *d, time_t s, time_t u);
  static void action2(Template *o, void *d, time_t s, time_t u);

};

#endif
