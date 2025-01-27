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
// halo.hpp
//---------------------------------------------------------------------------
#ifndef HALO_HPP
#define HALO_HPP

#include "object.hpp"
#include "cloth.hpp"

#define HALO_TYPE	71
#define HALO_NAME	"Halo"

/**
 * Halo class
 */
class Halo: public Cloth {
 protected:
  static GLfloat RI, RE;
  char strcolor[16];
  GLfloat color[3];

 public:
  enum {
    WEAR,
    TAKEOFF,
    MOVE,
    CREATE_WEAR,
    RECREATE,
    DESTROY
  };

  static const OClass oclass;	///< class variable

  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist

  static Object * (creator)(char *l);
  /**< Creation from a file */

  Halo(char *l);		///< Constructor file
  Halo(class User *pu, void *d, time_t s, time_t u);
  /**< Constructor from World */

  void quit();
  /**< Quits */

 private:
  void defaults();
  /**< Sets default values */

  void parser(char *l);
  /**< Parses file line */

  void geometry();
  /**< Builds solid geometry */

  void behaviors();
  /**< Sets behaviors. */

  void inits();
  /**< Sets initializations. */

  void wear();
  void takeoff();

  // callbacks
  static void wear_cb(Halo *halo, void *d, time_t s, time_t u);
  static void takeoff_cb(Halo *halo, void *d, time_t s, time_t u);

  static void recreate_cb(class User *user, void *d, time_t s, time_t u);

};

#endif
