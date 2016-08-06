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
#ifndef CART_HPP
#define CART_HPP

#include "wobject.hpp"
#include <list>

#define CART_TYPE	42
#define CART_NAME	"Cart"


/**
 * Cart class
 */
class Cart: public WObject {

protected:
  uint8_t number;	///< number of objects inside cart
  class User *user;	///< owner of the cart
  static std::list<WObject*> cartList; ///< list of objects in the cart

public:
  // target values handled by GuiRemoveCart in gui.cc
  enum {
    REMOVE,
    LEAVE
  };

  Cart(char *l);
  /**< constructor */

  static const OClass oclass;   ///< class variable

  virtual const OClass* getOClass() {return &oclass;};

  static void funcs();	///< init funclist

  virtual void addToCart(WObject *po);
  /**< called by thingAddToCart */

  virtual void removeFromCart(WObject *po);
  /**< called by GuiRemoveCart (target = "REMOVE") */

  virtual void leaveIntoWorld(WObject *po);
  /**< called by GuiRemoveCart (target = "LEAVE") */

  virtual bool isSomethingInCart(WObject *po);
  /**< checks if something is inside cart */

  virtual void quit();
  /**< Quits properly */

 private:
  virtual void parser(char *l);
  /**< Parses */

  virtual void defaults();
  /**< Sets default values */
};

#endif
