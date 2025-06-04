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
// clock.hpp
//---------------------------------------------------------------------------
#ifndef CLOCK_HPP
#define CLOCK_HPP

#include "object.hpp"

#define CLOCK_TYPE	27
#define	CLOCK_NAME	"Clock"


/**
 * Clock class
 */
class Clock: public Object {
 private:
  enum {
    SECOND,
    MINUTE,
    HOUR
  }; // needles

  uint8_t needle;	///< which needle: s | m | h
  uint8_t sec;		///< secondes
  uint8_t min;		///< minutes
  uint8_t hour;		///< hour
  uint16_t yday;	///< day number in year
  uint8_t sec_last;	///< last sec
  uint8_t min_last;	///< last min
  uint8_t hour_last;	///< last hour
  float daytime;	///< midday time
  float risetime;	///< rise time
  float falltime;	///< fall time
  bool haveneedle;	///< flag

 public:
  static const OClass oclass;	///< class variable

  const OClass* getOClass() {return &oclass;}

  Clock(char *l);	///< Constructor
  Clock();		///< Constructor internal
  ~Clock();		///< Destructor

  static void funcs();	///< init funclist

  static Object * (creator)(char *l);

  void permanent(float lasting);

 private:
  void inits();		///< init time

  void parser(char *l);
  /**< Parses */
};

#endif
