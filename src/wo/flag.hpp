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
#ifndef FLAG_HPP
#define FLAG_HPP

#include "wobject.hpp"

#define FLAG_TYPE 	47
#define FLAG_NAME 	"Flag"

#define DIM_FLAG	40

/**
 * Flag class
 */
class Flag: public WObject {

private:
  GLfloat width;	///< width
  GLfloat height;	///< height
  GLuint texid;		///< texture number

  uint8_t wiggle;	///< wiggle count
  uint8_t force;	///< wind force
  GLfloat zrot;		///< rotations x y z in degree

  GLfloat mesh[DIM_FLAG + 1][DIM_FLAG + 1][3];
  /**< The 3D array for the points on the grid of our "wave" */

public:
  static const OClass oclass;	///< class variable

  virtual const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist

  Flag(char *l);	///< Constructor

  static WObject * (creator)(char *l);
  /**< Creates from fileline */

  virtual void changePermanent(float lasting);
  /**< Permanent motions */

  virtual void render();
  /**< Specific rendering */

private:
  virtual void behavior();
  /**< Sets behaviors */

  virtual void inits();
  /**< Makes specific inits */

  virtual void parser(char *l);
  /**< Parses */

  virtual void draw();
  /**< Draws the flag */
};

#endif
