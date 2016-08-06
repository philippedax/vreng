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
#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "wobject.hpp"

#define BUTTON_TYPE	12
#define BUTTON_NAME	"Button"


/**
 * Button class
 */
class Button: public WObject {

 private:
  static const uint8_t USENAME_MAX;
  static const uint8_t ACTION_LEN;

  uint8_t num_action0;	///< action if on
  uint8_t num_action1;	///< action if off

 public:
  char *use_names;	///< object instance names

  /* properties */
  enum {
    PROPSTATE,
    PROPS
  };

  static const OClass oclass;	///< class variable

  virtual const OClass* getOClass() {return &oclass;}

  virtual bool updateToNetwork(const Pos &oldpos);

  static void funcs();	///< init funlist

  Button(char *l);	///< Constructor

  static WObject * (creator)(char *l);

  virtual void quit();	///< when leaving

 private:
  virtual void parser(char *l);
  /**< Parses */

  virtual void defaults();	///< set default values

  // GUI and network change callbacks
  static void get_bstate(Button *po, Payload *pp);
  static void put_bstate(Button *po, Payload *pp);
  static void commut(Button *po, void *d, time_t s, time_t us);
};

#endif
