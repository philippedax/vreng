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
#ifndef MIRAGE_HPP
#define MIRAGE_HPP

#include "wobject.hpp"

#define MIRAGE_TYPE	7
#define MIRAGE_NAME	"Mirage"


/**
 * Mirage class
 */
class Mirage: public WO {

private:
  static const float ASPEED;

  float aspeed;		///< angular speed
  float azspeed;	///< angular az speed
  float ayspeed;	///< angular ay speed
  float axspeed;	///< angular ax speed
  float radius;		///< radius of the animation
  bool turn;		///< turn rotation az
  bool roll;		///< roll rotation ay
  bool tilt;		///< tilt rotation ax
  bool orbit;		///< orbit rotation
  bool persist;		///< persistency flag
  bool circular;	///< circular rotation
  float centrex, centrey; ///< circular position
  float initialaz, anglerot; ///< circular angles
  uint8_t flares;	///< number of flares
  float scale;		///< flare scale
  GLfloat color[3];	///< flare color

public:
  /* actions */
  enum {
    APPROACH,
    MOVE,
    DESTROY,
    RECREATE,
    NONE
  };

  static const OClass oclass;	///< class variable

  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist

  Mirage(char *l);	///< Constructor

  Mirage(WO *user, char *form);
  /**< Constructor from palette */

  Mirage(class World *pw, void *d, time_t s, time_t u);
  /**< Constructor from World */

  static WO * (creator)(char *l);
  /**< Creates from fileline */

  void changePermanent(float lasting);
  /**< Permanent movement */

  void quit();
  /**< Quits */

private:
  void parser(char *l);
  /**< Parses file line */

  void defaults();
  /**< Sets default values */

  void setName();
  /**< Sets name */

  void behaviors();
  /**< Sets behaviors flags */

  //GUI callbacks
  static void destroy(Mirage *po, void *d, time_t s, time_t u);
  static void recreate(class World *w, void *d, time_t s, time_t u);
};

#endif
