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
#include "theme.hpp"	// g.theme
#include "palette.hpp"	// Palette
#include "navig.hpp"	// navig.manipulator
#include "scene.hpp"	// Scene
#include "joystick.hpp"	// Joystick1 Joystick2
#include "pref.hpp"	// ::g.pref.expand
#include "user.hpp"	// User
#include "world.hpp"	// World
#include "message.hpp"	// createMessagePanel
#include "channel.hpp"	// Channel
#include "capture.hpp"	// Capture
#include "axis.hpp"	// toggle
#include "grid.hpp"	// toggle


static void toggleHudCB(Widgets*)
{
  g.gui.scene()->toggleHud();
}

static void toggleAxisCB(Widgets*)
{
  Axis::axis()->toggle();
}

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

/** Constructor */
Panels::Panels(Widgets* _gw, Scene& scene) :
 gw(*_gw),
 joystick1(new Joystick1(_gw, (int) g.theme.controlPanelHeight/2 - 20)),
 joystick2(new Joystick2(_gw, 25)),
 manipulator(_gw->navig.manipulator())
{
  // right panel - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  // WORLDS
  UScrollpane& worlds_spane = uscrollpane(true, false, uvbox(gw.worlds));
  worlds_spane.showVScrollButtons(false);

  // BASKET
  UScrollpane& basket_spane = uscrollpane(true, false, uvbox(gw.basket));
  basket_spane.showVScrollButtons(false);

  // AVATARS
  UScrollpane& avatars_spane = uscrollpane(true, false, uvbox(gw.avatars));
  avatars_spane.showVScrollButtons(false);

  right_panel.addAttr(UOrient::vertical + uvspacing(8) + upadding(4,2))
             .add(uvbox(g.theme.panelStyle
                        + utop()
                        + ulabel(g.theme.World
                                 + " Worlds"
                                 + utip("Visited worlds")
                                )
                        + uvflex()
                        + ubox(g.theme.scrollpaneStyle
                               + worlds_spane
                              )
                       )
                + uvbox(g.theme.panelStyle
                        + utop()
                        + ulabel(g.theme.Folder
                                 + " Basket"
                                 + utip("Basket content")
                                )
                        + uvflex()
                        + ubox(g.theme.scrollpaneStyle
                               + basket_spane
                              )
                       )
                + uvbox(g.theme.panelStyle
                        + utop()
                        + ulabel(g.theme.Person
                                 + " Avatars"
                                 + utip("Current avatars")
                                )
                        + uvflex()
                        + ubox(g.theme.scrollpaneStyle
                               + avatars_spane
                              )
                       )
               );
    right_panel.show(false); // not shown by default

  // navigator - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  manipulator.show(false);  // joystick not shown by default
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

  Palette& navig_palette = *new Palette(g.theme.paletteStyle + navig_box);
  pnavig = &navig_palette;
  navig_palette.setPos(2|UPos::LEFT, 2|UPos::BOTTOM);
  navig_palette.setTitle(UColor::black + UFont::bold + "Navigator");
  scene.add(navig_palette);

  Palette& notif_palette = *new Palette(g.theme.paletteStyle
               //dax + usize(g.theme.messagePaletteWidth, g.theme.paletteHeight)
               + usize(g.theme.messagePaletteWidth, 20)
               //dax + gw.message.createMessagePanel(true)
               + gw.message.createQuery()
                                       );
  pnotif = &notif_palette;
  notif_palette.setPos(50|UPERCENT_CTR, 2|UPos::BOTTOM);
  notif_palette.setTitle(UColor::black + UFont::bold + "Notifications & Messages");
  scene.add(notif_palette);

  Palette& avatar_palette = *new Palette(g.theme.paletteStyle
               + usize(g.theme.paletteWidth/2, g.theme.paletteHeight/2)
               + avatars_spane);
  pavatar = &avatar_palette;
  avatar_palette.setPos(2|UPos::RIGHT, 2|UPos::BOTTOM);
  avatar_palette.setTitle(UColor::black + UFont::bold + "Avatars");
  scene.add(avatar_palette);

  if (::g.pref.expand == false) {
    navig_palette.collapse();
    //dax notif_palette.collapse();
    notif_palette.expand();
    avatar_palette.collapse();
  }

  // control panel - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  control_panel.add(  UOrient::horizontal
                    + usize().setHeight(g.theme.controlPanelHeight)
                    + UBackground::velin
                    + upadding(1,0) + uhspacing(10)
                    + uleft()
                    + navig_box
                    + " "
                    + uhflex()
                    + ubox(g.theme.panelStyle
                           + gw.message.createMessagePanel(false)
                           + gw.message.createQuery()
                          )
                   );
  control_panel.show(false);

  UBox& expand_collapse = ubutton(  utip("Expand/Collapse Panels")
                                  + UOn::select   / ustr("Collapse")
                                  + UOn::deselect / ustr("Expand")
                                  + UOn::select   / ushow(control_panel, true)
                                  + UOn::select   / ushow(right_panel, true)
                                  + UOn::deselect / ushow(control_panel, false)
                                  + UOn::deselect / ushow(right_panel, false)
                                  + UOn::select   / ushow(navig_palette, false)
                                  + UOn::select   / ushow(notif_palette, false)
                                  + UOn::select   / ushow(avatar_palette, false)
                                  + UOn::deselect / ushow(navig_palette, true)
                                  + UOn::deselect / ushow(notif_palette, true)
                                  + UOn::deselect / ushow(avatar_palette, true)
                                 );
  expand_collapse.setSelected(false);

  // control bar  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  UListbox& viewbar = ulistbox(  UOrient::horizontal
                               + UBackground::none
                               + upadding(1,1)
                               + uitem(utip("Main viewpoint first person")
                                       + g.theme.BigEye
                                       + ucall(int(User::UA_FIRSTVIEW), Widgets::callAction)
                                      )
                               + uitem(utip("Third person viewpoint")
                                       + g.theme.Eyes
                                       + ucall(int(User::UA_THIRDVIEWFAR), Widgets::callAction)
                                      )
                               + uitem(utip("Alternate viewpoints")
                                       + g.theme.Jumelles
                                       + ucall(int(User::UA_SWITCHVIEW), Widgets::callAction)
                                      )
                              );
  viewbar.select(0);

  control_bar.addAttr(UOrient::horizontal + uhcenter() + uhspacing(6))
             .add(  uhcenter()
                  + upadding(8,0).setBottom(14)
                  + "View:"
                  + viewbar
                  + usepar()
                  + " Show:"
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
                          + g.theme.Counter
                          + ucall(_gw, toggleHudCB)
                         )
                  + uitem(utip("Show map")
                          + g.theme.World
                          + ucall(int(User::UA_MAPVIEW), Widgets::callAction)
                         )
                  + usepar()

                  + " Objects:"
                  + uitem(utip("Show object cart")
                          + g.theme.Cart
                          //+ ushow(*gw.cartDialog, true)         !! A COMPLETER
                         )
                  + uitem(utip("Add object to the world")
                          + g.theme.AddObj
                          + ushow(gw.addobj_dialog, true)
                         )
                  + uitem(utip("Test: sandbox world !")
                          + g.theme.Sandbox //UPix::question
                          + ucall(_gw, sandboxCB)
                         )
                  + usepar()

                  + " Throw:"
                  + uitem(utip("Throw dart")
                          + UPix::ray
                          + ucall(int(User::UA_DART), Widgets::callAction)
                         )
                  + uitem(utip("Throw bullet")
                          + UFont::bold
                          + UFont::xx_large
                          + UColor::red
                          + "."
                          + ucall(int(User::UA_BULLET), Widgets::callAction)
                         )
                  + usepar()

                  + " Capture:"
                  + uitem(utip("Capture screenshot in JPG")
                          + g.theme.Camera
                          + ucall(&gw.capture, &Capture::writeJPGImage)
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
                  + usepar()
                  + uhflex()
                  + ubox()
                  + uright()
                  + expand_collapse
                 );

  main_panel.addAttr(UOrient::vertical).add(control_bar + control_panel);
}

void Panels::showCartDialog(bool state)
{
  UScrollpane& basket_spane = uscrollpane(true, false, uvbox(gw.basket));
  basket_spane.showVScrollButtons(false);

  // relies on basket_spane created by createPanels()
  UDialog * cartDialog = &udialog(utitle("Basket")
                                  + uscrollpane(usize(150, 100))
                                  + basket_spane
                                  + ubottom()
                                  + ubutton(UFont::bold + uhcenter()
                                  + " Close "
                                  + ucloseWin())
                                 );
}

void Panels::expandNavig()
{
  pnavig->expand();
}

void Panels::collapseNavig()
{
  pnavig->collapse();
}

void Panels::expandNotif()
{
  pnotif->expand();
}

void Panels::collapseNotif()
{
  pnotif->collapse();
}

void Panels::expandAvatar()
{
  pavatar->expand();
}

void Panels::collapseAvatar()
{
  pavatar->collapse();
}
