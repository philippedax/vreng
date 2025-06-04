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
// entry.hpp
//---------------------------------------------------------------------------
#ifndef ENTRY_HPP
#define ENTRY_HPP

#include "object.hpp"

#define ENTRY_TYPE	21
#define ENTRY_NAME	"Entry"


/**
 * Entry class
 */
class Entry: public Object {
 private:
  static Entry *entry;	///< The singelton instance of this class

 public:
  static const OClass oclass;	///< class variable

  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist

  Entry(char *l);	///< Constructor from file
  Entry(float *newpos);	///< Constructor from gate
  Entry() {}		///< Constructor from world
  ~Entry() {}		///< Destructor

  static Object * (creator)(char *l);	///< Creates from fileline

  static Entry * current();
  /**< Returns the current entry */

  void query(class User *user);
  /**< Answers the entry positions */

 private:
  void parser(char *l);
  /**< Parses */
};

#endif
