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
#ifndef NAVIG_HPP
#define NAVIG_HPP

#include <ubit/ubit.hpp>

class Widgets;

/**
 * Navig class
 */
class Navig {
public:
  Navig(Widgets*, class Scene&);
  UBox& createManipulator();
  
  void selectObject(ObjInfo*, int button_no);
  void startMotion(UMouseEvent&, Mvt* mvt_x, Mvt* mvt_y);
  void doMotion(UMouseEvent&);
  void stopMotion();
  
private:
  friend class Widgets;
  Widgets& gw;
  float xref, yref;
  bool followMouseMode;		///< flag to follow mouse.
  int depthsel;
  UStr selected_object_url;
  UStr object_name, object_class;
  ULabel object_infos;
  UPopmenu object_menu;
  UCtlmenu navig_menu;
  uptr<UMenu> opened_menu;

  void initNavigMenu();
  void setMouseRef(UMouseEvent& e);
  void mousePressB1orB3(UMouseEvent&, int x, int y, int button_no);
  void mousePressB2(UMouseEvent&, int x, int y, int button_no);
  void canvasMousePressCB(UMouseEvent&);  
  void canvasMouseReleaseCB(UMouseEvent&);
  void canvasMouseDragCB(UMouseEvent&);
  void canvasMouseMoveCB(UMouseEvent&);
  void canvasKeyPressCB(UKeyEvent&);
  void canvasKeyReleaseCB(UKeyEvent&);
 };

#endif
