//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)       https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2024 Philippe Dax
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
#ifndef SCENE_HPP
#define SCENE_HPP

#include "widgets.hpp"

// the Scene is a standard UBox (transparent menus and windows are then possible)
struct GLSection : public ubit::UGraph::Glpaint {
  GLSection(class Scene*);
};


/**
 * Scene class
 */
class Scene : public ubit::UBox {

 public:
  Scene(class Widgets*);	///< constructor.

  void getScene(GLint& x, GLint& y, GLsizei& w, GLsizei& h);
  void setScene(GLint x, GLint y, GLsizei width, GLsizei height);
  void setBackground(UColor& color);
  void toggleHud();

 private:
  friend class Gui;		// cycles net_delay

  class Widgets& gw;		///< gui widget
  bool is_visible;		///< true if visible (not iconified).
  bool is_initialized;		///< true if initialized
  bool is_launched;		///< true if initCB launched
  bool is_hudvisible;		///< true if hud is visible
  uint32_t cycles;		///< scenes per sec
  uint32_t net_delay;		///< default is 500ms
  UTimer render_timer, net_timer;
  ULabel message;
  UBackground background;
  UBox hudbox;			///< hud box
  UStr hud_row1, hud_row2, hud_row3, hud_row4, hud_row5, hud_row6;
  class Hud *hud;

  //
  // methods
  //
  void init();
  /** Inits the scene */

  void resize(UResizeEvent&, int width, int height);
  /** Resizes the scene */

  void netTimeoutCB();

  void refreshHud();

  void paintCB(UPaintEvent&);
  /** Repaints the scene */

  void resizeCB(UResizeEvent&);
};

#endif
