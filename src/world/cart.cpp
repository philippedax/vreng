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
// cart.cpp
//
// Cart handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "cart.hpp"
#include "user.hpp"	// localuser
#include "gui.hpp"	// addCart, dialogCart
#include "netobj.hpp"	// netop
#include "sql.hpp"	// VSql
#include "pref.hpp"	// g.user

#include <vector>


const OClass Cart::oclass(CART_TYPE, "Cart", NULL);

// local
std::vector<WO*> Cart::cartList;	// vector of objects in Cart


void Cart::parser(char *l)
{
  defaults();
  l = tokenize(l);
  l = parseName(l, name.given);	// basket
}

/** defaults values */
void Cart::defaults()
{
  number = 0;
  type = CART_TYPE;
}

Cart::Cart(char *l)
{
  parser(l);

  // If Cart is really persistent, these lines should be deleted
  // systematicaly remove objects
  vsql = new VSql();
  vsql->deleteRows(this);
}

/**
 * Adds the object to the logical cart
 * and calls the addCart to manage the cart widget
 * called by Thing::dropIntoBasket
 */
void Cart::addToCart(WO *po)
{
  // find object's world list and remove object from it
  switch (po->mode) {
    case MOBILE:
      po->delFromList(mobileList);
      po->setVisible(false);	// render invisible the object
      vsql = new VSql();	// first take the VSql handle;
      if (vsql) {
        vsql->insertRow(po);
        //echo("cartRow: (%s,%s)", po->objectName(), po->ownerName());
      }
    default:
      break;
  }

  // remove object from collision checking grid
  po->delGrid();

  // informs the GUI
  po->guip = ::g.gui.addCart(po);
  if (! number) {
    ::g.gui.showCartDialog(true);	// popup cartDialog
  }

  // net deletion declaration
  if (! po->isPermanent() && po->netop) {
    delete po->netop;
  }
  po->netop = NULL;

  cartList.push_back(po);
  number++;
}

/**
 * Leaves the object in the current world
 */
void Cart::leave(WO *po)
{
  // remove object from the cartList
  for (std::vector<WO*>::iterator it = cartList.begin(); it != cartList.end(); ++it) {
    if (*it == po) {
      cartList.pop_back();
    }
  }

  // set the object's new coordinates & state
  float near = 0.5;
  po->pos.x = localuser->pos.x + near * cos(localuser->pos.az);
  po->pos.y = localuser->pos.y + near * sin(localuser->pos.az);
  po->pos.z = localuser->pos.z + 0.5;	// visible by eyes
  po->move.ttl = 0;

  // restore object into mobileList
  mobileList.push_back(po);

  // render visible the object coming back into the world
  po->setVisible(true);
  po->enableBehavior(PERSISTENT);

  // show the object
  po->updatePosition();

  // update the object's name with the type name as prefix
  char tmpname[64];
  sprintf(tmpname, "%s-%s", po->typeName(), po->objectName());
  if (! po->name.given)
    po->name.given = new char[OBJNAME_LEN];
  strcpy(po->name.given, tmpname);
  po->updateNames();

  // owner is user
  po->setOwner();

  // declare that the object has moved for VSql update
  po->pos.alter = true;

  vsql = new VSql();     // first take the VSql handle;
  if (vsql) {
    po->vsql = vsql;		// copy it into the object
    vsql->deleteRow(po, CART_NAME, po->objectName(), "");
    vsql->insertRow(po);
    trace1(DBG_SQL, "leaveFromCart: %s", po->objectName());
  }

  // declare the object creation to the network
  if (! po->isPermanent() && po->netop)
    po->netop->declareCreation();

  if (number) number--;
  if (! number) ::g.gui.showCartDialog(false);	// switch-off cartDialog
  po->resetFlashy();
}

/**
 * Removes the object, (removeCart yet called)
 */
void Cart::removeFromCart(WO *po)
{
  // remove from cartList
  for (std::vector<WO*>::iterator it = cartList.begin(); it != cartList.end(); ++it) {
    if (*it == po) {
      cartList.pop_back();
    }
  }

  vsql = new VSql();     // first take the VSql handle;
  if (vsql) {
    vsql->deleteRow(po, CART_NAME, po->objectName(), "");
    trace1(DBG_SQL, "removeFromCart: %s", po->objectName());
  }

  if (number) number--;
  if (! number) ::g.gui.showCartDialog(false);	// switch-off cartDialog

  // destroy object from cart
  po->toDelete();
}

void Cart::quit()
{
  savePersist();
}

void Cart::funcs() {}
