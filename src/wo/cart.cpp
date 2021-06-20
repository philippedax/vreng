//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2011 Philippe Dax
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
#include "vreng.hpp"
#include "cart.hpp"
#include "carrier.hpp"	// takeFromCart
#include "user.hpp"	// localuser
#include "gui.hpp"	// addCart, dialogCart
#include "netobj.hpp"	// noh
#include "vrsql.hpp"	// VRSql
#include "render.hpp"	// showSolidList
#include "pref.hpp"	// g.user
#include <list>
using namespace std;

const OClass Cart::oclass(CART_TYPE, "Cart", NULL);

list<WObject*> Cart::cartList;	// list of objects in Cart


void Cart::parser(char *l)
{
  defaults();
  l = tokenize(l);
  l = parse()->parseName(l, names.given);	// basket
  names.instance = names.given;
}

/* defaults values */
void Cart::defaults()
{
  number = 0;
  type = CART_TYPE;
}

Cart::Cart(char *l)
{
  parser(l);

  // If Cart is really persistent, these lines should be deleted
#if VRSQL
  // systematicaly remove objects
  if ((psql = VRSql::getVRSql())) {
    psql->deleteRows(this);
  }
#endif
}

/**
 * Adds the object to the logical cart
 * and calls the addCart to manage the cart widget
 * called by Thing::dropIntoBasket
 */
void Cart::addToCart(WObject *po)
{
  // find object's world list and remove object from it
  switch (po->mode) {
    case MOBILE:
      po->delFromMobile();
      po->setVisible(false);	// render invisible the object
#if VRSQL
      psql = VRSql::getVRSql();	// first take the VRSql handle;
      if (psql) {
        psql->insertCol(CART_NAME, "owner", po->getInstance(), "");
        psql->updateString(po, CART_NAME, "owner", po->getInstance(), "", ::g.user);
        trace(DBG_SQL, "cartRow: (%s,%s)", po->getInstance(), po->ownerName());
      }
#endif
    default:
      break;
  }

  // remove object from collision checking grid
  po->delFromGrid();

  // informs the GUI
  po->guip = ::g.gui.addCart(po);
  if (! number) ::g.gui.showCartDialog(true);	// popup cartDialog

  // net deletion declaration
  if (! po->isPermanent() && po->noh) delete po->noh;
  po->noh = NULL;

  cartList.push_back(po);
  number++;
}

bool Cart::isSomethingInCart(WObject *po)
{
  return number;
}

/**
 * Leaves the object in the current world
 */
void Cart::leave(WObject *po)
{
  // remove object from the cartList
  for (list<WObject*>::iterator it = cartList.begin(); it != cartList.end(); ++it)
    if (*it == po) {
      cartList.erase(it);	//dax1 remove(*it) ???
    }

  // set the object's new coordinates & state
  float near = 0.5;
  po->pos.x = localuser->pos.x + near * cos(localuser->pos.az);
  po->pos.y = localuser->pos.y + near * sin(localuser->pos.az);
  po->pos.z = localuser->pos.z + 0.5;	// visible by eyes
  po->move.ttl = 0;

  // restore object into mobileList
  po->addToMobile();

  // render visible the object coming back into the world
  po->setVisible(true);
  //dax1 po->enableBehavior(PERSISTENT);

  // show the object
  po->updatePosition();

  // update the object's name with the type name as prefix
  char tmpname[64];
  sprintf(tmpname, "%s-%s", po->typeName(), po->named());
  strcpy(po->names.given, tmpname);
  po->updateNames();

  // owner is user
  po->setOwner();

  // declare that the object has moved for VRSql update
  po->pos.alter = true;

#if VRSQL
  psql = VRSql::getVRSql();     // first take the VRSql handle;
  if (psql) {
    po->psql = psql;		// copy it into the object
    psql->deleteRow(po, CART_NAME, po->getInstance(), "");
    psql->insertRow(po);
    psql->updatePos(po);
    psql->updateOwner(po);
    psql->updateGeom(po, po->geometry);
    trace(DBG_SQL, "leaveFromCart: %s", po->getInstance());
  }
#endif

  // declare the object creation to the network
  if (! po->isPermanent() && po->noh)
    po->noh->declareObjCreation();

  if (number) number--;
  if (! number) ::g.gui.showCartDialog(false);	// switch-off cartDialog
  po->resetFlashy();
}

/**
 * Removes the object, (removeCart yet called)
 */
void Cart::removeFromCart(WObject *po)
{
  // remove from cartList
  for (list<WObject*>::iterator it = cartList.begin(); it != cartList.end(); ++it) {
    if (*it == po) {
      cartList.erase(it);	// dax1 remove(*it) ???
    }
  }

#if VRSQL
  psql = VRSql::getVRSql();     // first take the VRSql handle;
  if (psql) {
    psql->deleteRow(po, CART_NAME, po->getInstance(), "");
    trace(DBG_SQL, "removeFromCart: %s", po->getInstance());
  }
#endif

  if (number) number--;
  if (! number) ::g.gui.showCartDialog(false);	// switch-off cartDialog

  // destroy object from cart
  po->toDelete();
}

void Cart::quit()
{
  quitPersistency();
}

void Cart::funcs() {}
