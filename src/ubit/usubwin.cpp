/************************************************************************
 *
 *  usubwin.cpp: window inside a window
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
#include <iostream>
#include <algorithm>
#include <ubit/ucall.hpp>
#include <ubit/usubwin.hpp>
#include <ubit/uwinImpl.hpp>
#include <ubit/uview.hpp>
#include <ubit/ustyle.hpp>
#include <ubit/uappli.hpp>
#include <ubit/uappliImpl.hpp>
#include <ubit/uon.hpp>
#include <ubit/ucolor.hpp>

#include <ubit/nat/udispX11.hpp>  // !!!@@@

using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT


UStyle* USubwin::createStyle() {
  UStyle* s = UWin::createStyle();
  // necessaire, surtout pour UGlcanvas
  s->setBgcolors(UColor::none);
  return s;
}

USubwin::USubwin(UArgs a) : UWin(a) {
  // Contrairement aux UWin, sont affichees par defaut et sont en mode BOX (= tiled)
  emodes.IS_SHOWABLE = true;
  wmodes.IS_HARDWIN = true;
  //emodes.IS_SUBWIN = true; (remplacé par virtual method isSubWin())
}

// must be redefined to init parent view
// void USubwin::initView(UChild *selflink, UView *parview) {
  void USubwin::initView(UView* parview) {
  UWin::initViewImpl(/*selflink,*/ parview, parview->getDisp());
  
  // as the subwin is laid out as a normal UBox, it must have
  // a parent view (in constrast with other Wins that do not have one)
  // (see getViewInside())
  UView* winview = getWinView(parview->getDisp());
  
  // le parent de la vue doit etre mis a jour (pour les softwins only)
  // MAIS PAS LE CHAMP win !!
  if (winview) winview->parview = parview;
  if (isShowable()) show(true);
}

bool USubwin::realize() {
  if (! wmodes.IS_HARDWIN) {
    UAppli::error("USubwin::realize","can't be a SOFTWIN: can't realize object %p",this);
    return false;
  }
  
  UHardwinImpl* hw = hardImpl();
  if (!hw) return false;
  if (hw->isRealized()) return true;  // deja realize'
  if (!realizeHardwin(UHardwinImpl::SUBWIN)) return false;
  
  //cerr << "SUBWIN win " <<((UHardwinImplX11*)hw)->getSysWin() <<endl;
  addAttr(UOn::motion / ucall(this, &USubwin::motionImpl));
  addAttr(UOn::resize / ucall(this, &USubwin::resizeImpl));
    
#if UBIT_WITH_X11
  UHardwinX11* _hw = (UHardwinX11*)hw;
  // this makes it possible for the GL window to receive events sent by the UMS
  unsigned char winid[2] = {UHardwinImpl::SUBWIN, 0};
  Atom UMS_WINDOW = ((UDispX11*)_hw->getDisp())->getAtoms().UMS_WINDOW;
  XChangeProperty(_hw->getSysDisp(), _hw->getSysWin(), UMS_WINDOW,
                  XA_STRING, 8, PropModeReplace, winid, 2);
#endif
  return true;
}

void USubwin::resizeImpl(UResizeEvent& e) {
  UView* v = e.getView();
  UHardwinImpl* hw = hardImpl();
  if (hw) {
    // setSize first because pos depends on size for GLUT
    hw->setSize(v->getSize());
    //UPoint p = hw->getPos();
    //cerr << "resizeCB size: " << v->getSize() << " p= " << p<< endl; 
  }
}
  
void USubwin::motionImpl(UViewEvent& e) {
  UView* v = e.getView();
  UHardwinImpl* hw = hardImpl();
  if (hw) {
    hw->setPos(v->getHardwinPos());
    //UPoint p = hw->getPos(p);
    //cerr << "motionCB pos: " << v->getWinPos() << " p= " << p<<  endl; 
  }
}

}
