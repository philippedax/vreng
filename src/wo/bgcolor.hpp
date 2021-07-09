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
#ifndef BGCOLOR_HPP
#define BGCOLOR_HPP

#include "wobject.hpp"

#define BGCOLOR_TYPE	22
#define BGCOLOR_NAME	"Bgcolor"


/**
 * Bgcolor class
 */
class Bgcolor: public WObject {
  friend class Render;		///< glClearColor

private:
  static Bgcolor *bgcolor;	///< The singleton instance of this class

  float color[4];	/// buffer color

public:
  static const OClass oclass;	///< class variable

  virtual const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist

  Bgcolor(char *l);	///< Constructor
  Bgcolor();		///< Contructor initial

  static WObject * (creator)(char *l); ///< Creates from fileline

  virtual void quit();		///< Quits

private:
  virtual void parser(char *l);
  /**< Parses */

  virtual void black();		///< default bgcolor
};

#endif
