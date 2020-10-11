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
#ifndef PANELS_HPP
#define PANELS_HPP

/**
 * Panels class
 */
class Panels {
public:
  static int 
  RIGHT_PANEL_WIDTH,
  CONTROL_PANEL_HEIGHT,
  LIST_PALETTE_WIDTH,
  LIST_PALETTE_HEIGHT,
  MESSAGE_PALETTE_WIDTH,
  MESSAGE_PALETTE_HEIGHT;

  Panels(class Widgets*, class Scene&);
  void showManipulator(bool state);
  void showCartDialog(bool state);
  Widgets &gw;

private:
  friend class Widgets;
  class Joystick* joystick;
  class Pitch* pitch;
  UBox& manipulator;  
  UBox right_panel, control_panel, main_panel;
  UBar control_bar;
};

#endif
