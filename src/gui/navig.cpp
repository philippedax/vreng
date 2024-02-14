//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://www.vreng.enst.fr/
//
// Copyright (C) 1997-2021 Philippe Dax
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
#include "navig.hpp"
#include "widgets.hpp"	// gw
#include "palette.hpp"	// Palette
#include "scene.hpp"	// scene.add
#include "motion.hpp"	// Motion
#include "message.hpp"	// message
#include "theme.hpp"	// theme
#include "user.hpp"	// User
#include "vnc.hpp"	// Vnc
#include "carrier.hpp"	// Carrier
#include "board.hpp"	// Board


// local
static ObjInfo objinfo[ACTIONSNUMBER + 6];
static Motion *motionx = null, *motiony = null;


/* Constructor */
Navig::Navig(Widgets* _gw, Scene& scene) :
 gw(*_gw),
 xref(0),
 yref(0),
 followMouse(false),
 depthsel(0),
 opened_menu(null) 
{  
  object_infos.addAttr(UBackground::green + UColor::white);
  object_infos.add(  uelem(UFont::bold + " "           + object_class + " ") 
                   + uelem(UFont::bold + UFont::italic + object_name + " ")
                  );
  object_menu.addAttr(ualpha(0.5) + UBackground::black);
  object_menu.add(object_infos);
  object_menu.softwin();
  
  initNavigMenu();
  
  scene.add(  object_menu
            + navig_menu
            + UOn::mpress   / ucall(this, &Navig::mousePressCB)
            + UOn::mrelease / ucall(this, &Navig::mouseReleaseCB)
            + UOn::mdrag    / ucall(this, &Navig::mouseDragCB)
            + UOn::mmove    / ucall(this, &Navig::mouseMoveCB)
            + UOn::kpress   / ucall(this, &Navig::keyPressCB)
            + UOn::krelease / ucall(this, &Navig::keyReleaseCB)
           );
}

//////////////////
// Scene callbacks
//////////////////

// the mouse is pressed on the canvas
void Navig::mousePressCB(UMouseEvent& e)
{
  int x = (int) e.getX(), y = (int) e.getY();
  int btn = 0;
  int bid = e.getButton();

  if      (bid == UMouseEvent::LeftButton)  btn = 1;
  else if (bid == UMouseEvent::MidButton)   btn = 2;
  else if (bid == UMouseEvent::RightButton) btn = 3;

  // GL graphics can be performed until the current function returns
  GLSection gls(&gw.scene); 

  if (gw.gui.carrier && gw.gui.carrier->underControl()) {	// events are sent to Carrier
    gw.gui.carrier->mouseEvent(x, y, btn);
  }
#if 0 //dax
  else if (gw.gui.board && gw.gui.board->isDrawing()) {	// events are sent to Board
    gw.gui.board->mouseEvent(x, y, btn);
  }
#endif
  else if (gw.gui.vnc) {				// events are sent to Vnc
    gw.gui.vnc->mouseEvent(x, y, btn);
  }
  else if (btn == 2) {			// button 2: fine grain selection
    mousePressB2(e, x, y);
    //dax mousePressB1orB3(e, x, y, btn);	// dax reverse buttons B1 B2
  }
  else {				// buttons 1 or 3: navigator or info menu
    mousePressB1orB3(e, x, y, btn);
    //dax mousePressB2(e, x, y);		// dax reverse buttons B1 B2
  }
}

// the mouse is released on the canvas
void Navig::mouseReleaseCB(UMouseEvent& e)
{
  stopMotion(); // sanity
  
  if (gw.gui.vnc) {		// events are redirected to Vnc
    gw.gui.vnc->mouseEvent((int) e.getX(), (int) e.getY(), 0);
  }
  else if (gw.gui.selected_object) {
    gw.gui.selected_object->resetRay();
  }
  if (localuser) {
    localuser->resetRay();	//stop showing direction
  }
  opened_menu = null;
}

// the mouse is dragged on the canvas
void Navig::mouseDragCB(UMouseEvent& e)
{
  if (gw.gui.selected_object && gw.gui.selected_object->isValid()) {
    gw.gui.selected_object->resetFlashy();	// stop flashing edges
    gw.gui.selected_object->resetRay();
  }
  else {
    if (localuser) {
      localuser->resetRay();	// stop showing direction
    }
  }
}

// the mouse is moved on the canvas
void Navig::mouseMoveCB(UMouseEvent& e)
{
  if (gw.gui.vnc) {		// events are redirected to Vnc
    gw.gui.vnc->mouseEvent((int) e.getX(), (int) e.getY(), 0);
  }
  else if (followMouse) {
    // mode followMouse continuously indicates object under pointer
    WO *object = gw.pointedObject((int) e.getX(), (int) e.getY(), objinfo, depthsel);
    selectObject(object ? objinfo : null);
  }
  else if (gw.gui.selected_object && gw.gui.selected_object->isValid()) {
    gw.gui.selected_object->resetFlashy();	// stop flashing edges
    gw.gui.selected_object->resetRay();
  }
}

// a key is pressed on the canvas
void Navig::keyPressCB(UKeyEvent& e)
{
  if (gw.gui.vnc) {	// key events are redirected to Vnc
    char kstr[2];
    kstr[0] = e.getKeyChar();
    kstr[1] = 0;
    gw.gui.vnc->keyEvent(kstr, true);
  }
  else {		// normal behaviour
    gw.processKey(e.getKeyCode(), e.getKeyChar(), true);

    if (! localuser)  return;

    struct timeval t;
    gettimeofday(&t, NULL);
    
    float user_lspeed = User::LSPEED;	// reset lspeed
    localuser->specialAction(User::UA_SETLSPEED, &user_lspeed, t.tv_sec, t.tv_usec);
    float user_aspeed = User::ASPEED;	// reset aspeed
    localuser->specialAction(User::UA_SETASPEED, &user_aspeed, t.tv_sec, t.tv_usec);
  }
}

// a key is released on the canvas
void Navig::keyReleaseCB(UKeyEvent& e)
{
  if (gw.gui.vnc) {     // key events are redirected to Vnc
    char kstr[2];
    kstr[0] = e.getKeyChar();
    kstr[1] = 0;
    gw.gui.vnc->keyEvent(kstr, false);
  }
  else {		// normal behaviour
    gw.processKey(e.getKeyCode(), e.getKeyChar(), false);
  }
}

// Press Buttons 1 or 3: navigator or info menu
void Navig::mousePressB1orB3(UMouseEvent& e, int x, int y, int btn)
{
  //Sound::playSound(CLICKSND);

  // desactivate previous object
  WO* prev_object = gw.gui.getSelectedObject();
  if (prev_object) {
    prev_object->resetFlashy();
    prev_object->resetRay();
  }

  // current clic
  static uint8_t z = 0;	// first object static to allow 3 objects
  //echo("z: %d", z);
  WO* object = gw.pointedObject(x, y, objinfo, z % 3);
  z++;			// next object hidden in th the z buffer

  if (object) {
    gw.gui.selected_object = object;
    //echo("clic [%d %d] on %s", x, y, object->objectName());
  
    if (object->names.url[0]) {
      selected_object_url = object->names.url;
    }
    else {
      selected_object_url.clear();
    }
    gw.message.performRequest(object);
    // Vrelet: calculate the clic vector and do the clic method on the object
    if (btn == 1) {
      if (gw.gui.vrelet) object->click(x, y);
      if (gw.gui.board)  object->click(x, y);
    }

selectObject(objinfo);
  
    if (btn == 3) {		// show object menu
      object_menu.open(e);
      opened_menu = object_menu;
    }
    else if (btn == 1) {	// navigator
      navig_menu.open(e);	// show(e, 0, 0); TRASH !!!
      opened_menu = navig_menu;
      object->setFlashy();	// flashes the edges of the solid
      object->setRay(x, y);	// launches stipple ray on the object
    }
  }
  else {	// no object!
    gw.setRayDirection(x, y);	// launches ray on x,y screen coord
  }
}

// Press button 2: fine grain selection
void Navig::mousePressB2(UMouseEvent&, int x, int y)
{
  depthsel++;
  WO* object = gw.pointedObject(x, y, objinfo, depthsel);
  if (object) {
    gw.gui.selected_object = object;
    object->resetFlashy();
    object->setFlashy();	// flashes edges of the solid
    object->setRay(x, y);	// launches stipple ray on the object

    selectObject(objinfo);
  }  
  else {	// no object!
    gw.setRayDirection(x, y);	// launches ray on x,y screen coord
  }
}

// Clears InfoBar
void Navig::clearInfoBar()
{
  gw.infos_box.removeAll();	// clears infos in infobar
}

// Updates object infos (infosbox in the infobar and in the contextual menu)
void Navig::selectObject(ObjInfo* objinfo)
{
  if (! objinfo)  return;

  gw.infos_box.removeAll();	// clears infos in infobar
  object_menu.removeAll();	// clears contextual menu
  
  // adds object class and name to the infosbox
  object_class = objinfo[0].name;
  object_name  = objinfo[1].name;
  gw.infos_box.add(object_infos);
  gw.infos_box.addAttr(UColor::navy + UFont::bold);

  // adds object class and name to the contextual menu
  object_menu.add(object_infos);
  object_menu.add(UColor::white);

  // adds buttons (actions)
  for (ObjInfo* oi = objinfo + 2; oi->name ; oi++) {
    UBox& b = ubutton(oi->name);
    if (oi->fun && strlen(oi->name)) {
      b.addAttr(UBackground::white);
      b.add(ucall(oi->farg, oi->fun));	// add action button to infosbox
    }
    gw.infos_box.add(b);
    object_menu.add(b);			// add action button to contextual menu
  }
  gw.infos_box.addAttr(UBackground::none);
}

/////////
// MOTION 
/////////

void Navig::userMotion(UMouseEvent& e, Motion* _motionx, Motion *_motiony)
{
  xref = e.getX();
  yref = e.getY();
  if (motionx)  motionx->stop();
  if (motiony)  motiony->stop();
  motionx = _motionx;
  motiony = _motiony;
}

void Navig::objectMove(UMouseEvent& e, Motion* _motionx, Motion *_motiony)
{
  xref = e.getX();
  yref = e.getY();
  //echo("xyref: %.0f %.0f", xref,yref);
  if (motionx)  motionx->stop();
  if (motiony)  motiony->stop();
  motionx = _motionx;
  motiony = _motiony;
}

void Navig::doMotion(UMouseEvent& e)
{
  float dx = e.getX() - xref;
  float dy = e.getY() - yref;
  if (motionx)  motionx->move((int) dx);
  if (motiony)  motiony->move((int) dy);
}

void Navig::stopMotion()
{
  if (motionx)  motionx->stop();
  if (motiony)  motiony->stop();
  motionx = null;
  motiony = null;
}

////////////
// NAVIGATOR
////////////

void Navig::initNavigMenu()
{
  navig_menu.addAttr(UBackground::black);
  // X translation on x-mouse
  UCall& XMotion = ucall(this, &Motion::u_trans_x, (Motion*)0, &Navig::userMotion);
  // Z rotation on x-mouse and Y translation on y-mouse
  UCall& YMotion = ucall(this, &Motion::u_rot_z, &Motion::u_trans_y, &Navig::userMotion);
  // Z translation on y-mouse
  UCall& ZMotion = ucall(this, (Motion*)0, &Motion::u_trans_z, &Navig::userMotion);
  UCall& move = ucall(this, &Navig::doMotion);
  UCall& stop = ucall(this, &Navig::stopMotion);

  // RIGHT
  navig_menu.item(0).add(g.theme.BigRight
                         + UOn::arm / YMotion
                         + UOn::mdrag / move
                         + UOn::disarm / stop
                        );
  // FORWARD
  navig_menu.item(1).add(g.theme.Forward
                         + UOn::arm / YMotion
                         + UOn::mdrag / move
                         + UOn::disarm / stop
                        );
  // VERTICAL
  navig_menu.item(2).add(g.theme.UpDown
                         + UOn::arm / ZMotion
                         + UOn::mdrag / move
                         + UOn::disarm / stop
                        );
  // LEFT
  navig_menu.item(3).add(g.theme.BigLeft
                         + UOn::arm / YMotion
                         + UOn::mdrag / move
                         + UOn::disarm / stop
                        );
  // HORIZONTAL
  navig_menu.item(4).add(g.theme.LeftTrans
                         + UOn::arm / XMotion
                         + UOn::mdrag / move
                         + UOn::disarm / stop
                        );
  // BACKWARD
  navig_menu.item(5).add(g.theme.Backward
                         + UOn::arm / YMotion
                         + UOn::mdrag / move
                         + UOn::disarm / stop
                        );
  // HORIZONTAL
  navig_menu.item(6).add(g.theme.RightTrans
                         + UOn::arm / XMotion
                         + UOn::mdrag / move
                         + UOn::disarm / stop
                        );
}

///////////////////
// MANIPULATOR
//
// called by panels
//
///////////////////

UBox& Navig::manipulator()    // !!!!!!! TO REVIEW !!!!!!!!!
{
  UPadding& pad = upadding(4,4);
  UVspacing &vs = uvspacing(4);
  UHspacing &hs = uhspacing(4);

  UArgs l = uhcenter() + uvcenter()
            + UOn::enter / UBackground::orange
            + UOn::arm   / UBackground::green
            + utip("Drag mouse to manipulate");

  UTrow& row1 = utrow
  (pad + vs + hs
   + uitem(l + g.theme.Transx	 // FORWARD/BACKWARD
           + UOn::mpress   / ucall(this, &Navig::mouseRefCB)
           + UOn::mdrag    / ucall(this, &Navig::mouseDragCB)
           + UOn::mrelease / ucall(this, &Navig::mouseReleaseCB)
           + UOn::arm      / ucall(this, (Motion*)0, &Motion::o_trans_f, &Navig::objectMove)
	  )
   + uitem(l + " ")
   + uitem(l + g.theme.Rotzleft	 // TURN LEFT
           + UOn::mpress   / ucall(this, &Navig::mouseRefCB)
           + UOn::mdrag    / ucall(this, &Navig::mouseDragCB)
           + UOn::mrelease / ucall(this, &Navig::mouseReleaseCB)
           + UOn::arm      / ucall(this, (Motion*)0, &Motion::o_rot_l_z, &Navig::objectMove)
          )
   + uitem(l + " ")
   + uitem(l + g.theme.Rotzright // TURN RIGHT
           + UOn::mpress   / ucall(this, &Navig::mouseRefCB)
           + UOn::mdrag    / ucall(this, &Navig::mouseDragCB)
           + UOn::mrelease / ucall(this, &Navig::mouseReleaseCB)
           + UOn::arm      / ucall(this, (Motion*)0, &Motion::o_rot_r_z, &Navig::objectMove)
          )
  );
  UTrow& row2 = utrow
  (pad + vs + hs
   + uitem(l + g.theme.Transy	 // LEFT/RIGHT
           + UOn::mpress   / ucall(this, &Navig::mouseRefCB)
           + UOn::mdrag    / ucall(this, &Navig::mouseDragCB)
           + UOn::mrelease / ucall(this, &Navig::mouseReleaseCB)
           + UOn::arm      / ucall(this, (Motion*)0, &Motion::o_trans_l, &Navig::objectMove)
          )
   + uitem(l + " ")
   + uitem(l+ g.theme.Rotxleft	 // ROLL LEFT
           + UOn::mpress   / ucall(this, &Navig::mouseRefCB)
           + UOn::mdrag    / ucall(this, &Navig::mouseDragCB)
           + UOn::mrelease / ucall(this, &Navig::mouseReleaseCB)
           + UOn::arm      / ucall(this, (Motion*)0, &Motion::o_rot_l_z, &Navig::objectMove)
          )
   + uitem(l + " ")
   + uitem(l + g.theme.Rotxright // ROLL RIGHT
           + UOn::mpress   / ucall(this, &Navig::mouseRefCB)
           + UOn::mdrag    / ucall(this, &Navig::mouseDragCB)
           + UOn::mrelease / ucall(this, &Navig::mouseReleaseCB)
           + UOn::arm      / ucall(this, (Motion*)0, &Motion::o_rot_r_z, &Navig::objectMove)
          )
  );
  UTrow& row3 = utrow
  (pad + vs + hs
   + uitem(l + g.theme.Transz	 // UP/DOWN
           + UOn::mpress   / ucall(this, &Navig::mouseRefCB)
           + UOn::mdrag    / ucall(this, &Navig::mouseDragCB)
           + UOn::mrelease / ucall(this, &Navig::mouseReleaseCB)
           + UOn::arm      / ucall(this, (Motion*)0, &Motion::o_trans_u, &Navig::objectMove)
          )
   + uitem(l + " ")
   + uitem(l + g.theme.Rotyup	 // TILT UP
           + UOn::mpress   / ucall(this, &Navig::mouseRefCB)
           + UOn::mdrag    / ucall(this, &Navig::mouseDragCB)
           + UOn::mrelease / ucall(this, &Navig::mouseReleaseCB)
           + UOn::arm      / ucall(this, (Motion*)0, &Motion::o_rot_u_y, &Navig::objectMove)
          )
   + uitem(l + " ")
   + uitem(l + g.theme.Rotydown	 // TILT DOWN
           + UOn::mpress   / ucall(this, &Navig::mouseRefCB)
           + UOn::mdrag    / ucall(this, &Navig::mouseDragCB)
           + UOn::mrelease / ucall(this, &Navig::mouseReleaseCB)
           + UOn::arm      / ucall(this, (Motion*)0, &Motion::o_rot_d_y, &Navig::objectMove)
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

void Navig::mouseRefCB(UMouseEvent& e)
{ 
}
