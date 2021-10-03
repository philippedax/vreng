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
#include "panels.hpp"
#include "theme.hpp"
#include "palette.hpp"
#include "navig.hpp"
#include "scene.hpp"
#include "joystick.hpp"		// Joystick1 Joystick2
#include "pref.hpp"		// ::g.pref.expand
#include "user.hpp"		// UserAction
#include "axis.hpp"		// toggle
#include "grid.hpp"		// toggle
#include "world.hpp"
#include "channel.hpp"
#include "message.hpp"
#include "capture.hpp"


static void toggleHudCB(Widgets*) { g.gui.scene()->toggleHud(); }
static void toggleAxisCB(Widgets*) { Axis::axis()->toggle(); }

static void toggleGridCB(Widgets*, int flag)
{
  switch (flag) {
    case 0: Grid::grid()->toggleGrid2d(); break;
    case 1: Grid::grid()->toggleGrid3d(); break;
    case 2: Grid::grid()->toggleOverlap(); break;
  }
}

void Panels::showManipulator(bool state)
{
  manipulator.show(state);
  joystick1->show(!state);
  joystick2->show(!state);
}

static void sandboxCB(Widgets*)
{
  World::current()->quit();
  delete Channel::current();	// delete Channel
  World::enter(NULL, NULL, false);
}

Panels::Panels(Widgets* _gw, Scene& scene) :
  gw(*_gw),
  joystick1(new Joystick1(_gw, (int) g.theme.controlPanelHeight/2 - 20)),
  joystick2(new Joystick2(_gw, 25)),
  manipulator(_gw->navig.manipulator())
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
  .add(uvbox(g.theme.panelStyle
             + utop()
             + ulabel(g.theme.World
             + " Worlds"
             + utip("Visited worlds"))
             + uvflex()
             + ubox(g.theme.scrollpaneStyle
             + worlds_spane)
            )
     + uvbox(g.theme.panelStyle
             + utop()
             + ulabel(g.theme.Folder
             + " Basket"
             + utip("Basket content"))
             + uvflex()
             + ubox(g.theme.scrollpaneStyle
             + carts_spane)
            )
     + uvbox(g.theme.panelStyle
             + utop()
             + ulabel(g.theme.Person
             + " Avatars"
             + utip("Current avatars"))
             + uvflex()
             + ubox(g.theme.scrollpaneStyle
             + avatars_spane)
            )
      );
    right_panel.show(false); // not shown by default

  // navigator - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  manipulator.show(false);  // joystick shown by default
  joystick1->show(true);
  joystick2->show(true);

  UBox& navig_box = uhbox(g.theme.panelStyle
                          + uvcenter()
                          + manipulator  // either manipulator or joystick is shown
                          + joystick1
                          + "   "
                          + joystick2
                         );

  // palettes - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  Palette& navig_palette = *new Palette(g.theme.paletteStyle
               + navig_box);
  navig_palette.setPos(5|UPos::LEFT, 5|UPos::BOTTOM);
  navig_palette.setTitle(UColor::orange + UFont::bold + "Navigator");
  scene.add(navig_palette);

  Palette& messages_palette = *new Palette(g.theme.paletteStyle
               + usize(g.theme.messagePaletteWidth, g.theme.paletteHeight)
               + gw.message.createMessagePanel(true));
  messages_palette.setPos(50|UPERCENT_CTR, 5|UPos::BOTTOM);
  messages_palette.setTitle(UColor::orange + UFont::bold + "Notifications & Messages");
  scene.add(messages_palette);

  Palette& avatars_palette = *new Palette(g.theme.paletteStyle
               + usize(g.theme.paletteWidth, g.theme.paletteHeight)
               + avatars_spane);
  avatars_palette.setPos(5|UPos::RIGHT, 5|UPos::BOTTOM);
  avatars_palette.setTitle(UColor::orange + UFont::bold + "Avatars");
  scene.add(avatars_palette);

  if (::g.pref.expand == false) {
    navig_palette.collapse();
    messages_palette.collapse();
    avatars_palette.collapse();
  }

  // control panel - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  control_panel
  .add(UOrient::horizontal + usize().setHeight(g.theme.controlPanelHeight)
       + UBackground::velin
       + upadding(2,0) + uhspacing(10)
       + uleft()
       + navig_box + " "
       + uhflex()
       + ubox(g.theme.panelStyle + gw.message.createMessagePanel(false))
      );
  control_panel.show(false);

  // more button  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  UBox& expand_collapse = ubutton
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
  expand_collapse.setSelected(false);

  // control bar  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  UListbox& viewlist = ulistbox
  (UOrient::horizontal
   + UBackground::none
   + upadding(1,1)
   + uitem(utip("Main viewpoint first person")
           + g.theme.BigEye
           + ucall(int(UserAction::UA_FIRSTVIEW), Widgets::callAction)
          )
   + uitem(utip("Third person viewpoints")
           + g.theme.Eyes
           + ucall(int(UserAction::UA_THIRDVIEWFAR), Widgets::callAction)
          )
   + uitem(utip("Alternate viewpoints")
           + g.theme.Jumelles
           + ucall(int(UserAction::UA_SWITCHVIEW), Widgets::callAction)
          )
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
       + uitem(utip("Show axis")
               + g.theme.Axis
               + ucall(_gw, toggleAxisCB)
              )
       + uitem(utip("Show 2D grid")
               + g.theme.Grid2D
               + ucall(_gw, 0, toggleGridCB)
              )
       + uitem(utip("Show 3D grid")
               + g.theme.Grid3D
               + ucall(_gw, 1, toggleGridCB)
              )
       + uitem(utip("Show hud")
               + ucall(_gw, toggleHudCB)
               + g.theme.Counter
              )
       + uitem(utip("Show map")
               + g.theme.World
               + ucall(int(UserAction::UA_MAPVIEW), Widgets::callAction)
              )
       + usepar()
       + "Objects:"
       + uitem(utip("Show object cart")
               // + ushow(*gw.cartDialog, true)         !! A COMPLETER
               + g.theme.Cart)
       + uitem(utip("Add object to the world")
               + g.theme.AddObj
               + ushow(gw.addobj_dialog, true)
              )
       + uitem(utip("Test: sandbox world !")
               + UPix::question
               + ucall(_gw, sandboxCB)
              )
       + usepar()
       + "Throw:"
       + uitem(utip("Throw dart")
               + UPix::ray
               + ucall(int(UserAction::UA_DART), Widgets::callAction)
              )
       + uitem(utip("Throw bullet")
               + UFont::bold + UFont::xx_large + UColor::red + "."
               + ucall(int(UserAction::UA_BULLET), Widgets::callAction)
              )
       + usepar()
       + "Snap:"
       + uitem(utip("Capture screenshot in JPG")
               + g.theme.Camera
               + ucall(&gw.capture, &Capture::writeJPGImage)
              )
       + uitem(utip("Capture screenshot in PNG")
               + g.theme.Camera
               + ucall(&gw.capture, &Capture::writePNGImage)
              )
#if WANT_GL2PS
       + uitem(utip("Capture screenshot in SVG")
               + g.theme.Camera
               + ucall(&gw.capture, &Capture::writeSVGImage)
              )
#endif
       + uitem(utip("Capture video (start/stop)")
               + UOn::select   / ucall(&gw.capture, &Capture::startVideo)
               + UOn::deselect / ucall(&gw.capture, &Capture::stopVideo)
               + UOn::select   / g.theme.Exit
               + UOn::deselect / g.theme.Movie
              )

       + uhflex() + ubox() + uright() + expand_collapse
      );

  // main panel - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  main_panel.addAttr(UOrient::vertical).add(control_bar + control_panel);
}

void Panels::showCartDialog(bool state)
{
#if 0 //dax
  // relies on carts_pane created by createPanels()
  UDialog * cartDialog = &udialog(utitle("Basket")
                                  + uheight(150) + uwidth(100)
                                  + carts_pane
                                  + ubottom()
                                  + ubutton(UFont::bold + uhcenter()
                                  + " Close "
                                  + ucloseWin())
                                 );
#else
  cerr << "TO BE DONE!" << endl;
#endif
}

