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
// button.hpp
//---------------------------------------------------------------------------
#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "object.hpp"

#define BUTTON_TYPE	12
#define BUTTON_NAME	"Button"


/**
 * Button class
 */
class Button: public Object {
 private:
  static const uint8_t USENAME_MAX;
  static const uint8_t ACTION_LEN;

  uint8_t num_action0;	///< action if on
  uint8_t num_action1;	///< action if off

 public:
  char *use_names;	///< object instance names

  // properties
  enum {
    PROPSTATE,
    PROPS
  };

  static const OClass oclass;	///< class variable

  const OClass* getOClass() {return &oclass;}

  bool publish(const Pos &oldpos);

  static void funcs();	///< init funlist

  Button(char *l);	///< Constructor
  ~Button() ;		///< Destructor

  static Object * (creator)(char *l);

 private:
  void parser(char *l);
  /**< Parses */

  void defaults();	///< set default values

  void behaviors();	///< set behaviors
  void inits();		///< set initializations

  // GUI and network change callbacks
  static void get_bstate(Button *po, Payload *pp);
  static void put_bstate(Button *po, Payload *pp);
  static void commut(Button *po, void *d, time_t s, time_t us);
};

#endif
