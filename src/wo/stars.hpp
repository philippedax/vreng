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
#ifndef STARS_HPP
#define STARS_HPP

#include "wobject.hpp"

#define STARS_TYPE	30
#define STARS_NAME	"Stars"


typedef struct {
  float x[2], y[2], v[2];
  float dx, dy, dr, rot;
} tStar;


/**
 * Stars class
 */
class Stars: public WObject {

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

  virtual const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist

  Stars(char *l);	///< Constructor

  static WObject * (creator)(char *l);
  /**< Creation from a file */

  virtual void changePermanent(float lasting);
  /**< Equation to move permanently */

  virtual void render();
  /**< Specific rendering */

  virtual void quit();
  /**< Quits properly */

private:
  virtual void parser(char *l);
  /**< Parses */

  virtual void defaults();
  /**< Sets default values */

  virtual void create(int n, int d);
  virtual void move();
  virtual void update();
  virtual bool pointed(int n);
  virtual void show(int n);
  virtual void rotate(float *x, float *y, float rot);
};

#endif
