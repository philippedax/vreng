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
/*
 *  gui.hpp : public interface for the VREng GUI
 *  VREng Project
 *  Author: Eric Lecolinet - ENST Paris
 *
 *  WWW: http://www.enst.fr/~elc  http://vreng.enst.fr/
 */
#ifndef GUI_HPP
#define GUI_HPP 1

#include "config.h"
#include <vector>
#include <ubit/ubit.hpp>
using namespace ubit;

struct GuiItem;

/**
 * Gui Class
 */
class Gui {
public:
  Gui();
  
  void createWidgets();
  void showWidgets(bool = true);
  
  class Scene* getScene();
  ///< returns a pointer to the scene.
  
  int getCycles();
  ///< returns the current number of frames.

  void gotoWorld(const UStr& url_or_name); 
  ///< go to this world.
  
  void showNavigator();
  void showManipulator();
  void clearInfoBar(class WObject*);
  
  void writeMessage(const char *mode, const char *from, const char *mess);
  ///< writes a message.
  
  void addChannelSources(int canal, int table[], int table_size);
  ///< NB: canal=0 for tab_fd / canal=1 for tab_manager_fd.
  
  void removeChannelSources(int canal);
  ///< NB: canal=0 for tab_fd / canal=1 for tab_manager_fd.

  GuiItem * addUser(class User*);
  void updateUser(class User*);
  void removeUser(class User*);
  void pauseUser();
  
  GuiItem * addWorld(class World *world, bool isCurrent);
  void updateWorld(class World *world, bool isCurrent);
  void removeWorld(class World *world);
  
  // Cart
  void showCartDialog(bool flag);
  GuiItem* addCart(class WObject *po);
  void removeCart(class WObject *po, int target);
  void updateCart(class WObject*);  ///< Updates the infoBar when a cart item is selected.
  
  // Vnc, Vrelet, Carrier, Board
  void launchVncConnect(class Vnc*);  ///< Lauches vnc dialog.
  void setToVnc(class Vnc*);          ///< Redirects events to vnc object.
  void setToVrelet(class Vrelet*);    ///< Redirects events to vrelet object.
  void setToCarrier(class Carrier*);  ///< Redirects events to carrier object.
  void setToBoard(class Board*);      ///< Redirects events to board object.
  
  // Ocaml
  void initClicked();
  void getClicked(int *click, float clicked[]);
  WObject* getSelectedObject();
  
private:
  friend class GuiWidgets;
  friend class Scene;
  friend class Navig;
  friend class Panel;
  struct ChannelSources;
  std::vector<ChannelSources*> channel_sources;	///< table of network events
  class GuiWidgets* widgets;	     ///< the widgets of the GUI
  class WObject* selected_object;  ///< the solid that is currently selected
  class Vnc* vnc;		               ///< vnc pointer
  class Carrier* carrier;	         ///< carrier pointer
  class Vrelet* vrelet;		         ///< vrelet pointer
  class Board* board;		         ///< board pointer
};

#endif
