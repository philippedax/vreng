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
 *  panels.cpp : Ubit Panels for the VREng GUI
 *
 *  VREng / Ubit Project
 *  Authors: Eric Lecolinet and Philippe Dax
 *
 *  Ubit Toolkit: A Brick Construction Game Model for Creating GUIs
 *  Please refer to the Ubit GUI Toolkit Home Page for details.
 *
 *  (C) 2002/2009 Eric Lecolinet - ENST Paris
 *  WWW: http://www.enst.fr/~elc/ubit
 */

#include "vreng.hpp"
#include "widgets.hpp"
#include "theme.hpp"
#include "panels.hpp"
#include "palette.hpp"
#include "navig.hpp"
#include "scene.hpp"
#include "joystick.hpp"
#include "pitch.hpp"
#include "capture.hpp"
#include "theme.hpp"
#include "useraction.hpp"
//#include "hud.hpp"		// toggle
#include "axis.hpp"		// toggle
#include "grid.hpp"		// toggle
#include "world.hpp"
#include "channel.hpp"
#include "message.hpp"

static void toggleHudCB(Widgets*) { Hud * hud = new Hud(); hud->toggleHud(); }

static void toggleAxisCB(Widgets*) { Axis::axis()->toggle(); }

static void toggleGridCB(Widgets*, int flag) {
  switch (flag) {
    case 0: Grid::grid()->toggleGrid(); break;
    case 1: Grid::grid()->toggleGrid3d(); break;
    case 2: Grid::grid()->toggleOverlap(); break;
  }
}

void Panels::showManipulator(bool state)
{
  manipulator.show(state);
  joystick->show(!state);
  pitch->show(!state);
}

void Panels::showCartDialog(bool state) { cerr << "TO BE DONE!" << endl; }
#if 0 //dax
 UDialog *cartDialog,
 cartDialog =    // relies on carts_pane created by createPanes()
 &udialog(utitle("Basket")
 + uheight(150) + uwidth(100)
 + carts_pane
 + ubottom() + ubutton(UFont::bold + uhcenter() + " Close " + ucloseWin())
 );
#endif

static void sandboxCB(Widgets*)
{
 World::current()->quit();
 delete Channel::current();	// delete Channel
 World::enter(NULL, NULL, false);
 }


Panels::Panels(Widgets* _gw, Scene& scene) :
gw(*_gw),
joystick(new Joystick(_gw, (int) g.theme.controlPanelHeight/2 - 20)),
pitch(new Pitch(_gw, 25)),
manipulator(_gw->navig.createManipulator())
{
  // WORLDS
  UScrollpane& worlds_spane = uscrollpane(true, false, uvbox(gw.worlds));
  worlds_spane.showVScrollButtons(false);

  // CARTS
  UScrollpane& carts_spane = uscrollpane(true, false, uvbox(gw.carts));
  carts_spane.showVScrollButtons(false);

  // AVATARS
  UScrollpane& avatars_spane = uscrollpane(true, false, uvbox(gw.avatars));
  avatars_spane.showVScrollButtons(false);

  // right panel - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  right_panel
  .addAttr(UOrient::vertical //+ usize(g.theme.rightPanelWidth)
           + uvspacing(8) + upadding(4,2))
  .add(uvbox(g.theme.panelStyle + utop()
             + ulabel(g.theme.World +" Worlds" + utip("Visited worlds"))
             + uvflex()
             + ubox(g.theme.scrollpaneStyle + worlds_spane)
             )
     + uvbox(g.theme.panelStyle + utop()
             + ulabel(g.theme.Folder +" Basket" + utip("Basket content"))
             + uvflex()
             + ubox(g.theme.scrollpaneStyle + carts_spane)
             )
     + uvbox(g.theme.panelStyle + utop()
             + ulabel(g.theme.Person +" Avatars" + utip("Current avatars"))
             + uvflex()
             + ubox(g.theme.scrollpaneStyle + avatars_spane)
             )
       );
    right_panel.show(false); // not shown by default

  // navigator - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  manipulator.show(false);  // joystick shown by default
  joystick->show(true);
  pitch->show(true);

  UBox& navig_box = uhbox
  (g.theme.panelStyle
   + uvcenter()
   + manipulator  // either manipulator or joystick is shown
   + joystick
   + "   "
   + pitch
   );

  // palettes - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  Palette& navig_palette = *new Palette(g.theme.paletteStyle
               + navig_box);
  navig_palette.setPos(5|UPos::LEFT, 5|UPos::BOTTOM);
  navig_palette.setTitle(UColor::yellow + UFont::bold + "Joystick");
  scene.add(navig_palette);

  Palette& messages_palette = *new Palette(g.theme.paletteStyle
               + usize(g.theme.messagePaletteWidth, g.theme.paletteHeight)
               + gw.messages.createMessagePanel(true));
  messages_palette.setPos(50|UPERCENT_CTR, 5|UPos::BOTTOM);
  messages_palette.setTitle(UColor::yellow + UFont::bold + "Infos & Messages");
  scene.add(messages_palette);

  Palette& avatars_palette = *new Palette(g.theme.paletteStyle
               + usize(g.theme.paletteWidth, g.theme.paletteHeight)
               + avatars_spane);
  avatars_palette.setPos(5|UPos::RIGHT, 5|UPos::BOTTOM);
  avatars_palette.setTitle(UColor::yellow + UFont::bold + "Avatars");
  scene.add(avatars_palette);

  // control panel - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  control_panel
  .add(UOrient::horizontal + usize().setHeight(g.theme.controlPanelHeight)
       + UBackground::velin
       + upadding(2,0) + uhspacing(10)
       + uleft()
       + navig_box + " "
       + uhflex()
       + ubox(g.theme.panelStyle + gw.messages.createMessagePanel(false))
       );
  control_panel.show(false);

  // more button  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  UBox& expand_collapse_btn = ubutton
  (utip("Expand/Collapse Panels")
   + UOn::select   / ustr("Collapse")
   + UOn::deselect / ustr("Expand")
   + UOn::select   / ushow(control_panel, true)
   + UOn::select   / ushow(right_panel, true)
   + UOn::deselect / ushow(control_panel, false)
   + UOn::deselect / ushow(right_panel, false)
   + UOn::select   / ushow(navig_palette, false)
   + UOn::select   / ushow(messages_palette, false)
   + UOn::select   / ushow(avatars_palette, false)
   + UOn::deselect / ushow(navig_palette, true)
   + UOn::deselect / ushow(messages_palette, true)
   + UOn::deselect / ushow(avatars_palette, true)
   );
  expand_collapse_btn.setSelected(false);

  // control bar  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  UListbox& viewlist = ulistbox
  (UOrient::horizontal + UBackground::none + upadding(1,1)
   + uitem(utip("Main viewpoint first person")
           + ucall(int(UserAction::FIRSTVIEW), Widgets::callAction)
           + g.theme.BigEye)
   + uitem(utip("Third person viewpoints")
           + ucall(int(UserAction::THIRDVIEWFAR), Widgets::callAction)
           + g.theme.Eyes)
   + uitem(utip("Alternate viewpoints")
           + ucall(int(UserAction::SWITCHVIEW), Widgets::callAction)
           + g.theme.Jumelles)
   );
  viewlist.select(0);

  control_bar
  .addAttr(UOrient::horizontal + uhcenter() + uhspacing(6))
  .add(uhcenter()
       + upadding(8,0).setBottom(14)
       + "View:"
       + viewlist
       + usepar()
       + "Show:"
       + uitem( utip("Show axis")
               + ucall(_gw, toggleAxisCB)
               + g.theme.Axis)
       + uitem( utip("Show 2D grid")
               + ucall(_gw, 0, toggleGridCB)
               + g.theme.Grid2D)
       + uitem( utip("Show 3D grid")
               + ucall(_gw, 1, toggleGridCB)
               + g.theme.Grid3D)
       + uitem( utip("Show hud")
               + ucall(_gw, toggleHudCB)
               + g.theme.Counter)
       + uitem( utip("Show overview")
               + ucall(int(UserAction::MAPVIEW), Widgets::callAction)
               + g.theme.World)
       + usepar()
       + "Objects:"
       + uitem(utip("Show object cart")
               // + ushow(*gw.cartDialog, true)         !! A COMPLETER
               + g.theme.Cart)
       + uitem(utip("Add object to the scene")
               + ushow(gw.addobj_dialog, true)
               + g.theme.AddObj)
       + uitem(utip("Test: sandbox world !")
               + ucall(_gw, sandboxCB)
               + UPix::question)
       + usepar()
       + "Throw:"
       + uitem(utip("Throw dart")
               + ucall(int(UserAction::DART), Widgets::callAction)
               + UPix::ray)
       + uitem(utip("Throw bullet")
               + ucall(int(UserAction::BULLET), Widgets::callAction)
               + UFont::bold + UFont::xx_large + UColor::red + ".")
       + usepar()
       + "Snap:"
       + uitem(utip("Capture screenshot in JPG")
               + ucall(&gw.capture, &Capture::writeJPGImage)
               + g.theme.Camera)
       + uitem(utip("Capture screenshot in PNG")
               + ucall(&gw.capture, &Capture::writePNGImage)
               + g.theme.Camera)
#if WANT_GL2PS
       + uitem(utip("Capture screenshot in SVG")
               + ucall(&gw.capture, &Capture::writeSVGImage)
               + g.theme.Camera)
#endif
       + uitem(utip("Capture video (start/stop)")
               + UOn::select   / ucall(&gw.capture, &Capture::startVideo)
               + UOn::deselect / ucall(&gw.capture, &Capture::stopVideo)
               + UOn::select   / g.theme.Exit
               + UOn::deselect / g.theme.Movie)

       + uhflex() + ubox() + uright() + expand_collapse_btn
       );

  // main panel - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  main_panel.addAttr(UOrient::vertical)
  .add(control_bar + control_panel);
}
