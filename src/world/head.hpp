//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2011 Philippe Dax
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
// head.hpp
//---------------------------------------------------------------------------
#ifndef HEAD_HPP
#define HEAD_HPP

#include "object.hpp"

#define HEAD_TYPE	81
#define HEAD_NAME	"Head"

#define LWO_SCALE	0.0028
#define _3DS_SCALE	0.0220


/**
 * Head class
 */
class Head: public Object {
 protected:
  bool visible;
  uint8_t model_e;			///< types of model
  uint8_t model_t;			///< types of 3D model
  float scale;
  float dx, dy, dz, dax, day, daz;	///< difference with avatar positions
  char modelname[16];
  class Model *model;			///< model instance
  float color[3];

 public:
  enum {
    MALE,
    FEMALE,
    NONE
  };

  enum {
    RECREATE
  };

  /* properties */
  enum {
    PROPHNAME,
    PROPXY,
    PROPZ,
    PROPAZ,
    PROPAX,
    PROPAY,
    PROPS       ///< last item = properties number
  };

  static const OClass oclass;	///< class variable

  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist

  Head(char *l);		///< Constructor.
  Head(class User *user, void *d, time_t s, time_t u); ///< Constructor from User.
  Head(class User *user, const char *url, const float *skin); ///< Constructor from User.

  static Object * (creator)(char *l);
  /**< Creation from a file */

  void permanent(float lasting);
  /**< movement */

  void render();
  /**< Renders */

  void quit();
  /**< Quits */

  void visibility(bool flag);
  /**< Handle visibility */

 protected:
  void behaviors();
  /**< Sets behaviors */

  void setName(const char *modelname);
  /**< Sets qualified name */

  void setName();
  /**< Sets qualified name */

  void inits();
  /**< Do specific inits */

 private:
  void defaults();
  /**< Sets default values */

  void parser(char *l);
  /**< Parses xml */

  void geometry();
  /**< Builds solid geometry */

  uint8_t getGender(const char *name);
  /**< Gets model id */

  /** callbacks */
  static void recreate_cb(class User *user, void *d, time_t s, time_t u);
};

#endif
