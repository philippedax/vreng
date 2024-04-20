//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)       https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2017 Philippe Dax
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
// fractal.hpp
//---------------------------------------------------------------------------
#ifndef FRACTAL_HPP
#define FRACTAL_HPP


#include "wobject.hpp"

#define FRACTAL_TYPE	83
#define FRACTAL_NAME	"fractal"


/**
 * Fractal class
 */
class Fractal: public WO {
 public:
  static const OClass oclass;   ///< class variable

  const OClass* getOClass() {return &oclass;}

  Fractal(char *l);	///< Constructor

  void updateTime(time_t s, time_t u, float *lasting);
  /**< Updates times. */

  void changePosition(float lasting);

  void render();

  static void funcs();	///< init funclist

  static WO * (creator)(char *l);
  /**< Creates from file line */

 private:
  //static const float DEF_TTL;
  uint8_t state;
  uint16_t level_in; 
  uint16_t level; 
  float width;
  float height;
  float turtle_x, turtle_z, turtle_r, turtle_theta;
  float left_w_factor;
  float right_w_factor;
  float left_h_factor;
  float right_h_factor;
  float x, z, x2, z2;
  float color[3];
  bool mycolor;

  void parser(char *l);
  /**< Parses file line. */

  void defaults();
  /**< Sets default values. */

  void behaviors();
  /**< Sets behaviors values. */

  void geometry();
  /**< Make a default solid */

  void inits();
  /**< Initializations. */

  void drawbranch(float w, float x0, float z0, float x1, float z1);
  /**< Draws branch. */

  void generate(float x, float z, float w, float h, float a, uint16_t l);
  /**< Generates particles. */

  void turn(float angle);

  float point(float x1, float z1, float x2, float z2);

  void step();
};


#endif
