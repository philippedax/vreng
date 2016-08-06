/************************************************************************
 *
 *  uscrollpane.cpp
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2008 | Eric Lecolinet | ENST Paris | www.enst.fr/~elc/ubit
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
#include <ubit/uon.hpp>
#include <ubit/ucall.hpp>
#include <ubit/uboxgeom.hpp>
#include <ubit/ubox.hpp>
#include <ubit/uscrollbar.hpp>
#include <ubit/uscrollpane.hpp>
#include <ubit/ustyle.hpp>
#include <ubit/uappli.hpp>
#include <ubit/uconf.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT


UStyle* UScrollpane::createStyle() {
  UStyle& s = *new UStyle();
  s.viewStyle = &UPaneView::style;
  s.orient = UOrient::HORIZONTAL;
  s.halign = UHalign::FLEX;
  s.valign = UValign::FLEX;
  s.hspacing = 1;
  s.vspacing = 1;
  // keep initial size
  s.local.size.width  = -1;
  s.local.size.height = -1;
  return &s;
}

/* ==================================================== ======== ======= */

UScrollpane::~UScrollpane() {
  //!!Att: scrollbar pas forcement enfnats du Pane !
  // (seront automatiquement detruit via le DAG inutile de s'en charger ici)
  if (vscrollbar) vscrollbar->setPane(null); vscrollbar = null;
  if (hscrollbar) hscrollbar->setPane(null); hscrollbar = null;
}

UScrollpane::UScrollpane(UArgs a) : 
UBox(a), hscrollbar(null), vscrollbar(null) {
  addAttr(UOn::resize / ucall(this, &UScrollpane::resizeCB));
  constructs(true, true, a);
}

UScrollpane::UScrollpane(int vs_mode, int hs_mode, UArgs a) :
UBox(a), hscrollbar(null), vscrollbar(null) {
  addAttr(UOn::resize / ucall(this, &UScrollpane::resizeCB));
  constructs(vs_mode, hs_mode, a);
}

void UScrollpane::constructs(int vs_mode, int hs_mode, const UArgs&) {
  UArgs ba;

  if (vs_mode) {
    vscrollbar = &uvscrollbar();
    if (UAppli::conf.transp_scrollbars) vscrollbar->setTransparent(true);
    ba += UValign::flex + UHalign::right + vscrollbar;
    vscrollbar->setPane(this);
  }

  if (hs_mode) {
    hscrollbar = &uhscrollbar();
    if (UAppli::conf.transp_scrollbars) hscrollbar->setTransparent(true);
    ba += UValign::bottom + UHalign::flex + hscrollbar;
    hscrollbar->setPane(this);
  }

  UBorder* border = new UCompositeBorder(ba);
  setAttr(*border);
  if (UAppli::conf.transp_scrollbars) border->setOverlaid(true);
  
  // the mouse wheel scrolls the pane
  observeChildrenEvents(UOn::wheel / ucall(this, &UScrollpane::wheelCB));
}

/* ==================================================== ======== ======= */

void UScrollpane::setTracking(bool state) {
  if (hscrollbar) hscrollbar->setTracking(state);
  if (vscrollbar) vscrollbar->setTracking(state);
}

UScrollpane& UScrollpane::showHScrollbar(bool state) {
  if (hscrollbar) hscrollbar->show(state);
  return *this;
}

UScrollpane& UScrollpane::showVScrollbar(bool state) {
  if (vscrollbar) vscrollbar->show(state);
  return *this;
}
  
UScrollpane& UScrollpane::showHScrollButtons(bool state) {
  UScrollbar* sb = hscrollbar;
  if (sb) {
    if (sb->getLessButton()) sb->getLessButton()->show(state);
    if (sb->getMoreButton()) sb->getMoreButton()->show(state);
  }
  return *this;
}

UScrollpane& UScrollpane::showVScrollButtons(bool state) {
  UScrollbar* sb = vscrollbar;
  if (sb) {
    if (sb->getLessButton()) sb->getLessButton()->show(state);
    if (sb->getMoreButton()) sb->getMoreButton()->show(state);
  }
  return *this;
}

void UScrollpane::unsetHScrollbar() {
  hscrollbar = null;
}

void UScrollpane::unsetVScrollbar() {
  vscrollbar = null;
}

UBox* UScrollpane::getScrolledBox() {
  for (UChildIter c = cbegin(); c != cend(); ++c) {
    UBox* box = (*c)->toBox();
    if (box) return box;
  }
  return null;    // not found
}

UView *UScrollpane::getScrolledView(UView* pane_view) {
  UBox* child = getScrolledBox();
  return (child ? child->getViewInImpl(pane_view /*,null*/) : null);
}

void UScrollpane::wheelCB(UWheelEvent& e) {
  if (vscrollbar) {
    if (e.getWheelDelta() > 0) vscrollbar->pressScrollButton(e,-1);
    else if (e.getWheelDelta() < 0) vscrollbar->pressScrollButton(e,+1);
  }
}

// Set the Vertical and Horizontal Scrollbars
// Notes:
// - if 'add_to_pane' is true (default case), the scrollbar is added to
//   the Pane child list and it will appear inside the Pane.
// - otherwise, the scrollbar is NOT added to the Pane and MUST be added
//   as a child of another box. This makes it possible to CONTROL a Pane
//   by a scrollbar that is NOT included in this Pane.

/*
void UPane::setVScrollbar(UScrollbar *sc, bool add_to_pane) {
  // enleve le scrollbar du Pane (s'il y est: sinon ne fait rien)
  // mais NE LE DETRUIT PAS (car on ne sait pas qui a cree ce scrollbar:
  // il n'a pas forcement ete cree par le UPane ou le Scrollpane)
  if (vscrollbar) {
    vscrollbar->setPane(null);
    remove(vscrollbar, false);
  }
  vscrollbar = sc;
  if (sc) {
    sc->setPane(this);
    if (add_to_pane) {
      add(UValign::flex);
      add(UHalign::right);
      add(sc);
      // pour que les add() suivants mettent les enfants en zone centrale
      add(UHalign::flex);
    }
  }
}
*/

void UScrollpane::makeVisible(UBox& child) {
  bool move_x = false, move_y = false;
  
  for (UView* v = child.getView(0); v != null; v = v->getNext()) {
    UBox* list = getScrolledBox();
    UView* listv = list ? list->getViewContaining(*v) : null;
    UView* panev = getViewContaining(*v);
    
    if (listv && panev
        && listv->getHeight() > 0 && listv->getWidth() > 0 //ICI isShown() serait utile!!!
        && panev->getHeight() > 0 && panev->getWidth() > 0
        ) {
      UPoint pane_pos = UView::convertPosTo(*panev, *v, UPoint(0,0));
      
      if (pane_pos.x + v->getWidth() > panev->getWidth()) move_x = true;
      else if (pane_pos.x < 0) move_x = true;
      
      if (pane_pos.y + v->getHeight() > panev->getHeight()) move_y = true;
      else if (pane_pos.y < 0) move_y = true;
      
      if (move_x || move_y) {
        UPoint list_pos = UView::convertPosTo(*listv, *v, UPoint(0,0));
        if (move_x) {
          float _xscroll = (float)list_pos.x / listv->getWidth() * 100;
          setScrollImpl(_xscroll, yscroll);
        }
        if (move_y) {
          float _yscroll = (float)list_pos.y / listv->getHeight() * 100;
          setScrollImpl(xscroll, _yscroll);
        }
      }
    }
  }
}

/* ==================================================== [Elc] ======= */
// synchronizes the scrollbars (att aux loops!)

UScrollpane& UScrollpane::setScroll(float _xscroll, float _yscroll) {
  // NO_DELAY necessaire pour update immediat par appel de setScroll()
  //update(UUpdate::LAYOUT | UUpdate::NO_DELAY);
  doUpdate(UUpdate::LAYOUT);
  setScrollImpl(_xscroll, _yscroll);
  return *this;
}

void UScrollpane::setScrollImpl(float _xscroll, float _yscroll) {
  if (_xscroll < 0) _xscroll = 0; else if (_xscroll > 100) _xscroll = 100;
  if (_yscroll < 0) _yscroll = 0; else if (_yscroll > 100) _yscroll = 100;
  //cerr << _xscroll <<" " << xscroll  <<" " <<_yscroll  <<" " << yscroll<<endl;
  
  // 20oct08: attempt to avoid losing a lot of time when limits are reached
  if (xscroll == _xscroll && yscroll == _yscroll) return;
  
  float xoffset_max = 0, yoffset_max = 0;  // pour toutes les Views de 'pane' 
  
  for (UView* view = views; view != null; view = view->getNext()) {
    UPaneView* pane_view = dynamic_cast<UPaneView*>(view);  // UPaneView par constr
    UView *winview = null;
    
    if (pane_view && (winview = pane_view->getWinView())) {
      UView* viewport_view = getScrolledView(pane_view);
      
      if (viewport_view) {
        UPaintEvent e(UOn::paint, winview);
        e.setSourceAndProps(viewport_view);
        float xoffset = 0, yoffset = 0;
        float deltax = 0, deltay = 0;
        
        // horizontal scroll
        float viewport_w = pane_view->getWidth()
        - pane_view->padding.left.val - pane_view->padding.right.val;
        
        xoffset = _xscroll * (viewport_view->getWidth() - viewport_w) / 100.0; // + 0.5;
        if (xoffset >= 0) {
          deltax = xoffset - pane_view->getXScroll();
          pane_view->setXScroll(xoffset);
        }
        
        // vertical scroll
        float viewport_h = pane_view->getHeight()
        - pane_view->padding.top.val - pane_view->padding.bottom.val;
        
        yoffset = _yscroll * (viewport_view->getHeight() - viewport_h) / 100.0; // + 0.5;
        if (yoffset >= 0) {
          deltay = yoffset - pane_view->getYScroll();
          pane_view->setYScroll(yoffset);
        }
        
        if (xoffset >= 0 || yoffset >= 0) {
          xoffset_max = std::max(xoffset_max, xoffset);
          yoffset_max = std::max(yoffset_max, yoffset);
          //viewport_view->getBox()->repaint();
        }
      }
    }
  }

  // correction du scroll en fonction des contraintes du pane
  // (typiquement si le viewport est plus petit que le scrollpane)
  if (xoffset_max == 0) xscroll = 0; else xscroll = _xscroll;
  if (yoffset_max == 0) yscroll = 0; else yscroll = _yscroll;

  // NB: verif callbacks OK
  if (hscrollbar) *hscrollbar->pvalue = xscroll;
  if (vscrollbar) *vscrollbar->pvalue = yscroll;
  repaint();
  
  /*
   cette fonction fait n'importe quoi
  // update scrollbars
  if (hscrollbar) hscrollbar->setValueImpl(xscroll, true);
  if (vscrollbar) vscrollbar->setValueImpl(yscroll, true);
*/
}

/* ==================================================== ======== ======= */

void UScrollpane::resizeCB(UResizeEvent& e) {
  UPaneView *pane_view  = dynamic_cast<UPaneView*>(e.getView());

    // !!! A COMPLETER !!! prendre en compte les Units 
  
  if (hscrollbar) {
    float viewport_w = pane_view->getWidth()
    - pane_view->padding.left.val - pane_view->padding.right.val;

    UView *viewport_view = getScrolledView(pane_view);
    if (!viewport_view) return;

    // corrbug div by 0
    if (viewport_view->getWidth() == viewport_w) hscrollbar->setValue(0);
    else {
      float xoffset 
      = hscrollbar->getValue() * (viewport_view->getWidth()-viewport_w) /100.0;

      float newscroll = (float)xoffset / (viewport_view->getWidth()-viewport_w) *100.0;
      
      hscrollbar->setValue(newscroll);
    }
  }

  if (vscrollbar) {
    float viewport_h = pane_view->getHeight()
    - pane_view->padding.top.val - pane_view->padding.bottom.val;

    UView *viewport_view = getScrolledView(pane_view);
    if (!viewport_view) return;
    
    if (viewport_view->getHeight() == viewport_h) vscrollbar->setValue(0);
    else {
      float yoffset = 
      vscrollbar->getValue() * (viewport_view->getHeight()-viewport_h) /100.0;
      
      float newscroll = (float)yoffset / (viewport_view->getHeight()-viewport_h) *100.0;

      vscrollbar->setValue(newscroll);
    }
  }
}

/* ==================================================== ===== ======= */

UViewStyle UPaneView::style(&UPaneView::createView, UCONST);

UPaneView::UPaneView(UBox* box, UView* parview, UHardwinImpl* w) : 
  UView(box, parview, w), 
  padding(0,0),
  xscroll(0),
  yscroll(0) {
}

// "static" constructor used by UViewStyle to make a new view
UView* UPaneView::createView(UBox* box, UView* parview, UHardwinImpl* w) {
  return new UPaneView(box, parview, w);
}

}

