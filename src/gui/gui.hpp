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
/*
 *  gui.hpp : public interface for the VREng GUI
 *  VREng Project
 *  Author: Eric Lecolinet - ENST Paris
 *
 *  WWW: http://www.enst.fr/~elc  http://vreng.enst.fr/
 */
#ifndef GUI_HPP
#define GUI_HPP

#include "ubit/ubit.hpp"
#include <vector>

using namespace ubit;


struct GuiItem;

/**
 * Gui class
 */
class Gui {
 public:
  Gui();	// constructor.
 
  void createWidgets();
  void showWidgets(bool = true);
 
  class Scene* scene();
  ///< returns a pointer to the scene.
 
  int getCycles();
  ///< returns the current number of frames.

  void gotoWorld(const UStr& url_or_name); 
  ///< go to this world.
 
  void showNavigator();
  void showManipulator();
  void expandNavig();
  void collapseNavig();
  void expandNotif();
  void collapseNotif();
  void expandAvatar();
  void collapseAvatar();
  void clearInfoBar();
  void clearInfoBar(class WO*);
 
  void writeMessage(const char *mode, const char *from, const char *mess);
  ///< writes a message.
 
  // Channel
  void addChannelSources(int canal, int table[], int table_size);
  void removeChannelSources(int canal);
  ///< NB: canal=0 for tab_fd / canal=1 for tab_manager_fd.

  // User
  GuiItem * addUser(class User*);
  void updateUser(class User*);
  void removeUser(class User*);
  void pauseUser();
 
  // World
  GuiItem * addWorld(class World *world, bool isCurrent);
  void updateWorld(class World *world, bool isCurrent);
  void removeWorld(class World *world);
  
  // Cart
  void showCartDialog(bool flag);
  GuiItem* addCart(class WO *po);
  void removeCart(class WO *po, int target);
  void updateCart(class WO*);		///< Updates the infoBar when a cart item is selected.
  
  // Vnc, Vrelet, Carrier, Board
  void launchVnc(class Vnc*);		///< Lauches vnc dialog.
  void setToVnc(class Vnc*);		///< Redirects events to vnc object.
  void setToVrelet(class Vrelet*);	///< Redirects events to vrelet object.
  void setToCarrier(class Carrier*);	///< Redirects events to carrier object.
  void setToBoard(class Board*);	///< Redirects events to board object.
  
  // Ocaml
  void initClicked();
  void getClicked(uint8_t *click, float clicked[]);
  WO* getSelectedObject();
  
 private:
  friend class Widgets;
  friend class Scene;
  friend class Navig;
  friend class Panel;

  struct ChannelSources;
  std::vector<ChannelSources*> channel_sources;	///< table of network events

  class Widgets* widgets;		///< the widgets of the GUI
  class WO* selected_object;	///< the object that is currently selected
  class Vnc* vnc;			///< vnc pointer
  class Carrier* carrier;		///< carrier pointer
  class Vrelet* vrelet;			///< vrelet pointer
  class Board* board;			///< board pointer
};

#endif
