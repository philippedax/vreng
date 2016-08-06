/************************************************************************
 *
 *  uctlmenu.cpp : Control menus (see upiemenu.hpp)
 *  Ubit GUI Toolkit - Version 6
 *  (C) 2009 | Eric Lecolinet | TELECOM ParisTech | http://www.enst.fr/~elc/ubit
 *
 * ***********************************************************************
 * COPYRIGHT NOTICE :
 * THIS PROGRAM IS DISTRIBUTED WITHOUT ANY WARRANTY AND WITHOUT EVEN THE
 * IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT UNDER THE TERMS OF THE GNU
 * GENERAL PUBLIC LICENSE AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION;
 * EITHER VERSION 2 OF THE LICENSE, OR (AT YOUR OPTION) ANY LATER VERSION.
 * SEE FILES 'COPYRIGHT' AND 'COPYING' FOR MORE DETAILS.
 * ***********************************************************************/

#include <ubit/ubit_features.h>
#include <vector>
#include <iostream>
#include <cmath>
#include <ubit/ubit.hpp>
#include <ubit/uzoom.hpp>
#include <ubit/uctlmenu.hpp>
//#include <ubit/ueventImpl.hpp>  // TST
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT
  

UStyle* UCtlmenu::createStyle() {
  UStyle* style = UMenu::createStyle();
  style->local.alpha = 0.;
  style->local.border = null;
  style->local.padding.set(1, 1);
  style->local.background = null;
  style->setBgcolors(UColor::none);
  return style;
}

UCtlmenu::UCtlmenu() {
  spring_mode = false;
  ctlmenu_mode = true;
}

// ==================================================== [Ubit Toolkit] =========

void UCtlAction::operator()(UEvent& e) {
  // NB: getSource() renvoie le menu
  UCtlmenu* m = dynamic_cast<UCtlmenu*>(e.getSource());
  if (m && e.getCond() == UOn::mdrag) mdrag((UMouseEvent&)e, *m);
}

void UCtlAction::addingTo(UChild& child, UElem& parent) {
  /*
   static UMultiCond* mc = null;
   if (!mc) {
     mc = new UMultiCond;
     mc->add(UOn::mdrag);
     mc->add(UOn::arm);
     mc->add(UOn::disarm);
   }
   */
  
  // sert a specifier les conditions par defaut
  if (!child.getCond()) child.setCond(UOn::mdrag);
  
  // NB: c'est bien UNode::addingTo qu'il faut appeler sinon la cond par
  // defaut serait UOn::action
  UNode::addingTo(child, parent);
}

// ==================================================== [Ubit Toolkit] =========

//UZoomAction::UZoomAction(UZoompane& zbox) 
//: pscale(&zbox.scale()), ppos(&zbox.pos()), pbox(&zbox) {}

UZoomAction::UZoomAction(UBox& zoomed_box, float _gain) : 
UCtlAction(_gain), 
zbox(zoomed_box),
posAttr(zoomed_box.obtainAttr<UPos>()),
scaleAttr(zoomed_box.obtainAttr<UScale>()) {
}

void UZoomAction::mdrag(UMouseEvent& e, UCtlmenu& m) {
  UView* menuview = e.getView();
  if (!menuview) return;
  
  if (e.isFirstDrag()) {
    mouse_delta = 0.;  

    // Unit ignoré : peut poser probleme !!!
    // NB: posAttr est normalisee donc independante du scale du conteneur parent
    zbox_pos0.set(posAttr->getX().val, posAttr->getY().val);
    zbox_scale0 = *scaleAttr;
    
    UView* zbox_view = zbox->getView(e);
    if (!zbox_view) {
      zbox_view = zbox->getView(0);
      //cerr << "UZoomAction: null box view"<<endl;
      //return;
    }
    mouse_in_zbox0 = e.getPosIn(*zbox_view);
    
    //cerr <<"PRESS: "<< mouse_in_zbox0.x <<" / : "<< (zbox_pos0.x * zbox_scale0)<< endl;
    /*
    UPoint view_scrpos = box_view->getScreenPos();
    UPoint mouse_scrpos = e.getScreenPos();
    mousepos.x = (mouse_scrpos.x - view_scrpos.x);
    mousepos.y = (mouse_scrpos.y - view_scrpos.y);
     */
   }
  else {
    float d = m.getMouseMovement().x * xmag;
    if (d == mouse_delta) return;
    mouse_delta = d;
    
    float scale = 1.;
    if (mouse_delta > 0) scale = zbox_scale0 * (1 + mouse_delta / 100);
    else if (mouse_delta < 0) scale = zbox_scale0 / (1 - mouse_delta / 100);
    else return;

    //cerr <<"drag: "<< (zbox_pos0.x * zbox_scale0 + (1- scale/zbox_scale0) * mouse_in_zbox0.x) / scale << endl;

    zbox->setAutoUpdate(false);
    posAttr->set((zbox_pos0.x + (1- scale/zbox_scale0) * mouse_in_zbox0.x),
                 (zbox_pos0.y + (1- scale/zbox_scale0) * mouse_in_zbox0.y));
    *scaleAttr = scale;
    zbox->setAutoUpdate(true);

    e.getWin()->update(); // pour que l'objet zoomé se retaille correctement
  }
}

// ==================================================== [Ubit Toolkit] =========

UPanAction::UPanAction(UBox& panned_box, float _gain) : 
UCtlAction(_gain), 
box(panned_box),
posAttr(panned_box.obtainAttr<UPos>()) {
}

void UPanAction::mdrag(UMouseEvent& e, UCtlmenu& m) {
  if (e.isFirstDrag()) {
    // Unit ignoré : peut poser probleme !!!
    pos0.set(posAttr->getX().val, posAttr->getY().val);   // !!! convesrion afaire

    UView* box_view = box->getView(e);
    if (!box_view) return;
    // fait le contraire de ce qu'on veut
    //container_scale = box_view->getParentView()->getScale();
  }
  else {
    UPoint newpos;
    newpos.x = pos0.x + m.getMouseMovement().x * xmag; // / container_scale;
    newpos.y = pos0.y + m.getMouseMovement().y * ymag; // / container_scale;
    if (*posAttr == newpos) return;
    *posAttr = newpos;
  }
}

// ==================================================== [Ubit Toolkit] =========

UScrollAction::UScrollAction(UScrollpane& _pane, float _gain)
: UCtlAction(_gain), pane(&_pane) {}

UScrollAction::UScrollAction(float _gain)
: UCtlAction(_gain), pane(null) {}

void UScrollAction::setPane(UScrollpane& p) {pane = &p;}

void UScrollAction::mdrag(UMouseEvent& e, UCtlmenu& m) {
  if (!pane) return;
  
  if (e.isFirstDrag()) {
    arm_xpos = pane->getXScroll();
    arm_ypos = pane->getYScroll();
    xpos = ypos = 0;  
  }
  else {
    float xs = arm_xpos + m.getMouseMovement().x * xmag;
    if (xs > 100) xs = 100.; else if (xs < 0) xs = 0.;
  
    float ys = arm_ypos + m.getMouseMovement().y * ymag;
    if (ys > 100) ys = 100.; else if (ys < 0) ys = 0.;
  
    if (xs == xpos && ys == ypos) return;
    xpos = xs;
    ypos = ys;
    pane->setScrollImpl(xpos, ypos);  //cf. UPane::setScroll()
   // update paint (dont update geometry): necessaire dans ce cas
    pane->update(UUpdate::paint);
  }
}

}


