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
struct GLSection : public ubit::UGraph::Glpaint { GLSection(class Scene*); };
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class Scene : public ubit::UBox {
public:
  Scene(class GuiWidgets*);
  
  bool isInitialized() {return is_initialized;} ///< checks whether the scene is initialized.

  void setNetDelay(int msec) {net_delay = msec;}  // default is 500ms
  int getNetDelay() const {return net_delay;}
  
  void getCoords(GLint coords[4]);
  void getCoords(GLint& x, GLint& y, GLsizei& w, GLsizei& h);
  void setViewport(GLint x, GLint y, GLsizei width, GLsizei height);
  void setBackground(UColor&);
  
private:
  friend class Gui;
  class GuiWidgets& gw;
  bool is_visible;		 ///< true if visible (not iconified).
  bool is_initialized;
  bool is_startCB_launched;
  int  net_delay;     ///< default is 500ms
  int  cycles;
  UTimer render_timer, net_timer;
  ULabel message;
  UBox hudbox;
  UStr hud_line1, hud_line2, hud_line3, hud_line4, hud_line5;
  UBackground background;
  class Hud *hud;

  void initScene();
  void paintScene();
  void resizeScene(UResizeEvent&, int width, int height);
  void updateHud();
  void netTimeoutCB();

  // callback methods for UBox
  void paintCB(UPaintEvent&);
  void resizeCB(UResizeEvent&);
};

#endif
