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
#ifndef RIVER_HPP
#define RIVER_HPP

#include "wobject.hpp"

#define RIVER_TYPE	68
#define RIVER_NAME	"River"

/**
 * River class
 */
class River: public WObject {

protected:
  static const uint8_t DEF_WAVES;
  static const float DEF_WIDTH;
  static const float DEF_DEPTH;
  static const GLfloat DEF_SCALE;
  static const GLfloat DEF_COLOR[];

  uint8_t waves;	///< number of waves
  float width;		///< width
  float depth;		///< depth
  float *mesh;		///< mesh
  float *phase;		///< phase
  float *speed;		///< speed
  float *ampl;		///< amplitude
  GLfloat color[4];	///< basic color
  GLfloat scale;	///< scale factor

public:
  static const OClass oclass;	///< class variable

  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist

  River(char *l);	///< Constructor

  static WObject * (creator)(char *l);
  /**< Creates from fileline */

  virtual void render();
  /**< Renderer */

  virtual void quit();
  /**< Quits */

 private:
  void parser(char *l);
  /**< Parses */

  void defaults();
  /**< Sets defaults values */

  void behavior();
  /**< Sets behavior */

  void makeSolid();
  /**< Makes a solid */

  void inits();
  /**< Do specific inits */

  void draw(float a, float b);
  /**< Draws wave point */

};

#endif
