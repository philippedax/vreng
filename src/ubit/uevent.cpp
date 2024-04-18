/************************************************************************
 *
 *  uevent.cpp: Ubit Event
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2009 | Eric Lecolinet | ENST Paris | www.enst.fr/~elc/ubit
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
#include <ubit/ugl.hpp>
#include <ubit/ucall.hpp>
#include <ubit/uon.hpp>
#include <ubit/ubox.hpp>
#include <ubit/ustr.hpp>
#include <ubit/ukey.hpp>
#include <ubit/uupdatecontext.hpp>
#include <ubit/uviewImpl.hpp>
#include <ubit/uwinImpl.hpp>
#include <ubit/ucolor.hpp>
#include <ubit/ugraph.hpp>
#include <ubit/uappliImpl.hpp>
#include <ubit/ueventflow.hpp>
#include <ubit/umsproto.hpp>
#include <ubit/umessage.hpp>
#include <ubit/uflag.hpp>

using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

static const int _BUTTONS = 
  UModifier::LeftButton | UModifier::MidButton | UModifier::RightButton;

UEvent::UEvent(const UCond& c, UObject* s, UObject* a)
: cond(&c), source(s), aux(a) {} 

UEvent::~UEvent() {}
 
void UEvent::setCond(const UCond& c) {cond = &c;}

UWinEvent::UWinEvent(const UCond& c, UView* v, int t, int s) : 
UEvent(c, (v? v->getBox(): null)), 
source_view(v), 
type(t), 
state(s) {
}

USysWMEvent::USysWMEvent(const UCond& c, UView* v, void* sev) : 
UEvent(c, (v? v->getBox(): null)), 
source_view(v), 
sys_event(sev) {
}

UInputEvent::UInputEvent(const UCond& c, UView* v, UEventFlow* f, 
                         unsigned long time, int st) : 
UEvent(c, (v? v->getBox(): null)), 
state(st), 
when(time),
source_view(v),
eflow(f),
event_observer(null) {
  modes.PROPAGATE = false;
  modes.IS_MDRAG = modes.IS_FIRST_MDRAG = modes.IS_BROWSING 
  = modes.DONT_CLOSE_MENU = modes.SOURCE_IN_MENU = false;
} 

UElem* UInputEvent::getSource() const {
  return source_view ? source_view->getBox() : null;
}

UView* UInputEvent::getViewOf(const UBox& box) const {
  return box.getView(*this);
}

UView* UInputEvent::getWinView() const {
  return source_view ? source_view->getWinView() : null;
}
  
UWin* UInputEvent::getWin() const {
  return (source_view && source_view->hardwin) ? source_view->hardwin->win : null;
}

UDisp* UInputEvent::getDisp() const {
  return (source_view && source_view->hardwin) ? source_view->hardwin->disp : null;
}
  
int UInputEvent::getModifiers() const {return state;}

bool UInputEvent::isShiftDown() const 
{return (getModifiers() & UModifier::ShiftDown) != 0;}

bool UInputEvent::isControlDown() const 
{return (getModifiers() & UModifier::ControlDown) != 0;}

bool UInputEvent::isAltDown() const 
{return (getModifiers() & UModifier::AltDown) != 0;}

bool UInputEvent::isMetaDown() const
{return (getModifiers() & UModifier::MetaDown) != 0;}

bool UInputEvent::isAltGraphDown() const 
{return (getModifiers() & UModifier::AltGraphDown) != 0;}

UMouseEvent::UMouseEvent(const UCond& c, UView* source, UEventFlow* f, 
                         unsigned long time, int state,
                         const UPoint& pos, const UPoint& abs_pos, int btn) :
UInputEvent(c, source, f, time, state), 
button(btn), click_count(0),
pos(pos), abs_pos(abs_pos) {
}
  
int UMouseEvent::getButtons() const {
  return state & _BUTTONS;
}

void UMouseEvent::translatePos(float x, float y) {  // NB: change pas source !
  pos.x += x; pos.y += y;
  abs_pos.x += x; abs_pos.y += y;
}

UPoint UMouseEvent::getPosIn(const UView& view) const {
  if (!source_view) return UPoint(0,0);
  else return UView::convertPosTo(view, *source_view, pos);
}

UPoint UMouseEvent::getPosIn(const UBox& parent) const {
  if (!source_view) return UPoint(0,0);
  UView* v = parent.getViewContaining(*source_view);
  if (v) return UView::convertPosTo(*v, *source_view, pos);
  else return UPoint(0,0);
}

UPoint UMouseEvent::getPosAndViewIn(const UBox& parent, UView*& parent_view) const {
  if (!source_view) return UPoint(0,0);
  parent_view = parent.getViewContaining(*source_view);
  if (parent_view) return UView::convertPosTo(*parent_view, *source_view, pos);
  else return UPoint(0,0);
}

UData* UMouseEvent::getData() {
  if (!source_view) return null;
  UDataContext dc;
  return source_view->findData(dc, pos, null, 0, -1);
}

UData* UMouseEvent::getData(UDataContext& dc) {
  if (!source_view) return null;
  return source_view->findData(dc, pos, null, 0, -1);
}

// same as findData() but discards UDatas that are not Strings
UStr* UMouseEvent::getStr() {
  if (!source_view) return null;
  UDataContext dc;
  UData* data = getData(dc);
  return (data ? data->toStr() : null);
}

UStr* UMouseEvent::getStr(UDataContext& dc) {
  if (!source_view) return null;
  UData* data = getData(dc);
  return (data ? data->toStr() : null);
}

UWheelEvent::UWheelEvent(const UCond& c, UView* source, UEventFlow* f, 
                         unsigned long time, int state,
                         const UPoint& pos, const UPoint& abs_pos, 
                         int wheel_btn, int wheel_delta) :
UMouseEvent(c, source, f, time, state, pos, abs_pos, wheel_btn), 
delta(wheel_delta) {
} 
    
UKeyEvent::UKeyEvent(const UCond& c, UView* source, UEventFlow* f, unsigned long when, 
                     int mods, int kcode, short kchar)
: UInputEvent(c, source, f, when, mods), keycode(kcode), keychar(kchar) {}

void UKeyEvent::setKeyChar(short ch) {keychar = ch;}
void UKeyEvent::setKeyCode(int ks) {keycode = ks;}

UViewEvent::UViewEvent(const UCond& c, UView* v) : 
UEvent(c, (v? v->getBox(): null)), 
source_view(v) {
}
  
UBox* UViewEvent::getSource() const {
  return source_view ? source_view->getBox() : null;
}

UDisp* UViewEvent::getDisp() const {
  UHardwinImpl* hw = null;
  if (source_view && (hw = source_view->getHardwin())) return hw->getDisp();
  else return null;
}

UTimerEvent::UTimerEvent(const UCond& c, UTimer* t, unsigned long w) 
: UEvent(c, t), /*timer(t),*/ when(w) {}

UUserEvent::UUserEvent(const UCond& c, UObject* s, int t, void* d) 
: UEvent(c, s), /*source(s),*/ type(t), data(d) {}

UMessageEvent::UMessageEvent(const UCond& c, UMessagePort* p) 
: UEvent(c, p) /*,port(p)*/ {}

const UStr* UMessageEvent::getMessage() const {
  //if (port) return &port->getValue(); else return null;
  if (source) return &((UMessagePort*)source)->getValue(); 
  else return null;
}

UResizeEvent::UResizeEvent(const UCond& c, UView* v) : UViewEvent(c, v) {} 

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UPaintEvent::UPaintEvent(const UCond& c, UView* v, const URect* winclip) : 
UViewEvent(c, v),
current_context(null),
layout_view(null)
{
  if (winclip) {
    is_clip_set = true;
    redraw_clip = *winclip;
  }
  else {
    is_clip_set = false;
    redraw_clip.setRect(0,0,0,0);
  }
} 
 
bool UPaintEvent::setSourceAndProps(UView* v) {
  UViewContext vc;
  bool stat = v->findContext(vc, UView::FIND_CLIP);
  source_view = v;
  setProps(vc);
  return stat;
}

void UPaintEvent::setProps(const UViewContext& vc) {
  is_clip_set = vc.is_clip_set;
  redraw_clip = vc.clip;
  layout_view = vc.layout_view;
}
  
void UPaintEvent::getClip(URect& r) const {
  if (source_view && is_clip_set) {
    r.x = redraw_clip.x - source_view->x;
    r.y = redraw_clip.x - source_view->y;
    r.width  = redraw_clip.width;
    r.height = redraw_clip.height;
  }
  else if (source_view) {
    r.x = r.y = 0;
    r.width  = source_view->width;
    r.height = source_view->height;
  }
  else {
    r.x = r.y = 0;
    r.width = r.height = 0;
  }  
}

URect UDataContext::getBounds() const {
  if (!source_view) return URect();
  else return URect(win_clip.x - source_view->x,  //- dataContext->getView()->x, 
                    win_clip.y - source_view->y,  //- dataContext->getView()->y, 
                    win_clip.width, win_clip.height);
}

UDataContext::UDataContext() {
  data = null;
  it = it2 = UChildIter();
  strpos = -1;
  strpos2 = -1;       // !must be se to -1 if unused
  exact_match = false;
  //winclip.setRect(0,0,0,0); // !important pour set/union
  //dataContext = null;  // !important pour ~UDataContext
}

UDataContext::~UDataContext() {
  //delete winContext;
  //delete dataContext;
}

// -- exact_match   = true  : Data is exactly located under the Mouse
//                    false : Data is the last Data before the Mouse position
// -- merge_regions = true  : region 'r' is merged with the previous 'region'
//                    false : region 'r' replaces the previous value of 'region'
//
void UDataContext::set(UUpdateContext& ctx, UData* d, UChildIter _it, UChildIter end,
                       const URect& r, int _strpos, bool _exact_match) {
  data = d;
  it = _it;
  it2 = end;
  strpos = _strpos;   //don't change strpos2!
  //if (!dataContext) dataContext = new UUpdateContext(ctx);
  exact_match = _exact_match;
  win_clip = r;
}

void UDataContext::merge(UUpdateContext& ctx, UData* d, UChildIter _it, UChildIter end,
                       const URect& r, bool _exact_match) {
  data = d;
  it = _it;
  it2 = end;  //don't change strpos and strpos2!
  //if (!dataContext) dataContext = new UUpdateContext(ctx);
  exact_match = _exact_match;
  win_clip.doUnion(r);
}

}
