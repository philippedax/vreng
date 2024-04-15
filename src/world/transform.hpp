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
#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include "wobject.hpp"

#define TRANSFORM_TYPE	51
#define TRANSFORM_NAME	"Transform"


/**
 * operation list
 */
struct sOplist {
  uint8_t op;		///< transform operation
  GLfloat x;
  GLfloat y;
  GLfloat z;
  GLfloat a;		///< angle in degrees
  struct sOplist *next;	///< next operation
};

/**
 * Transform class
 * Push, Pop, Rot, Trans, Scale  operate a matrix transformation
 * - either creation of a local repere
 * - either a transformation inside the local repere
 */
class Transform: public WO {

 private:
  struct sOplist *opList;	///< list of operations
  struct sOplist *opl;		///< current pointer in the list of operations

  Pos trans;
  Pos rot;
  Pos scale;

  /* operations */
  enum {
    NONE,
    PUSH,
    POP,
    ROT,
    TRANS,
    SCALE
  }; // opers

 public:
  static const OClass oclass;	///< class variable

  const OClass* getOClass() {return &oclass;}

  Transform(char *l);	///< Constructor

  static void funcs();	///< init funclist

  static WO * (creator)(char *l);	///< Creates from fileline

  void render();	///< do the operation

  void quit();		///< quits

 private:
  void parser(char *l);
  void behaviors();
  void inits();

  void addList(uint8_t op, GLfloat x, GLfloat y, GLfloat z, GLfloat a);
  ///< add to list
};

#endif
