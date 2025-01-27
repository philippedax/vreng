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
// sun.hpp
//---------------------------------------------------------------------------
#ifndef SUN_HPP
#define SUN_HPP

#include "object.hpp"

#define SUN_TYPE	69
#define SUN_NAME	"Sun"


/**
 * Sun class
 */
class Sun: public Object {
 protected:
  static const GLfloat DEF_RADIUS;
  static const GLfloat light_pos[4];
  static const GLfloat light_amb[4];

  GLfloat radius;		///< sun radius
  GLfloat scale;		///< sun scale
  GLfloat light_dif[4];
  GLfloat light_spe[4];
  GLfloat rot;			///< angle sun position
  GLfloat ox, oy, oz;		///< origin position
  GLint dlist;

  class Light *light;		///< light pointer
  class Flare *flares;		///< flares

 public:
  static const OClass oclass;	///< class variable

  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist

  Sun(char *l);	///< Constructor

  static Object * (creator)(char *l);
  /**< Creates from file line. */

  void changePermanent(float lasting);
  /**< Permanent movement. */

  void lighting();
  /**< Renders light. */

  void render();
  /**< Renders. */

  void quit();
  /**< Quits. */

 private:
  void parser(char *l);
  /**< Parses file line.*/

  void defaults();
  /**< Sets defaults. */

  void behaviors();
  /**< Sets behaviors. */

  void inits();
  /**< Inits specific */

  void geometry();
  /**< Builds solid geometry. */

  void draw();
  /**< Draws. */
};

#endif
