//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)       https://github.com/philippedax/vreng
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
// panels.hpp
//---------------------------------------------------------------------------
#ifndef PANELS_HPP
#define PANELS_HPP

#include "palette.hpp"


/**
 * Panels class
 */
class Panels {
 private:
  friend class Widgets;

  Widgets& gw;
  class Joystick1* joystick1;
  class Joystick2* joystick2;
  UBox& manipulator;  
  UBox right_panel, control_panel, main_panel;
  UBar control_bar;

 public:
  Panels(class Widgets*, class Scene&);

  Palette *pnavig;
  Palette *pnotif;
  Palette *pavatar;

  void showManipulator(bool state);
  void showBasket(bool state);

  void expandNavig();
  void collapseNavig();
  void expandNotif();
  void collapseNotif();
  void expandAvatar();
  void collapseAvatar();
};

#endif
