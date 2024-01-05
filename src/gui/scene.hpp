//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)       http://vreng.enst.fr/
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
 *  scene.hpp : scene for the VREng GUI
 *  VREng Project
 *  Author: Eric Lecolinet - ENST Paris
 *  http://www.enst.fr/~elc  http://vreng.enst.fr/
 */
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
  
  void getScene(GLint coords[4]);
  void getScene(GLint& x, GLint& y, GLsizei& w, GLsizei& h);
  void setScene(GLint x, GLint y, GLsizei width, GLsizei height);
  void setBackground(UColor& color);
  bool isInitialized();
  void toggleHud();

  //notused// void setNetDelay(int msec);
  //notused// int getNetDelay() const;
  
private:
  friend class Gui;		// cycles net_delay

  class Widgets& gw;		///< gui widget
  bool is_visible;		///< true if visible (not iconified).
  bool is_initialized;		///< true if initialized
  bool is_initCB_launched;	///< true if initCB launched
  bool is_hudvisible;		///< true if hud is visible
  uint32_t cycles;		///< scenes per sec
  uint32_t net_delay;		///< default is 500ms
  UTimer render_timer, net_timer;
  ULabel message;
  UBackground background;
  UBox hudbox;
  UStr hud_line1, hud_line2, hud_line3, hud_line4, hud_line5, hud_line6;
  class Hud *hud;

  void init();
  /** Inits the scene */

  void loopScene();
  /** Paints the scene */

  void resize(UResizeEvent&, int width, int height);
  /** Resizes the scene */

  void netTimeoutCB();

  void refreshHud();

  // callback methods for UBox
  void paintCB(UPaintEvent&);
  void resizeCB(UResizeEvent&);
};

#endif
