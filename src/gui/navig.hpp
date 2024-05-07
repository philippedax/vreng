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
// navig.hpp
//---------------------------------------------------------------------------
#ifndef NAVIG_HPP
#define NAVIG_HPP

#include "widgets.hpp"

class Widgets;


/**
 * Navig class
 */
class Navig {
 private:
  Widgets& gw;
  float xref, yref;
  int depthsel;
  UStr object_name, object_class;
  ULabel object_infos;			///< infos box
  UPopmenu object_menu;
  UCtlmenu navig_menu;
  uptr<UMenu> opened_menu;

  void navigator();

  void pressB1orB3(UMouseEvent& e, int x, int y, int button_no);
  void pressB2(UMouseEvent& e, int x, int y);

  // Callbacks
  void mousePressCB(UMouseEvent& e);  
  void mouseReleaseCB(UMouseEvent& e);
  void mouseDragCB(UMouseEvent& e);
  void mouseMoveCB(UMouseEvent& e);
  void mouseRefCB(UMouseEvent& e);
  void keyPressCB(UKeyEvent& e);
  void keyReleaseCB(UKeyEvent& e);

  void objectMotion(UMouseEvent& e, Motion* mvt_x, Motion* mvt_y);

 public:
  Navig(Widgets*, class Scene&);	///< constructor

  UBox& manipulator();
  
  void selectObject(ObjInfo* objinfo);

  void userMotion(UMouseEvent& e, Motion* mvt_x, Motion* mvt_y);
  void doMotion(UMouseEvent& e);
  void stopMotion();
  void clearInfoBar();
};

#endif
