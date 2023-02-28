//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://www.vreng.enst.fr/
//
// Copyright (C) 1997-2021 Philippe Dax
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
#include "widgets.hpp"	// widgets
#include "panels.hpp"	// panels
#include "navig.hpp"	// navig
#include "message.hpp"	// message
#include "scene.hpp"	// scene
#include "motion.hpp"	// Motion
#include "world.hpp"    // current
#include "wobject.hpp"  // WObject
#include "user.hpp"	// localuser
#include "vnc.hpp"      // Vnc
#include "cart.hpp"     // Cart
#include "pref.hpp"	// width3D
#include "audio.hpp"	// start
#include "event.hpp"	// netIncoming
#include "channel.hpp"	// Channel
#include "vac.hpp"	// resolveWorldUrl, getUrlAndChannel
#include "theme.hpp"	// g.theme


// Constructor
Gui::Gui() :
 widgets(null),
 selected_object(null),  // the object that is currently selected
 vnc(null),
 carrier(null),
 vrelet(null)
{
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

Scene* Gui::scene()
{
  return (widgets) ? &widgets->scene : null;
}

int Gui::getCycles()
{
  return (widgets) ? widgets->scene.cycles : 0;
}

void Gui::writeMessage(const char *mode, const char *from, const char *mess)
{
  if (widgets) widgets->message.writeMessage(mode, from, mess);
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
    USource* s = new USource(table[k]);
    s->onAction(ucall(table[k], ::netIncoming));
    channel_sources[channel]->push_back(s);
  }
}

void Gui::removeChannelSources(int channel)
{
  if (channel >= int(channel_sources.size()))  return;
  
  ChannelSources* cs = channel_sources[channel];
  if (! cs)  return;

  for (int k=0; k < cs->size(); k++) {
    delete (*cs)[k];
  }
  delete cs;
  channel_sources[channel] = null;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Handling Cart
//
void Gui::showCartDialog(bool flag)
{
  widgets->panels.showBasket(flag);
}

GuiItem* Gui::addCart(WObject *cart)
{
  if (! cart) return NULL;

  echo("Item %s added to basket", cart->getInstance());
  
  GuiItem* gu = new GuiItem();
  gu->add(ustr(cart->getInstance()) + ucall(this, cart, &Gui::updateCart));

  widgets->setInfobar(null);
  widgets->basket.add(gu);
  return gu;
}

void Gui::removeCart(WObject *cart, int action)
{
  if (! cart) return;

  if (cart->isGui()) {
    widgets->setInfobar(null);
    widgets->basket.remove(*cart->getGui()); // remove item's box
    cart->resetGui();
  }
  if (! localuser)  return;

  switch (action) {
  case Cart::LEAVE:
    echo("%s left in %s world", cart->getInstance(), World::current()->getName());
    localuser->cart->leave(cart);
    break;
  case Cart::REMOVE:
    echo("%s removed from basket", cart->getInstance());
    localuser->cart->removeFromCart(cart);
    break;
  }
}

void Gui::updateCart(WObject* po)
{
  UBox* actions_cart = &uhbox(ulabel(ugroup(g.theme.objectTypeStyle
                                            + USymbol::right
                                            + ustr(po->names.type)
                                           )
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
  if (! user)  return NULL;

  //echo("Avatar %s joins %s", user->getInstance(), user->worldName());
  return widgets->addUser(user);
}

void Gui::removeUser(User *user)	// when an user quits
{
  if (! user)  return;

  //echo("Avatar %s leaves %s", user->getInstance(), user->worldName());
  if (user->isGui()) {
    widgets->removeUser(user->getGui());
    // MS : for Ubit at least, removeUser does a delete on the guiItem structure.
    // safer to mark it a NULL than to carry an invalid pointer around.
    user->resetGui();
  }
}

void Gui::updateUser(User *user)
{
  if (! user)  return;

  //echo("Avatar %s is in %s", user->getInstance(), user->worldName());
  if (user->isGui()) widgets->updateUser(user->getGui(), user);
}

void Gui::pauseUser()
{
  widgets->callAction(User::UA_PAUSE);
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

  strcpy(chanstr, DEF_VRENG_CHANNEL);
  if (strchr(urlorname, '/')) {	// url or path
    strcpy(urlvre, urlorname);
    if (! vac->resolveWorldUrl(urlvre, chanstr)) {
      if (strcmp(chanstr, DEF_VRENG_CHANNEL))  return;	// url not found
    }
  }
  else {	// worldname
    if (! vac->getUrlAndChannel(urlorname, urlvre, chanstr))  return;  // world not found
  }
  //echo("goto %s at %s", urlvre, chanstr);

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

// Handling Vnc
//
void Gui::setToVnc(Vnc* _vnc)
{
  vnc = _vnc;
}

void Gui::launchVnc(Vnc* _vnc)
{
  VncDialog::vncDialog(widgets, _vnc);
}

// Handling Vrelet
//
void Gui::setToVrelet(Vrelet* _vrelet)
{
  vrelet = _vrelet;
}

// Handling Board
//
void Gui::setToBoard(Board* _board)
{
  board = _board;
}

// Handling Carrier
//
void Gui::setToCarrier(Carrier* _carrier)
{
  carrier = _carrier;
  Motion* motion = Motion::current();
  motion->setToCarrier(carrier);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Ocaml
//
void Gui::initClicked()
{
  if (widgets) widgets->message.initClicked();
}

void Gui::getClicked(uint8_t *nclick, float clicked[])
{
  if (widgets) widgets->message.getClicked(nclick, clicked);
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

void Gui::expandNavig()
{
  widgets->panels.expandNavig();
}

void Gui::collapseNavig()
{
  widgets->panels.collapseNavig();
}

void Gui::expandNotif()
{
  widgets->panels.expandNotif();
}

void Gui::collapseNotif()
{
  widgets->panels.collapseNotif();
}

void Gui::expandAvatar()
{
  widgets->panels.expandAvatar();
}

void Gui::collapseAvatar()
{
  widgets->panels.collapseAvatar();
}

// clears the info bar.
void Gui::clearInfoBar()
{
  widgets->navig.clearInfoBar();
}

void Gui::clearInfoBar(WObject *obj)
{
  if (obj == selected_object) {
    selected_object = NULL;
    widgets->navig.clearInfoBar();	// clears the InfoBar
  }
}
