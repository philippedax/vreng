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
// theme.hpp
//---------------------------------------------------------------------------
#ifndef THEME_HPP
#define THEME_HPP

#include "widgets.hpp"


/**
 * Theme class
 */
class Theme {
 public:
  Theme();
  
  void init();
  ///< inits the theme; must be called before using the GUI.
  
  float paletteWidth, paletteHeight, messagePaletteWidth, controlPanelHeight;
  
  UArgs mainStyle, menuStyle, paletteStyle, panelStyle, scrollpaneStyle, objectTypeStyle, objectNameStyle;
 
  UColor joystickColor, joystickArmColor, chatColor, noticeColor, requestColor, warningColor, messColor;

  UIma Forward, Backward, BigLeft, BigRight, Left, Right, Up, LeftTrans, RightTrans, UpDown, JoystickUpDown, JoystickLeftRight;
  
  UIma Transx, Transy, Transz, Rotxleft, Rotxright, Rotzleft, Rotzright, Rotyup, Rotydown;
  
  UIma Save, Folder, Edit, Exit, List, Prefs, Tools, Doc, World, Book, Movie, Axis, Grid2D, Grid3D, Cart, AddObj, Camera, BigEye, Eyes, Person, Counter, Jumelles, Sandbox;

};

#endif
