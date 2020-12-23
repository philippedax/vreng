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
/*
 *  gui.cpp : generic interface and callback functions for the VREng GUI
 *  NOTE: this file should be common to all GUI variants
 *
 *  VREng Project / Ubit GUI
 *  Authors: Eric Lecolinet and Philippe Dax
 *
 *  (C) 1999-2008 Eric Lecolinet / ENST Paris
 *  WWW: http://www.enst.fr/~elc/ubit
 */
#include "vreng.hpp"
#include "gui.hpp"
#include "widgets.hpp"
#include "panels.hpp"
#include "navig.hpp"
#include "message.hpp"
#include "scene.hpp"
#include "motion.hpp"
#include "world.hpp"    // current
#include "wobject.hpp"  // WObject
#include "user.hpp"	// localuser
#include "vnc.hpp"      // Vnc
#include "cart.hpp"     // Cart
#include "pref.hpp"	// width3D
#include "cache.hpp"	// check
#include "audio.hpp"	// start
#include "event.hpp"	// NetIncoming NetTimeout
#include "channel.hpp"	// Channel
#include "vac.hpp"	// resolveWorldUrl, getUrlAndChannel
#include "theme.hpp"


// Constructor
Gui::Gui() :
widgets(null),
selected_object(null),  // the object that is currently selected
vnc(null),
carrier(null),
vrelet(null) {
}

void Gui::createWidgets()
{
  widgets = new Widgets(this);  // creates the widgets of the GUI
  trace(DBG_INIT, "Gui initialized");
}

void Gui::showWidgets(bool state)
{
  if (widgets) widgets->show(state);
}

Scene* Gui::getScene()
{
  return widgets ? &widgets->scene : null;
}

int Gui::getCycles()
{
  return (widgets) ? widgets->scene.cycles : 0;
}

void Gui::writeMessage(const char *mode, const char *from, const char *mess)
{
  if (widgets) widgets->message.writeMessage(mode, from, mess);
}

// ocaml
void Gui::initClicked()
{
  if (widgets) widgets->message.initClicked();
}

// ocaml
void Gui::getClicked(int *click, float clicked[])
{
  if (widgets) widgets->message.getClicked(click, clicked);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Handling Network
//
struct Gui::ChannelSources : public vector<USource*> {
  ChannelSources(int count) : vector<USource*>(count) {}
};

void Gui::addChannelSources(int channel, int table[], int table_count)
{
  if (channel >= int(channel_sources.size())) {
    channel_sources.resize(channel+1, null);
  }
  channel_sources[channel] = new ChannelSources(table_count);
  for (int k=0; k < table_count; k++) {
    USource* s =  new USource(table[k]);
    s->onAction(ucall(table[k], ::NetIncoming));
    channel_sources[channel]->push_back(s);
  }
}

void Gui::removeChannelSources(int channel)
{
  if (channel >= int(channel_sources.size()))  return;
  
  ChannelSources* css = channel_sources[channel];
  if (css == null)  return;

  for (unsigned int k=0; k < css->size(); k++) delete (*css)[k];
  delete css;
  channel_sources[channel] = null;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Handling Cart
//
void Gui::showCartDialog(bool flag)
{
  widgets->panels.showCartDialog(flag);
}

GuiItem* Gui::addCart(WObject *cart)
{
  if (!cart) return NULL;

  notice("Item %s added to cart", NN(cart->getInstance()));
  
  GuiItem* gu = new GuiItem();
  gu->add(ustr(cart->getInstance()) + ucall(this, cart, &Gui::updateCart));

  widgets->setInfobar(null);
  widgets->carts.add(gu);
  return gu;
}

void Gui::removeCart(WObject *cart, int action)
{
  if (!cart) return;

  if (cart->isGui()) {
    widgets->setInfobar(null);
    widgets->carts.remove(*cart->getGui()); // remove item's box
    cart->resetGui();
  }
  if (! localuser)  return;

  switch (action) {
  case Cart::LEAVE:
    notice("%s left in %s world", NN(cart->getInstance()),NN(World::current()->getName()));
    localuser->cart->leave(cart);
    break;
  case Cart::REMOVE:
    notice("%s removed from cart", NN(cart->getInstance()));
    localuser->cart->removeFromCart(cart);
    break;
  }
}

void Gui::updateCart(WObject* po)
{
  UBox* actions_cart = &uhbox(ulabel(ugroup(g.theme.objectTypeStyle
                              + USymbol::right
                              + ustr(po->names.type))
                              + " "
                              + ugroup(g.theme.objectNameStyle
                                       + po->getInstance()
                                      )
                                    )
                              + uitem("Leave"
                                       + ucall(this, po, int(Cart::LEAVE), &Gui::removeCart)
                                     )
                              + uitem("Remove"
                                       + ucall(this, po, int(Cart::REMOVE), &Gui::removeCart)
                                     )
                             );
  widgets->setInfobar(actions_cart);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Handling User
//
GuiItem * Gui::addUser(User *user) 	// when a new user comes in
{
  if (!user) return NULL;

  notice("Avatar %s joins %s", NN(user->getInstance()), NN(user->worldName()));
  return widgets->addUser(user);
}

void Gui::removeUser(User *user)	// when an user quits
{
  if (!user) return;

  notice("Avatar %s leaves %s", NN(user->getInstance()), NN(user->worldName()));
  if (user->isGui()) {
    widgets->removeUser(user->getGui());
    // MS : for Ubit at least, removeUser does a delete on
    // the guiItem structure. Safer to mark it a NULL than to
    // carry an invalid pointer around.
    user->resetGui();
  }
}

void Gui::updateUser(User *user)
{
  if (!user) return;

  notice("Avatar %s is in %s", NN(user->getInstance()), NN(user->worldName()));
  if (user->isGui()) widgets->updateUser(user->getGui(), user);
}

void Gui::pauseUser()
{
  widgets->callAction(UserAction::UA_PAUSE);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Handling World
//
void Gui::gotoWorld(const UStr& url_or_name)
{
  if (url_or_name.empty())  return;

  const char* urlorname = url_or_name.c_str();
  char urlvre[URL_LEN], chanstr[CHAN_LEN];
  Vac *vac = Vac::current();

  strcpy(chanstr, DEF_VRE_CHANNEL);
  if (strchr(urlorname, '/')) {	// url or path
    strcpy(urlvre, urlorname);
    if (! Cache::check(urlvre))  return;	// bad url
    if (! vac->resolveWorldUrl(urlvre, chanstr)) {
      if (strcmp(chanstr, DEF_VRE_CHANNEL))  return;	// url not found
    }
  }
  else {	// worldname
    if (! vac->getUrlAndChannel(urlorname, urlvre, chanstr))  return;  // world not found
  }
  notice("goto %s at %s", urlvre, chanstr);

  World::current()->quit();
  delete Channel::current();			// delete old Channel
  World::enter(urlvre, chanstr, true);
  World::current()->setChanAndJoin(chanstr);	// join new channel

  if (audioactive) Audio::start(chanstr);
}

GuiItem * Gui::addWorld(World *world, bool isCurrent)
{
  if (! world)  return NULL;

  return widgets->addWorld(world, isCurrent);
}

void Gui::removeWorld(World *world)
{
  if (! world)  return;

  if (world->isGui()) widgets->removeWorld(world);
}

void Gui::updateWorld(World *world, bool isCurrent)
{
  if (! world)  return;

  if (world->isGui()) widgets->updateWorld(world, isCurrent);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Handling VNC
//
void Gui::setToVnc(Vnc* _vnc)
{
  vnc = _vnc;
}

void Gui::launchVncConnect(Vnc* _vnc)
{
  VncDialog::create(widgets, _vnc);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Handling Vrelet
//
void Gui::setToVrelet(Vrelet* _vrelet)
{
  vrelet = _vrelet;
}

void Gui::setToBoard(Board* _board)
{
  board = _board;
}

void Gui::setToCarrier(Carrier* _carrier)
{
  carrier = _carrier;
  Motion* motion = Motion::pointer();
  motion->setToCarrier(carrier);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

WObject* Gui::getSelectedObject()
{
  return selected_object;
}

void Gui::showNavigator()
{
  widgets->panels.showManipulator(false);
}

void Gui::showManipulator()
{
  widgets->panels.showManipulator(true);
}

// clears the info bar.
void Gui::clearInfoBar(WObject *obj)
{
  if (obj == selected_object) {
    selected_object = NULL;
    widgets->navig.selectObject(NULL, 0); // clears the objectBar
  }
}
