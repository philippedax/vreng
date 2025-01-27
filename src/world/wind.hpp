//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2009 Philippe Dax
// TelecomParis (Ecole Nationale Superieure des Telecommunications)
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
// wind.hpp
//---------------------------------------------------------------------------
#ifndef WIND_HPP
#define WIND_HPP

#include "object.hpp"

#define WIND_TYPE	54
#define WIND_NAME	"Wind"


/**
 * Wind class
 */
class Wind: public Object {
 private:
  static Wind *wind;		///< singleton.
  static const char url[36];	///< wind url.
  uint16_t speed;		///< linear speed in km/s.
  float orient;			///< orientation in radians.
  pthread_t tid;

 public:
  static const OClass oclass;	///< class variable.

  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist.

  Wind(char *l);	///< Constructor.

  static Object* (creator)(char *l);	///< Creates from fileline.

  static Wind * current();	///< Gets wind pointer.

  static void * getHttp(void *url);	///< Gets wind data.

  uint16_t getSpeed() const;	///< Gets speed.

  float getOrient() const;	///< Gets orientation.

  void quit();		///< Quits.


 private:
  void defaults();
  /**< Sets default values. */

  void parser(char *l);
  /**< Parses. */

};

#endif
