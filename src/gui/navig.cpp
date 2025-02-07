//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
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
//  navig.cpp : Ubit Navigator for the VREng GUI
//
//  VREng / Ubit Project
//  Author: Eric Lecolinet
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "navig.hpp"
#include "widgets.hpp"	// gw
#include "scene.hpp"	// scene.add
#include "motion.hpp"	// Motion
#include "message.hpp"	// message
#include "theme.hpp"	// theme
#include "user.hpp"	// User
#include "vnc.hpp"	// Vnc
#include "carrier.hpp"	// Carrier


// local
static ObjInfo objinfo[ACTIONSNUMBER + 6];
static Motion *motx = null, *moty = null;


/** Constructor */
Navig::Navig(Widgets* _gw, Scene& scene) :
 gw(*_gw),
 xref(0),
 yref(0),
 depthsel(0),
 opened_menu(null) 
{  
  object_infos.addAttr(UBackground::green + UColor::white);
  object_infos.add(  uelem(  UFont::bold
                           + " "
                           + object_class
                           + " "
                          ) 
                   + uelem(  UFont::bold
                           + UFont::italic
                           + object_name
                           + " "
                          )
                  );
  object_menu.addAttr(ualpha(0.5) + UBackground::black);
  object_menu.add(object_infos);
  object_menu.softwin();
  
  navigator();
  
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

/** The mouse is pressed on the canvas */
void Navig::mousePressCB(UMouseEvent& mev)
{
  int x = int(mev.getX());
  int y = int(mev.getY());
  int b = 0;

  int bid = mev.getButton();
  if      (bid == UMouseEvent::LeftButton)  b = 1;
  else if (bid == UMouseEvent::MidButton)   b = 2;
  else if (bid == UMouseEvent::RightButton) b = 3;

  // GL graphics can be performed until the current function returns
  GLSection gls(&gw.scene); 

  if (gw.gui.carrier && gw.gui.carrier->underControl()) {	// events are sent to Carrier
    gw.gui.carrier->mouseEvent(x, y, b);
  }
  else if (gw.gui.vnc) {		// events are sent to Vnc
    gw.gui.vnc->mouseEvent(x, y, b);
  }
  else if (b == 2) {			// button 2: fine grain selection
    pressB2(x, y);
  }
  else {				// buttons 1 or 3: navigator or info menu
    pressB1orB3(mev, x, y, b);
  }
}

/** The mouse is released on the canvas */
void Navig::mouseReleaseCB(UMouseEvent& mev)
{
  stopMotion(); // sanity
  
  if (gw.gui.vnc) {		// events are redirected to Vnc
    gw.gui.vnc->mouseEvent(int(mev.getX()), int(mev.getY()), 0);
  }
  else if (gw.gui.selected_object) {
    gw.gui.selected_object->resetRay();
  }
  if (localuser) {
    localuser->resetRay();	//stop showing direction
  }
  opened_menu = null;
}

/** The mouse is dragged on the canvas */
void Navig::mouseDragCB(UMouseEvent& mev)
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

/** The mouse is moved on the canvas */
void Navig::mouseMoveCB(UMouseEvent& mev)
{
  if (gw.gui.vnc) {		// events are redirected to Vnc
    gw.gui.vnc->mouseEvent(int(mev.getX()), int(mev.getY()), 0);
  }
#if 0 //expensive followMouse
  else if (followMouse) {
    // mode followMouse continuously indicates object under pointer
    Object *object = gw.pointedObject(int(mev.getX()), int(mev.getY()), objinfo, depthsel);
    selectObject(object ? objinfo : null);
  }
#endif //expensive followMouse
  if (gw.gui.selected_object && gw.gui.selected_object->isValid()) {
    gw.gui.selected_object->resetFlashy();	// stop flashing edges
    gw.gui.selected_object->resetRay();
  }
}

/** A key is pressed on the canvas */
void Navig::keyPressCB(UKeyEvent& kev)
{
  if (gw.gui.vnc) {	// key events are redirected to Vnc
    char kstr[2];
    kstr[0] = kev.getKeyChar();
    kstr[1] = 0;
    gw.gui.vnc->keyEvent(kstr, true);
  }
  else {		// normal behaviour
    gw.processKey(kev.getKeyCode(), kev.getKeyChar(), true);

    if (! localuser)  return;

    struct timeval t;
    gettimeofday(&t, NULL);
    
    float user_lspeed = User::LSPEED;	// reset lspeed
    localuser->specialAction(User::U_SETLSPEED, &user_lspeed, t.tv_sec, t.tv_usec);
    float user_aspeed = User::ASPEED;	// reset aspeed
    localuser->specialAction(User::U_SETASPEED, &user_aspeed, t.tv_sec, t.tv_usec);
  }
}

/** A key is released on the canvas */
void Navig::keyReleaseCB(UKeyEvent& kev)
{
  if (gw.gui.vnc) {     // key events are redirected to Vnc
    char kstr[2];
    kstr[0] = kev.getKeyChar();
    kstr[1] = 0;
    gw.gui.vnc->keyEvent(kstr, false);
  }
  else {		// normal behaviour
    gw.processKey(kev.getKeyCode(), kev.getKeyChar(), false);
  }
}

/** Press Buttons 1 or 3: navigator or info menu */
void Navig::pressB1orB3(UMouseEvent& ev, int x, int y, int b)
{
  // desactivate previous object
  Object* prev_object = gw.gui.getSelectedObject();
  if (prev_object) {
    prev_object->resetFlashy();
    prev_object->resetRay();
  }
  //Sound::playSound(CLICKSND);

  // current clic
  static uint8_t clic = 0;		// first object
  Object* object = gw.pointedObject(x, y, objinfo, clic % MAX_CLICKS);
  clic++;				// next object hidden in the z buffer

  if (object) {
    gw.gui.selected_object = object;
    //echo("clic on %s", object->objectName());

    gw.message.performRequest(object);
    if (b == 1) {
      // do the click method on the object
      if (gw.gui.vrelet) object->click(x, y);
      if (gw.gui.board)  object->click(x, y);
    }

    selectObject(objinfo);
  
    if (b == 3) {		// show object menu
      object_menu.open(ev);
      opened_menu = object_menu;
    }
    else if (b == 1) {		// navigator
      navig_menu.open(ev);	// show(e, 0, 0); TRASH !!!
      opened_menu = navig_menu;
      object->setFlashy();	// flashes the edges of the object
      object->setRay(x, y);	// launches stipple ray on the object
    }
  }
  else {			// no object!
    gw.setRayDirection(x, y);	// launches ray on x,y screen coord
  }
}

/** Press button 2: fine grain selection */
void Navig::pressB2(int x, int y)
{
  depthsel++;
  Object* object = gw.pointedObject(x, y, objinfo, depthsel);
  if (object) {
    gw.gui.selected_object = object;
    object->resetFlashy();
    object->setFlashy();	// flashes edges of the solid
    object->setRay(x, y);	// launches stipple ray on the object
    selectObject(objinfo);
  }  
  else {			// no object!
    gw.setRayDirection(x, y);	// launches ray on x,y screen coord
  }
}

/** Clears InfoBar */
void Navig::clearInfoBar()
{
  gw.infos_box.removeAll();	// clears infos in infobar
}

/** Updates object infos (infosbox in the infobar and in the contextual menu) */
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

/** User motion */
void Navig::userMotion(UMouseEvent& e, Motion* _motx, Motion *_moty)
{
  xref = e.getX();
  yref = e.getY();
  if (motx)  motx->stop();
  if (moty)  moty->stop();
  motx = _motx;
  moty = _moty;
}

/** Object motion */
void Navig::objectMotion(UMouseEvent& e, Motion* _motx, Motion *_moty)
{
  xref = e.getX();
  yref = e.getY();
  if (motx)  motx->stop();
  if (moty)  moty->stop();
  motx = _motx;
  moty = _moty;
}

/** Do motion */
void Navig::doMotion(UMouseEvent& e)
{
  float dx = e.getX() - xref;
  float dy = e.getY() - yref;
  if (motx)  motx->move(int(dx));
  if (moty)  moty->move(int(dy));
}

/** Stop motion */
void Navig::stopMotion()
{
  if (motx)  motx->stop();
  if (moty)  moty->stop();
  motx = null;
  moty = null;
}

////////////
// NAVIGATOR
////////////

void Navig::navigator()
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
           + UOn::arm      / ucall(this, (Motion*)0, &Motion::o_trans_f, &Navig::objectMotion)
	  )
   + uitem(l + " ")
   + uitem(l + g.theme.Rotzleft	 // TURN LEFT
           + UOn::mpress   / ucall(this, &Navig::mouseRefCB)
           + UOn::mdrag    / ucall(this, &Navig::mouseDragCB)
           + UOn::mrelease / ucall(this, &Navig::mouseReleaseCB)
           + UOn::arm      / ucall(this, (Motion*)0, &Motion::o_rot_l_z, &Navig::objectMotion)
          )
   + uitem(l + " ")
   + uitem(l + g.theme.Rotzright // TURN RIGHT
           + UOn::mpress   / ucall(this, &Navig::mouseRefCB)
           + UOn::mdrag    / ucall(this, &Navig::mouseDragCB)
           + UOn::mrelease / ucall(this, &Navig::mouseReleaseCB)
           + UOn::arm      / ucall(this, (Motion*)0, &Motion::o_rot_r_z, &Navig::objectMotion)
          )
  );
  UTrow& row2 = utrow
  (pad + vs + hs
   + uitem(l + g.theme.Transy	 // LEFT/RIGHT
           + UOn::mpress   / ucall(this, &Navig::mouseRefCB)
           + UOn::mdrag    / ucall(this, &Navig::mouseDragCB)
           + UOn::mrelease / ucall(this, &Navig::mouseReleaseCB)
           + UOn::arm      / ucall(this, (Motion*)0, &Motion::o_trans_l, &Navig::objectMotion)
          )
   + uitem(l + " ")
   + uitem(l+ g.theme.Rotxleft	 // ROLL LEFT
           + UOn::mpress   / ucall(this, &Navig::mouseRefCB)
           + UOn::mdrag    / ucall(this, &Navig::mouseDragCB)
           + UOn::mrelease / ucall(this, &Navig::mouseReleaseCB)
           + UOn::arm      / ucall(this, (Motion*)0, &Motion::o_rot_l_z, &Navig::objectMotion)
          )
   + uitem(l + " ")
   + uitem(l + g.theme.Rotxright // ROLL RIGHT
           + UOn::mpress   / ucall(this, &Navig::mouseRefCB)
           + UOn::mdrag    / ucall(this, &Navig::mouseDragCB)
           + UOn::mrelease / ucall(this, &Navig::mouseReleaseCB)
           + UOn::arm      / ucall(this, (Motion*)0, &Motion::o_rot_r_z, &Navig::objectMotion)
          )
  );
  UTrow& row3 = utrow
  (pad + vs + hs
   + uitem(l + g.theme.Transz	 // UP/DOWN
           + UOn::mpress   / ucall(this, &Navig::mouseRefCB)
           + UOn::mdrag    / ucall(this, &Navig::mouseDragCB)
           + UOn::mrelease / ucall(this, &Navig::mouseReleaseCB)
           + UOn::arm      / ucall(this, (Motion*)0, &Motion::o_trans_u, &Navig::objectMotion)
          )
   + uitem(l + " ")
   + uitem(l + g.theme.Rotyup	 // TILT UP
           + UOn::mpress   / ucall(this, &Navig::mouseRefCB)
           + UOn::mdrag    / ucall(this, &Navig::mouseDragCB)
           + UOn::mrelease / ucall(this, &Navig::mouseReleaseCB)
           + UOn::arm      / ucall(this, (Motion*)0, &Motion::o_rot_u_y, &Navig::objectMotion)
          )
   + uitem(l + " ")
   + uitem(l + g.theme.Rotydown	 // TILT DOWN
           + UOn::mpress   / ucall(this, &Navig::mouseRefCB)
           + UOn::mdrag    / ucall(this, &Navig::mouseDragCB)
           + UOn::mrelease / ucall(this, &Navig::mouseReleaseCB)
           + UOn::arm      / ucall(this, (Motion*)0, &Motion::o_rot_d_y, &Navig::objectMotion)
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
  //echo("ref");
}
