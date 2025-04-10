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
std::vector<Object*> Cart::cartList;	// vector of objects in Cart


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
void Cart::addToCart(Object *o)
{
  // find object's world list and remove object from it
  switch (o->mode) {
    case MOBILE:
      o->delFromList(mobileList);
      o->setVisible(false);	// render invisible the object
      vsql = new VSql();	// first take the VSql handle;
      if (vsql) {
        vsql->insertRow(o);
        //echo("cartRow: (%s,%s)", o->objectName(), o->ownerName());
      }
    default:
      break;
  }

  // remove object from collision checking grid
  o->delGrid();

  // informs the GUI
  o->guip = ::g.gui.addCart(o);
  if (! number) {
    ::g.gui.showCartDialog(true);	// popup cartDialog
  }

  // net deletion declaration
  if (! o->isPermanent() && o->netop) {
    delete o->netop;
  }
  o->netop = NULL;

  cartList.push_back(o);
  number++;
}

/**
 * Leaves the object in the current world
 */
void Cart::leave(Object *o)
{
  // remove object from the cartList
  for (std::vector<Object*>::iterator it = cartList.begin(); it != cartList.end(); ++it) {
    if (*it == o) {
      cartList.pop_back();
    }
  }

  // set the object's new coordinates & state
  float near = 0.5;
  o->pos.x = localuser->pos.x + near * cos(localuser->pos.az);
  o->pos.y = localuser->pos.y + near * sin(localuser->pos.az);
  o->pos.z = localuser->pos.z + 0.5;	// visible by eyes
  o->move.ttl = 0;

  // restore object into mobileList
  mobileList.push_back(o);

  // render visible the object coming back into the world
  o->setVisible(true);
  o->enableBehavior(PERSISTENT);

  // show the object
  o->updatePos();

  // update the object's name with the type name as prefix
  char tmpname[64];
  sprintf(tmpname, "%s-%s", o->typeName(), o->objectName());
  if (! o->name.given)
    o->name.given = new char[OBJNAME_LEN];
  strcpy(o->name.given, tmpname);
  o->updateNames();

  // owner is user
  o->setOwner();

  // declare that the object has moved for VSql update
  o->pos.alter = true;

  vsql = new VSql();     // first take the VSql handle;
  if (vsql) {
    o->vsql = vsql;		// copy it into the object
    vsql->deleteRow(o, CART_NAME, o->objectName(), "");
    vsql->insertRow(o);
    trace1(DBG_SQL, "leave: %s", o->objectName());
  }

  // declare the object creation to the network
  if (! o->isPermanent() && o->netop)
    o->netop->declareCreation();

  if (number) number--;
  if (! number) ::g.gui.showCartDialog(false);	// switch-off cartDialog
  o->resetFlashy();
}

/**
 * Removes the object, (removeCart yet called)
 */
void Cart::removeObject(Object *o)
{
  // remove from cartList
  for (std::vector<Object*>::iterator it = cartList.begin(); it != cartList.end(); ++it) {
    if (*it == o) {
      cartList.pop_back();
    }
  }

  vsql = new VSql();     // first take the VSql handle;
  if (vsql) {
    vsql->deleteRow(o, CART_NAME, o->objectName(), "");
    trace1(DBG_SQL, "removeObject: %s", o->objectName());
  }

  if (number) number--;
  if (! number) ::g.gui.showCartDialog(false);	// switch-off cartDialog

  // destroy object from cart
  o->toDelete();
}

void Cart::quit()
{
  savePersist();
}

void Cart::funcs() {}
