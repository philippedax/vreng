//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
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
//
//  gui.cpp : generic interface and callback functions for the VREng GUI
//
//  VREng Project / Ubit GUI
//  Authors: Eric Lecolinet and Philippe Dax
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "gui.hpp"
#include "widgets.hpp"	// widgets
#include "panels.hpp"	// panels
#include "navig.hpp"	// navig
#include "message.hpp"	// message
#include "scene.hpp"	// scene
#include "motion.hpp"	// Motion
#include "world.hpp"    // current
#include "object.hpp"   // Object
#include "user.hpp"	// localuser
#include "vnc.hpp"      // Vnc
#include "cart.hpp"     // Cart
#include "pref.hpp"	// width3D
#include "audio.hpp"	// start
#include "event.hpp"	// netIncoming
#include "channel.hpp"	// Channel
#include "vac.hpp"	// resolveWorldUrl, getUrlAndChannel
#include "theme.hpp"	// g.theme

#include "ubit/ubit.hpp"

using namespace ubit;


/** Constructor */
Gui::Gui() :
 widgets(null),
 selected_object(null),		// the object that is currently selected
 vnc(null),
 carrier(null),
 vrelet(null)
{}

/** Creates Widgets */
void Gui::createWidgets()
{
  widgets = new Widgets(this);  // creates the widgets of the GUI

  trace1(DBG_INIT, "Gui initialized");
}

/** Shows Widgets */
void Gui::showWidgets(bool state)
{
  if (widgets) widgets->show(state);
}

/** Gets scene pointer */
Scene* Gui::scene()
{
  return (widgets) ? &widgets->scene : null;
}

/** Gets cycles (fps) */
int Gui::getCycles()
{
  return (widgets) ? widgets->scene.cycles : 0;
}

/** Writes a message */
void Gui::writeMessage(const char *mode, const char *from, const char *mess)
{
  if (widgets) widgets->message.writeMessage(mode, from, mess);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Handling Network
//
struct Gui::ChannelSources : public std::vector<USource*> {
  ChannelSources(int count) : std::vector<USource*>(count) {}
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

  for (int k=0; k < int(cs->size()); k++) {
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

/** Adds cart to the palette */
GuiItem* Gui::addCart(Object *cart)
{
  if (! cart) return NULL;

  echo("Item %s added to basket", cart->objectName());
  
  GuiItem* gu = new GuiItem();
  gu->add(ustr(cart->objectName()) + ucall(this, cart, &Gui::updateCart));

  widgets->setInfobar(null);
  widgets->basket.add(gu);
  return gu;
}

/** Removes cart from the palette */
void Gui::removeCart(Object *cart, int action)
{
  if (! cart) return;

  if (cart->guip) {
    widgets->setInfobar(null);
    widgets->basket.remove(*cart->guip); // remove item's box
    cart->guip = NULL;
  }
  if (! localuser)  return;

  switch (action) {
  case Cart::LEAVE:
    echo("%s left in %s world", cart->objectName(), World::current()->name);
    localuser->cart->leave(cart);
    break;
  case Cart::REMOVE:
    echo("%s removed from basket", cart->objectName());
    localuser->cart->removeObject(cart);
    break;
  }
}

/** Updates cart from the palette */
void Gui::updateCart(Object* o)
{
  UBox* actions_cart = &uhbox(ulabel(ugroup(g.theme.objectTypeStyle
                                            + USymbol::right
                                            + ustr(o->name.type)
                                           )
                                     + " "
                                     + ugroup(g.theme.objectNameStyle
                                              + o->objectName()
                                             )
                                    )
                              + uitem("Leave"
                                       + ucall(this, o, int(Cart::LEAVE), &Gui::removeCart)
                                     )
                              + uitem("Remove"
                                       + ucall(this, o, int(Cart::REMOVE), &Gui::removeCart)
                                     )
                             );
  widgets->setInfobar(actions_cart);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Handling Avatar
//
/** Adds avatar to the palette */
GuiItem * Gui::addAvatar(User *user) 	// when a new user comes in
{
  if (! user)  return NULL;

  //echo("Avatar %s joins %s", user->objectName(), user->worldName());
  return widgets->addAvatar(user);
}

/** Removes avatar from the palette */
void Gui::removeAvatar(User *user)	// when an user quits
{
  if (! user)  return;

  //echo("Avatar %s leaves %s", user->objectName(), user->worldName());
  if (user->guip) {
    widgets->removeAvatar(user->guip);
    // MS : for Ubit at least, removeAvatar does a delete on the guiItem structure.
    // safer to mark it a NULL than to carry an invalid pointer around.
    user->guip = NULL;
  }
}

/** Updates avatar from the palette */
void Gui::updateAvatar(User *user)
{
  if (! user)  return;

  //echo("Avatar %s is in %s", user->objectName(), user->worldName());
  if (user->guip) widgets->updateAvatar(user->guip, user);
}

void Gui::pauseAvatar()
{
  widgets->action(User::U_PAUSE);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Handling World
//

/** Opens a new world */
void Gui::openWorld(const UStr& url_or_name)
{
  if (url_or_name.empty()) {
    echo("world null");
    // world to repopulate
    return;
  }

  const char* urlorname = url_or_name.c_str();
  char url[URL_LEN], chan[CHAN_LEN];
  Vac *vac = Vac::current();

  strcpy(chan, DEF_VRENG_CHANNEL);
  if (strchr(urlorname, '/')) {		// url or path
    strcpy(url, urlorname);
    if (! vac->resolveWorldUrl(url, chan)) {
      if (strcmp(chan, DEF_VRENG_CHANNEL)) {
	return;				// url not found
      }
    }
  }
  else {				// worldname
    if (! vac->getUrlAndChannel(urlorname, url, chan)) {
      return;				// world not found
    }
  }
  //echo("goto %s at %s", url, chan);

  World::current()->quit();
  delete Channel::current();		// delete old Channel
  World::enter(url, chan, true);
  World::current()->joinChan(chan);	// join new channel

  if (audioactive) Audio::start(chan);
}

/** Adds a world into the palette */
GuiItem * Gui::addWorld(World *world, bool isCurrent)
{
  if (! world)  return NULL;

  return widgets->addWorld(world, isCurrent);
}

/** Removes a world from the palette */
void Gui::removeWorld(World *world)
{
  if (! world)  return;

  if (world->guip) widgets->removeWorld(world);
}

/** Updates a world from the palette */
void Gui::updateWorld(World *world, bool isCurrent)
{
  if (! world)  return;

  if (world->guip) widgets->updateWorld(world, isCurrent);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Handling Vnc
//
/** Vnc mode */
void Gui::setToVnc(Vnc* _vnc)
{
  vnc = _vnc;
}

void Gui::launchVnc(Vnc* _vnc)
{
  VncDialog::vncDialog(widgets, _vnc);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Handling Vrelet
//
void Gui::setToVrelet(Vrelet* _vrelet)
{
  vrelet = _vrelet;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Handling Board
//
void Gui::setToBoard(Board* _board)
{
  board = _board;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
// Handling selection
//
Object* Gui::getSelectedObject()
{
  return selected_object;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Handling palettes
//
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

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Clears the info bar.
//
void Gui::clearInfoBar()
{
  widgets->navig.clearInfoBar();
}

void Gui::clearInfoBar(Object *obj)
{
  if (obj == selected_object) {
    selected_object = NULL;
    widgets->navig.clearInfoBar();	// clears the InfoBar
  }
}
