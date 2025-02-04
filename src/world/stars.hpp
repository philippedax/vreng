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
// stars.hpp
//---------------------------------------------------------------------------
#ifndef STARS_HPP
#define STARS_HPP

#include "object.hpp"

#define STARS_TYPE	30
#define STARS_NAME	"Stars"


typedef struct {
  float x[2], y[2], v[2];
  float dx, dy, dr, rot;
} tStar;


/**
 * Stars class
 */
class Stars: public Object {
 private:
  static const uint16_t NUMBER;
  static const uint16_t MAXPOS;
  static const float SPEED;
  static const float WARP;

  uint16_t number;	///< number of stars
  uint16_t maxpos;	///< maxpos
  float speed;		///< speed
  float warp;		///< limit
  tStar *stars;		///< me

  enum {
    STARS_NORMAL,
    STARS_WEIRD
  };
  enum {
    STARS_STREAK,
    STARS_CIRCLE1
  };

 public:
  static const OClass oclass;	///< class variable

  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist

  Stars(char *l);	///< Constructor

  static Object * (creator)(char *l);
  /**< Creation from a file */

  void permanent(float lasting);
  /**< Equation to move permanently */

  void render();
  /**< Specific rendering */

  void quit();
  /**< Quits properly */

 private:
  void parser(char *l);
  /**< Parses */

  void defaults();
  /**< Sets default values */

  void behaviors();
  void geometry();
  void init();
  void create(int n, int d);
  void move();
  void update();
  bool pointed(int n);
  void draw(int n);
  void rotate(float *x, float *y, float rot);
};

#endif
