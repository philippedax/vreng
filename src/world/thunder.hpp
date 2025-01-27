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
// thunder.hpp
//---------------------------------------------------------------------------
#ifndef THUNDER_HPP
#define THUNDER_HPP

#include "object.hpp"

#define THUNDER_TYPE	56
#define THUNDER_NAME	"Thunder"


typedef struct {
  GLfloat x, y, z;
} s_point;


/**
 * Thunder class
 */
class Thunder: public Object {
 private:
  static const uint8_t NUMBER;		///< number of simultaneous thunders
  static const uint8_t PERIOD;		///< default period

  uint8_t number;	///< number of segments
  uint8_t period;	///< period
  s_point inc;		///< segment increment
  GLint dlist;		///< display-list
  GLfloat color[4];	///< color

 public:
  static const OClass oclass;	///< class variable

  const OClass* getOClass() {return &oclass;}

  static void funcs();		///< init funclist

  Thunder(char *l);		///< Constructor

  static Object * (creator)(char *l);
  /**< Creation from a file */

  void changePermanent(float lasting);
  /**< Equation to move permanently */

  void render();
  /**< Specific rendering */

  void quit() {};
  /**< Quits properly */

 private:
  void parser(char *l);
  /**< Parses. */

  void defaults();
  /**< Sets default values. */

  void behaviors();
  /**< Sets behaviors. */

  void geometry();
  /**< Sets solid geometry. */

  void inits();
  /**< Sets specific inits. */

  void draw();
  /**< Draws a thunder. */
};

#endif
