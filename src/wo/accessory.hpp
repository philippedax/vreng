//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2012 Philippe Dax
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
#ifndef ACCESSORY_HPP
#define ACCESSORY_HPP

#include "wobject.hpp"
#include "cloth.hpp"

#define ACCESSORY_TYPE	80
#define ACCESSORY_NAME	"Accessory"

/**
 * Accessory class
 */
class Accessory: public Cloth {

public:
  static const OClass oclass;	///< class variable.

  virtual const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist.

  Accessory(char *l);	///< Constructor.

  static WObject * (creator)(char *l);
  /**< Create from fileline. */

  virtual void changePermanent(float lasting);
  /**< Position movement. */

  virtual void quit();
  /**< Quits. */

private:
  static const float LSPEED;    ///< linear speed

  bool following;
  float speed;
  float slowx;
  float slowy;
  float slowz;
  float shiftx;
  float shifty;
  float shiftz;

  virtual void parser(char *l);
  /**< Parses fileline. */

  virtual void defaults();
  /**< Default values. */

  virtual void behavior();
  /**< Sets behavior. */

  virtual void init();
  /**< Initializes object. */

  virtual void follow();
  virtual void takeoff();
  virtual void drop();

  // callbacks
  static void follow_cb(Accessory *accessory, void *d, time_t s, time_t u);
  static void takeoff_cb(Accessory *accessory, void *d, time_t s, time_t u);
  static void drop_cb(Accessory *accessory, void *d, time_t s, time_t u);
};

#endif
