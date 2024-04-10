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
 *  theme.cpp : Ubit themes for the VREng GUI
 *
 *  VREng / Ubit Project
 *  Authors: Eric Lecolinet and Philippe Dax
 *
 *  Ubit Toolkit: A Brick Construction Game Model for Creating GUIs
 *  Please refer to the Ubit GUI Toolkit Home Page for details.
 *
 *  (C) 2002/2009 Philippe Dax and Eric Lecolinet - ENST Paris
 */
#include "xpm.hpp"
#include "theme.hpp"
#include "ubit/uxpm.hpp"	// xpm files


Theme::Theme() {}

/** Initialization - static */
void Theme::init()
{
  controlPanelHeight = 120;
  paletteWidth = 100;
  paletteHeight = 100; 
  messagePaletteWidth = 350; 
  
  mainStyle += UBackground::metal + UColor::black;
  menuStyle += ualpha(0.5) + UBackground::black + UColor::white + UFont::bold;
  paletteStyle += ualpha(0.5) + UBackground::black + UColor::white + UBorder::line;
  panelStyle += *new URoundBorder(UBorder::LINE, UColor::navy, UColor::white,5,5,15,15);
  scrollpaneStyle += ualpha(0.6) + UBackground::black + UColor::white + UFont::bold + UBorder::shadowIn + usize(100);
  objectTypeStyle += UColor::navy + UFont::bold;
  objectNameStyle += UColor::red;
  
  joystickColor = UColor::white;
  joystickArmColor = UColor::orange;
  chatColor = UColor::white;
  noticeColor = UColor::orange;
  requestColor = UColor::green;
  warningColor = UColor::red;
  messColor = UColor::orange;
  
  Forward.loadFromData(UXpm::bigUp);
  Backward.loadFromData(UXpm::bigDown);
  BigLeft.loadFromData(UXpm::bigLeft);
  BigRight.loadFromData(UXpm::bigRight);
  Left.loadFromData(UXpm::leftarrow);
  Right.loadFromData(UXpm::rightarrow);
  Up.loadFromData(UXpm::uparrow);
  LeftTrans.loadFromData(UXpm::leftarrow);
  RightTrans.loadFromData(UXpm::rightarrow);
  UpDown.loadFromData(updown_xpm);   // UXpm::uparrow
  JoystickUpDown.loadFromData(updown_xpm);
  JoystickLeftRight.loadFromData(lateral_xpm);
 
  Transx.loadFromData(transx_xpm);
  Transy.loadFromData(transy_xpm);
  Transz.loadFromData(transz_xpm);
  Rotxleft.loadFromData(rotxleft_xpm);
  Rotxright.loadFromData(rotxright_xpm);
  Rotzleft.loadFromData(rotzleft_xpm);
  Rotzright.loadFromData(rotzright_xpm); 
  Rotyup.loadFromData(rotyup_xpm); 
  Rotydown.loadFromData(rotydown_xpm);  
  
  Save.loadFromData(UXpm::diskette);
  Folder.loadFromData(UXpm::folder);
  Exit.loadFromData(UXpm::stop);
  Edit.loadFromData(UXpm::edit);
  List.loadFromData(list_xpm);
  Prefs.loadFromData(preferences_xpm); 
  Tools.loadFromData(toolbox_xpm); 
  World.loadFromData(world_xpm); 
  Book.loadFromData(UXpm::book);
  Movie.loadFromData(movie_xpm);
  Axis.loadFromData(axis_xpm); 
  Grid2D.loadFromData(grid2d_xpm); 
  Grid3D.loadFromData(grid3d_xpm); 
  Cart.loadFromData(cart_xpm); 
  AddObj.loadFromData(UXpm::windows); 
  Camera.loadFromData(camera_xpm);
  BigEye.loadFromData(bigeye_xpm); 
  Eyes.loadFromData(eyes_xpm); 
  Person.loadFromData(person_xpm);
  Counter.loadFromData(count_xpm);
  Jumelles.loadFromData(jumelles_xpm);
  Sandbox.loadFromData(sandbox_xpm);
}
