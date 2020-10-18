//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2011 Philippe Dax
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
 *  navig.cpp : Ubit Navigator for the VREng GUI
 *
 *  VREng / Ubit Project
 *  Author: Eric Lecolinet
 *
 *  Ubit Toolkit: A Brick Construction Game Model for Creating GUIs
 *  Please refer to the Ubit GUI Toolkit Home Page for details.
 *
 *  (C) 2002-2008 Eric Lecolinet @ ENST Paris
 *  WWW: http://www.enst.fr/~elc/ubit
 */

#include "vreng.hpp"
#include "widgets.hpp"
#include "scene.hpp"	// add
#include "navig.hpp"
#include "message.hpp"
#include "mvt.hpp"
#include "theme.hpp"
#include "user.hpp"	// User
#include "vnc.hpp"	// Vnc
#include "carrier.hpp"	// Carrier
#include "board.hpp"	// Board
#include "sound.hpp"	// playSound

// local
static ObjInfo objinfo[ACTIONSNUMBER + 6];
static Mvt *mvtx = null, *mvty = null;


Navig::Navig(Widgets* _gw, Scene& scene) :
gw(*_gw), xref(0), yref(0), followMouseMode(false), depthsel(0), opened_menu(null) 
{  
  object_infos.addAttr(UBackground::green + UColor::white);
  object_infos.add(uelem(UFont::bold + " " + object_class + " ") 
                   + uelem(UFont::bold + UFont::italic + object_name + " ")
                   + " ");
  object_menu.addAttr(ualpha(0.5) + UBackground::black);
  object_menu.add(object_infos);
  object_menu.softwin();
  
  initNavigMenu();
  
  scene.add(object_menu
            + navig_menu
            + UOn::mpress   / ucall(this, &Navig::canvasMousePressCB)
            + UOn::mrelease / ucall(this, &Navig::canvasMouseReleaseCB)
            + UOn::mdrag    / ucall(this, &Navig::canvasMouseDragCB)
            + UOn::mmove    / ucall(this, &Navig::canvasMouseMoveCB)
            + UOn::kpress   / ucall(this, &Navig::canvasKeyPressCB)
            + UOn::krelease / ucall(this, &Navig::canvasKeyReleaseCB)
            );
}

// Scene callbacks

// the mouse is pressed on the canvas
void Navig::canvasMousePressCB(UMouseEvent& e)
{
  int x = (int) e.getX(), y = (int) e.getY();
  int button = 0;
  int bid = e.getButton();
  if      (bid == UMouseEvent::LeftButton)  button = 1;
  else if (bid == UMouseEvent::MidButton)   button = 2;
  else if (bid == UMouseEvent::RightButton) button = 3;

  // GL graphics can be performed until the current function returns
  GLSection gls(&gw.scene); 
  //EX: scene->makeCurrent();

  if (gw.gui.carrier && gw.gui.carrier->isTaking()) // events are sent to Carrier
    gw.gui.carrier->mouseEvent(x, y, button);
#if 0 //dax
  else if (gw.gui.board && gw.gui.board->isDrawing()) // events are sent to Board
    gw.gui.board->mouseEvent(x, y, button);
#endif
  else if (gw.gui.vnc)		// events are sent to Vnc
    gw.gui.vnc->mouseEvent(x, y, button);
  else if (button == 2)		// button 2: fine grain selection
    mousePressB2(e, x, y, button);
  else 				// buttons 1 or 3: navigator or info menu
    mousePressB1orB3(e, x, y, button);
}

// the mouse is released on the canvas
void Navig::canvasMouseReleaseCB(UMouseEvent& e)
{
  stopMotion(); // securite
  
  //navig_menu.show(false); inutile et ferme (bizarrement) object_menu
  opened_menu = null;
  
  if (gw.gui.vnc)	// events are redirected to Vnc
    gw.gui.vnc->mouseEvent((int) e.getX(), (int) e.getY(), 0/*button*/);
  else if (gw.gui.selected_object)
    gw.gui.selected_object->resetRay();
  if (localuser) localuser->resetRay(); //stop showing direction
}

// the mouse is dragged on the canvas
void Navig::canvasMouseDragCB(UMouseEvent& e)
{
  if (gw.gui.selected_object && gw.gui.selected_object->isValid()) {
    gw.gui.selected_object->resetFlashy();	// stop flashing edges
    gw.gui.selected_object->resetRay();
  }
  else {
    if (localuser) localuser->resetRay();	//stop showing direction
  }
}

// the mouse is moved on the canvas
void Navig::canvasMouseMoveCB(UMouseEvent& e)
{
  float x = e.getX(), y = e.getY();
  
  if (gw.gui.vnc)		// events are redirected to Vnc
    gw.gui.vnc->mouseEvent((int) x, (int) y, 0/*button*/);
  else if (/*button == 0 &&*/ followMouseMode) {
    // mode followMouse continuously indicates object under pointer
    WObject *object = gw.getPointedObject((int) x, (int) y, objinfo, depthsel);
    selectObject((object ? objinfo : null), 0);
  }
  else if (gw.gui.selected_object && gw.gui.selected_object->isValid()) {
    gw.gui.selected_object->resetFlashy();	// stop flashing edges
    gw.gui.selected_object->resetRay();
  }
}

// a key is pressed on the canvas
void Navig::canvasKeyPressCB(UKeyEvent& e)
{
  if (gw.gui.vnc) {	// key events are redirected to Vnc
    char kstr[2];
    kstr[0] = e.getKeyChar();
    kstr[1] = 0;
    gw.gui.vnc->keyEvent(kstr, true);
  }
  else {  // normal behaviour
    if (! localuser)  return;
    struct timeval t;
    gettimeofday(&t, NULL);
    
    float user_lspeed = User::LSPEED;	// reset lspeed
    localuser->specialAction(UserAction::UA_SETLSPEED, &user_lspeed, t.tv_sec, t.tv_usec);
    float user_aspeed = User::ASPEED;	// reset aspeed
    localuser->specialAction(UserAction::UA_SETASPEED, &user_aspeed, t.tv_sec, t.tv_usec);
    gw.processKey(e.getKeyCode(), e.getKeyChar(), true);
  }
}

// a key is released on the canvas
void Navig::canvasKeyReleaseCB(UKeyEvent& e)
{
  if (gw.gui.vnc) {     // key events are redirected to Vnc
    char kstr[2];
    kstr[0] = e.getKeyChar();
    kstr[1] = 0;
    gw.gui.vnc->keyEvent(kstr, false);
  }
  else			// normal behaviour
    gw.processKey(e.getKeyCode(), e.getKeyChar(), false);
}

// PRESS buttons 1 or 3: navigator or info menu
void Navig::mousePressB1orB3(UMouseEvent& e, int x, int y, int button)
{
  // sound effect
  //Sound::playSound(CLICKSND);
  // desactivate previous object
  WObject* prev_object = gw.gui.getSelectedObject();
  if (prev_object && prev_object->isValid()) {
    prev_object->resetFlashy();
    prev_object->resetRay();
  }
  // current object
  depthsel = 0;
  WObject* object = gw.getPointedObject(x, y, objinfo, depthsel);
  if (object && object->isValid()) {
    gw.gui.selected_object = object;
    trace(DBG_GUI, "clic [%d %d] on %s", x, y, object->getInstance());
  
    if (object->names.url[0]) selected_object_url = object->names.url;
    else                      selected_object_url.clear();
    gw.messages.performRequest(object);
    // Vrelet: calculate the clic vector and do the clic method on the object
    if (gw.gui.vrelet && button == 1) object->click(x, y);
    if (gw.gui.board  && button == 1) object->click(x, y);

    selectObject(objinfo, button);
  
    if (button == 3) {		// show object menu
      object_menu.open(e);
      opened_menu = object_menu;
    }
    else if (button == 1) {	// navigator
      navig_menu.open(e);	// show(e, 0, 0);
      opened_menu = navig_menu;
      if (object->solid) {
        object->setFlashy();	// flashes the edges of the solid
        object->setRay(x, y);	// Launches ray
      }
    }
  }
  else
    gw.setRayDirection(x, y);	// Launches stipple ray
}

// PRESS button 2: fine grain selection
void Navig::mousePressB2(UMouseEvent&, int x, int y, int button)
{
  depthsel++;
  WObject* object = gw.getPointedObject(x, y, objinfo, depthsel);
  if (object && object->isValid() && object->solid) {
    gw.gui.selected_object = object;
    object->resetFlashy();
    object->setFlashy();		// flashes edges of the solid
    object->setRay(x, y);		// Launches ray
    selectObject(objinfo, button);
  }  
  else gw.setRayDirection(x, y);	// Launches stipple ray
}

// MOTION 

void Navig::startMotion(UMouseEvent& e, Mvt* _mvtx, Mvt *_mvty)
{
  xref = e.getX();
  yref = e.getY();
  if (mvtx)  mvtx->stop();
  if (mvty)  mvty->stop();
  mvtx = _mvtx;
  mvty = _mvty;
}

void Navig::doMotion(UMouseEvent& e)
{
  float deltax = e.getX() - xref;
  float deltay = e.getY() - yref;
  if (mvtx)  mvtx->move((int) deltax);
  if (mvty)  mvty->move((int) deltay);
}

void Navig::stopMotion()
{
  if (mvtx)  mvtx->stop();
  if (mvty)  mvty->stop();
  mvtx = null;
  mvty = null;
}

// Updates object info (infoBox in the infoBar and contextual info menu)
void Navig::selectObject(ObjInfo* objinfo, int btn)
{
  gw.infos.removeAll();
  object_menu.removeAll();
  object_menu.add(object_infos);
  object_menu.add(UColor::white);

  if (!objinfo)  return;
  
  object_class = objinfo[0].name;
  object_name  = objinfo[1].name;
  
  // add object class and name to the infos box
  gw.infos.add(object_infos);
  gw.infos.add(UColor::navy);
  gw.infos.add(UBackground::green);
  // add buttons to the infos box and the object menu
  for (ObjInfo* poi = objinfo + 2; poi->name != null; poi++) {
    // add button to infobox
    UBox& b = ubutton(poi->name);
    if (poi->fun) b.add(ucall(poi->farg, poi->fun));
    gw.infos.add(b);
    // add button to object menu
    object_menu.add(b);
  }
}

// =========
// NAVIGATOR
void Navig::initNavigMenu()
{
  //navig_menu.setShowDelay(300);  // delay of the expert mode
  navig_menu.addAttr(UBackground::black);
  // Z rotation on x-mouse and Y translation on y-mouse
  UCall& startYZMotion = ucall(this, &Mvt::zrot, &Mvt::ytrans, &Navig::startMotion);
  // X translation on x-mouse
  UCall& startXTranslation = ucall(this, &Mvt::xtrans, (Mvt*)0, &Navig::startMotion);
  // Z translation on y-mouse
  UCall& startZTranslation = ucall(this, (Mvt*)0, &Mvt::ztrans, &Navig::startMotion);
  UCall& move = ucall(this, &Navig::doMotion);
  UCall& stop = ucall(this, &Navig::stopMotion);
  // RIGHT
  navig_menu.item(0).add(g.theme.Right
                         + UOn::arm / startYZMotion
                         + UOn::mdrag / move
                         + UOn::disarm / stop
                         );
  // FORWARD
  navig_menu.item(2).add(g.theme.Forward
                         + UOn::arm / startYZMotion
                         + UOn::mdrag / move
                         + UOn::disarm / stop
                         );
  // VERTICAL TRANSLATION
  navig_menu.item(3).add(g.theme.UpDown
                         + UOn::arm / startZTranslation
                         + UOn::mdrag / move
                         + UOn::disarm / stop
                         );
  // LEFT
  navig_menu.item(4).add(g.theme.Left
                         + UOn::arm / startYZMotion
                         + UOn::mdrag / move
                         + UOn::disarm / stop
                         );
  // HORIZONTAL TRANSLATION
  navig_menu.item(5).add(g.theme.LeftTrans
                         + UOn::arm / startXTranslation
                         + UOn::mdrag / move
                         + UOn::disarm / stop
                         );
  // BACKWARD
  navig_menu.item(6).add(g.theme.Backward
                         + UOn::arm / startYZMotion
                         + UOn::mdrag / move
                         + UOn::disarm / stop
                         );
  // HORIZONTAL TRANSLATION
  navig_menu.item(7).add(g.theme.RightTrans
                         + UOn::arm / startXTranslation
                         + UOn::mdrag / move
                         + UOn::disarm / stop
                         );
}

// MANIPULATOR
UBox& Navig::createManipulator()    // !!!!!!! A REVOIR !!!!!!!!!
{
  UPadding& pad = upadding(2,2);
  UVspacing &vs = uvspacing(2);
  UHspacing &hs = uhspacing(2);
  UArgs l = uhcenter() + uvcenter()
         + UOn::enter / UBackground::orange
         + UOn::arm   / UBackground::green
         + utip("Drag mouse to manipulate");

  UTrow& row1 = utrow
  (pad + vs + hs
   + uitem(l + g.theme.Transx	 //FORWARD/BACKWARD
           + UOn::mpress   / ucall(this, &Navig::setMouseRef)
           + UOn::mdrag    / ucall(this, &Navig::canvasMouseDragCB)
           + UOn::mrelease / ucall(this, &Navig::canvasMouseReleaseCB)
           + UOn::arm / ucall(this, (Mvt*)0, &Mvt::trans_forw, &Navig::startMotion)
	  )
   + uitem(l + " ")
   + uitem(l + g.theme.Rotzleft	 //TURN LEFT
           + UOn::mpress   / ucall(this, &Navig::setMouseRef)
           + UOn::mdrag    / ucall(this, &Navig::canvasMouseDragCB)
           + UOn::mrelease / ucall(this, &Navig::canvasMouseReleaseCB)
           + UOn::arm / ucall(this, (Mvt*)0, &Mvt::zrot_left, &Navig::startMotion)
          )
   + uitem(l + " ")
   + uitem(l + g.theme.Rotzright //TURN RIGHT
           + UOn::mpress   / ucall(this, &Navig::setMouseRef)
           + UOn::mdrag    / ucall(this, &Navig::canvasMouseDragCB)
           + UOn::mrelease / ucall(this, &Navig::canvasMouseReleaseCB)
           + UOn::arm / ucall(this, (Mvt*)0, &Mvt::zrot_right, &Navig::startMotion)
   )
  );

  UTrow& row2 = utrow
  (pad + vs + hs
   + uitem(l + g.theme.Transy	 //LEFT/RIGHT
           + UOn::mpress   / ucall(this, &Navig::setMouseRef)
           + UOn::mdrag    / ucall(this, &Navig::canvasMouseDragCB)
           + UOn::mrelease / ucall(this, &Navig::canvasMouseReleaseCB)
           + UOn::arm / ucall(this, (Mvt*)0, &Mvt::trans_left, &Navig::startMotion)
           )
   + uitem(l + " ")

   + uitem(l+ g.theme.Rotxleft	 //ROLL LEFT
           + UOn::mpress   / ucall(this, &Navig::setMouseRef)
           + UOn::mdrag    / ucall(this, &Navig::canvasMouseDragCB)
           + UOn::mrelease / ucall(this, &Navig::canvasMouseReleaseCB)
           + UOn::arm / ucall(this, (Mvt*)0, &Mvt::xrot_left, &Navig::startMotion)
           )
   + uitem(l + " ")
   + uitem(l + g.theme.Rotxright //ROLL RIGHT
           + UOn::mpress   / ucall(this, &Navig::setMouseRef)
           + UOn::mdrag    / ucall(this, &Navig::canvasMouseDragCB)
           + UOn::mrelease / ucall(this, &Navig::canvasMouseReleaseCB)
           + UOn::arm / ucall(this, (Mvt*)0, &Mvt::xrot_right, &Navig::startMotion)
           )
  );

  UTrow& row3 = utrow
  (pad + vs + hs
   + uitem(l + g.theme.Transz	 //UP/DOWN
           + UOn::mpress   / ucall(this, &Navig::setMouseRef)
           + UOn::mdrag    / ucall(this, &Navig::canvasMouseDragCB)
           + UOn::mrelease / ucall(this, &Navig::canvasMouseReleaseCB)
           + UOn::arm / ucall(this, (Mvt*)0, &Mvt::trans_up, &Navig::startMotion)
           )
   + uitem(l + " ")
   + uitem(l + g.theme.Rotyup	 //TILT UP
           + UOn::mpress   / ucall(this, &Navig::setMouseRef)
           + UOn::mdrag    / ucall(this, &Navig::canvasMouseDragCB)
           + UOn::mrelease / ucall(this, &Navig::canvasMouseReleaseCB)
           + UOn::arm / ucall(this, (Mvt*)0, &Mvt::yrot_up, &Navig::startMotion)
           )
   + uitem(l + " ")
   + uitem(l + g.theme.Rotydown	 //TILT DOWN
           + UOn::mpress   / ucall(this, &Navig::setMouseRef)
           + UOn::mdrag    / ucall(this, &Navig::canvasMouseDragCB)
           + UOn::mrelease / ucall(this, &Navig::canvasMouseReleaseCB)
           + UOn::arm / ucall(this, (Mvt*)0, &Mvt::yrot_down, &Navig::startMotion)
           )
   );
  
 return utable(pad + vs + hs
               + UBorder::none + UFont::bold
               + UBackground::none + UColor::white
               + row1
               + row2
               + row3
               );
}

void Navig::setMouseRef(UMouseEvent& e)
{ 
#if 0
  int bid = e.getButton();
  if      (bid == UMouseEvent::LeftButton)  button = 1;
  else if (bid == UMouseEvent::MidButton)   button = 2;
  else if (bid == UMouseEvent::RightButton) button = 3;
  else button = 0;
#endif
}
